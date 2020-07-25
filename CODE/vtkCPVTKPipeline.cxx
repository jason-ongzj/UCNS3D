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
#include <vtkPVTrivialProducer.h>
#include <vtkCellDataToPointData.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkScalarsToColors.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>
#include <vtkWindowToImageFilter.h>

#include <vtkMPIController.h>
#include <vtkMultiProcessController.h>
#include <vtkCompositeRenderManager.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataNormals.h>

#include <array>

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

  renderWindow->SetSize(1256, 799);
  renderWindow->SetStereoTypeToCrystalEyes();

  vtkCamera* camera = vtkCamera::New();
  camera->SetPosition(35.82379571216059, -3.9871568578962098, 329.7784155802543);
  camera->SetFocalPoint(35.82379571216059, -3.9871568578962098, 9.869604110717773);
  camera->SetFocalDisk(1.0);
  camera->Zoom(1.0);

  std::cout << "UnstructuredGrid number of cells: " <<
      grid->GetNumberOfCells() << "\n";

  // Data copy from vtkUnstructuredGrid to vtkPolyData. To be freed later so as
  // to prevent memory leak.
  vtkGeometryFilter* geometryFilter = vtkGeometryFilter::New();
  geometryFilter->SetInputData(grid);
  geometryFilter->MergingOff();
  geometryFilter->Update();

  vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
  polyDataMapper->SetInputData(geometryFilter->GetOutput());
  polyDataMapper->ScalarVisibilityOn();
  polyDataMapper->SetScalarRange(0, 0.5);
  polyDataMapper->SetScalarModeToUsePointFieldData();
  polyDataMapper->ColorByArrayComponent("U", 0);
  polyDataMapper->Update();

  std::cout << "PolyData number of cells: " <<
      polyDataMapper->GetInput()->GetNumberOfCells() << "\n";

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(polyDataMapper);

  auto colors = vtkNamedColors::New();
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());
  actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  // Disable this line to show render window
  renderWindow->ShowWindowOff();

  renderWindow->AddRenderer(renderer);
  renderer->AddActor(actor);
  renderer->SetActiveCamera(camera);

  // Parallel rendering section for construction of postprocessed image
  vtkCompositeRenderManager* compositeRenderManager =
    vtkCompositeRenderManager::New();
  compositeRenderManager->SetRenderWindow(renderWindow);

  // Disable this line to show onscreen render windows for non-root nodes
  compositeRenderManager->InitializeOffScreen();

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
  geometryFilter->Delete();
  // normals->Delete();
  polyDataMapper->Delete();
  actor->Delete();
  renderer->Delete();
  renderWindow->Delete();
  camera->Delete();
  colors->Delete();
  compositeRenderManager->Delete();
}
