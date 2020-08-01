#include "vtkCPVTKPipeline.h"

#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRenderView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkNew.h>
#include <vtkCellDataToPointData.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkScalarsToColors.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkWindowToImageFilter.h>
#include <vtkMultiProcessController.h>
#include <vtkCompositeRenderManager.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkTextActor.h>
#include <vtkNamedColors.h>
#include <vtkThreshold.h>
#include <vtkDataObject.h>

#include <array>
#include <string>

vtkStandardNewMacro(vtkCPVTKPipeline);

vtkCPVTKPipeline::vtkCPVTKPipeline()
{
  this->OutputFrequency = 0;
}

vtkCPVTKPipeline::~vtkCPVTKPipeline()
{
}

void vtkCPVTKPipeline::Initialize(int outputFrequency)
{
  this->OutputFrequency = outputFrequency;
}

int vtkCPVTKPipeline::RequestDataDescription(vtkCPDataDescription* dataDescription)
{
  if (!dataDescription)
  {
    vtkWarningMacro("dataDescription is NULL.");
    return 0;
  }

  if (dataDescription->GetForceOutput() == true ||
    (this->OutputFrequency != 0 && dataDescription->GetTimeStep() % this->OutputFrequency == 0))
  {
    dataDescription->GetInputDescriptionByName("input")->AllFieldsOn();
    dataDescription->GetInputDescriptionByName("input")->GenerateMeshOn();
    return 1;
  }
  return 0;
}

int vtkCPVTKPipeline::CoProcess(vtkCPDataDescription* dataDescription)
{
  if (!dataDescription)
  {
    vtkWarningMacro("DataDescription is NULL");
    return 0;
  }

  vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast(
    dataDescription->GetInputDescriptionByName("input")->GetGrid());

  if (grid == NULL)
  {
    std::cout << "DataDescription is missing input unstructured grid.\n";
  }

  if (this->RequestDataDescription(dataDescription) == 0)
  {
    return 1;
  }

  // MPI process has been initialized in Fortran simulation code, so we don't
  // have to initialize a new vtkMultiProcessController object.
  vtkMultiProcessController* controller = vtkMultiProcessController::GetGlobalController();
  int myId = controller->GetLocalProcessId();
  int numProcs = controller->GetNumberOfProcesses();

  std::cout << "Current timestep: " << dataDescription->GetTimeStep() << endl;
  std::cout << "LocalProcessID: " << myId << endl;

  std::string ext = ".png";
  auto imgwriter = vtkSmartPointer<vtkImageWriter>::New();
  if (ext == ".jpg") {
    imgwriter = vtkSmartPointer<vtkJPEGWriter>::New();
  } else {
    imgwriter = vtkSmartPointer<vtkPNGWriter>::New();
  }

  vtkRenderer* renderer = vtkRenderer::New();
  vtkRenderWindow* renderWindow = vtkRenderWindow::New();

  renderWindow->SetStereoTypeToCrystalEyes();

  vtkCamera* camera = vtkCamera::New();

  std::cout << "UnstructuredGrid number of cells: " <<
      grid->GetNumberOfCells() << "\n";

  vtkThreshold* threshold = vtkThreshold::New();
  threshold->SetInputData(grid);
  threshold->ThresholdBetween(0.1, 0.5);
  threshold->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "Q");
  threshold->Update();

  // Data copy from vtkUnstructuredGrid to vtkPolyData. To be freed later so as
  // to prevent memory leak.
  vtkGeometryFilter* geometryFilter = vtkGeometryFilter::New();
  geometryFilter->SetInputData(threshold->GetOutput());
  geometryFilter->MergingOff();
  geometryFilter->Update();

  vtkTextProperty* text_property = vtkTextProperty::New();
  text_property->SetJustificationToCentered();
  text_property->SetVerticalJustificationToBottom();
  text_property->SetFontSize(0);
  text_property->BoldOff();

  vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

  vtkTextActor* title = vtkTextActor::New();
  title->SetInput("UCNS3D-Catalyst");

  vtkTextProperty* title_txtprop = title->GetTextProperty();
  title_txtprop->BoldOn();
  title_txtprop->ShadowOn();
  title_txtprop->SetShadowOffset(4, 4);
  title_txtprop->SetColor(colors->GetColor3d("White").GetData());

  vtkTextActor* quantity = vtkTextActor::New();
  quantity->SetInput("Q-Criterion");
  quantity->SetDisplayPosition(400, 10);
  vtkTextProperty *quantity_txtprop = quantity->GetTextProperty();
  quantity_txtprop->BoldOn();
  quantity_txtprop->ShadowOn();
  quantity_txtprop->SetColor(colors->GetColor3d("White").GetData());

  std::string time_input = "Time: ";
  time_input += std::to_string(dataDescription->GetTime());
  time_input += " s";

  char input[time_input.length() + 1];
  for (int x = 0; x < sizeof(input); x++) {
      input[x] = time_input[x];
  }
  const char* input_string = input;

  vtkTextActor* time = vtkTextActor::New();
  time->SetInput(input_string);

  vtkTextProperty *time_txtprop = time->GetTextProperty();
  time_txtprop->BoldOn();
  time_txtprop->ShadowOn();
  time_txtprop->SetColor(colors->GetColor3d("White").GetData());

  vtkLookupTable* rainbowBlueRedLut = vtkLookupTable::New();
  rainbowBlueRedLut->SetNumberOfColors(256);
  rainbowBlueRedLut->SetHueRange(0.667, 0.0);
  rainbowBlueRedLut->Build();

  vtkScalarBarActor* scalarBar = vtkScalarBarActor::New();
  scalarBar->SetLookupTable(rainbowBlueRedLut);
  scalarBar->SetLabelTextProperty(text_property);
  scalarBar->SetOrientationToHorizontal();
  scalarBar->DrawTickLabelsOn();
  scalarBar->SetTextPositionToPrecedeScalarBar();
  scalarBar->SetWidth(0.8);
  scalarBar->SetHeight(0.1);
  scalarBar->SetPosition(0.1, 0.08);

  // Geometry-specific settings
  // -------------------------------------------------------------------------
  // TGV-Coarse
  camera->SetPosition(-15.131393718362109, 11.197342387841433, -14.440502063555241);
  camera->SetFocalPoint(3.141592741012574, 3.1415927410125715, 3.1415927410125755);
  camera->SetViewUp(0.2502916268143657, 0.9520257035449052, 0.17607146655959552);
  camera->SetParallelScale(6.978615083554667);
  renderWindow->SetSize(900,520);
  scalarBar->SetTextPad(5); // Padding also controls the text size
  title->SetDisplayPosition(40, 460);
  title_txtprop->SetFontSize(24);
  quantity->SetDisplayPosition(390, 15);
  quantity_txtprop->SetFontSize(18);
  time->SetDisplayPosition(660, 100);
  time_txtprop->SetFontSize(18);

  // Transonic Cylinder
  // camera->SetPosition(35.82379571216059, -3.9871568578962098, 329.7784155802543);
  // camera->SetFocalPoint(35.82379571216059, -3.9871568578962098, 9.869604110717773);
  // camera->SetFocalDisk(1.0);
  // camera->Zoom(1.0);
  // renderWindow->SetSize(1256, 799);
  // scalarBar->SetTextPad(8);
  // title_txtprop->SetFontSize(36);
  // title->SetDisplayPosition(20, 750);
  // quantity->SetDisplayPosition(578, 10);
  // quantity_txtprop->SetFontSize(24);
  // -------------------------------------------------------------------------

  std::cout << "scalarBar position: " << scalarBar->GetPosition()[0] <<
    ", " << scalarBar->GetPosition()[1] << "\n";

  vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
  polyDataMapper->SetInputData(geometryFilter->GetOutput());
  polyDataMapper->ScalarVisibilityOn();
  polyDataMapper->SetLookupTable(rainbowBlueRedLut);
  polyDataMapper->SetScalarRange(0.1, 0.5);
  polyDataMapper->SetScalarModeToUsePointFieldData();
  polyDataMapper->ColorByArrayComponent("Q", 0);
  polyDataMapper->Update();

  std::cout << "PolyData number of cells: " <<
      polyDataMapper->GetInput()->GetNumberOfCells() << "\n";

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(polyDataMapper);

  // Disable this line to show render window
  renderWindow->ShowWindowOff();

  renderWindow->AddRenderer(renderer);
  renderer->AddActor(actor);
  renderer->AddActor(scalarBar);
  renderer->AddActor(title);
  renderer->AddActor(quantity);
  renderer->AddActor(time);
  renderer->SetActiveCamera(camera);

  // Parallel rendering section for construction of postprocessed image
  vtkCompositeRenderManager* compositeRenderManager =
    vtkCompositeRenderManager::New();
  compositeRenderManager->SetRenderWindow(renderWindow);

  // Disable this line to show onscreen render windows for non-root nodes
  // compositeRenderManager->InitializeOffScreen();

  if(myId != 0){
    // Call for parallel rendering requests from other nodes
    compositeRenderManager ->StartServices();
  }

  if (myId == 0){
    compositeRenderManager->GetRenderWindow()->Render();
    vtkWindowToImageFilter* window_to_image_filter = vtkWindowToImageFilter::New();
    window_to_image_filter->SetInput(compositeRenderManager->GetRenderWindow());
    window_to_image_filter->SetScale(1);
    window_to_image_filter->SetInputBufferTypeToRGB();
    window_to_image_filter->Update();

    std::string fileName = "filename_";
    fileName += std::to_string(dataDescription->GetTimeStep());
    fileName += ext;

    imgwriter->SetFileName(fileName.c_str());
    imgwriter->SetInputConnection(window_to_image_filter->GetOutputPort());
    imgwriter->Write();

    compositeRenderManager->GetRenderWindow()->Finalize();
    window_to_image_filter->Delete();

    compositeRenderManager ->StopServices();
  }

  std::cout << "myId passed: " << myId << "\n";

  controller->Barrier();

  // Manual deletion of object pointers to ensure memory is freed. This includes
  // using normal pointer notation rather than using vtkSmartPointer.

  grid->Delete();
  renderer->Delete();
  renderWindow->Delete();
  camera->Delete();
  geometryFilter->Delete();
  threshold->Delete();
  text_property->Delete();
  quantity->Delete();
  rainbowBlueRedLut->Delete();
  scalarBar->Delete();
  polyDataMapper->Delete();
  actor->Delete();
  compositeRenderManager->Delete();
}
