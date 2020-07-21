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
#include <vtkScalarsToColors.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkWindowToImageFilter.h>

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
  // std::cout << "RequestDataDescription: " << dataDescription->GetTimeStep() % this->OutputFrequency << "\n";

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
    vtkWarningMacro("DataDescription is missing input unstructured grid.");
    return 0;
  }

  if (this->RequestDataDescription(dataDescription) == 0)
  {
    return 1;
  }

  vtkSmartPointer<vtkRenderView> renderView = vtkSmartPointer<vtkRenderView>::New();
  renderView->SetDisplaySize(900, 522);
  int count = dataDescription->GetTimeStep();
  std::cout << "Current timestep: " << count << endl;
  vtkSmartPointer<vtkRenderer> renderer = renderView->GetRenderer();
  vtkSmartPointer<vtkRenderWindow> renderWindow = renderView->GetRenderWindow();
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

  camera->SetPosition(-6.962290992619889, 13.00713402789492, 18.716862694358497);
  camera->SetFocalPoint(3.141592741012574, 3.1415927410125715, 3.1415927410125755);
  camera->SetViewUp(0.21276442893019504, 0.8819412362199431, -0.4206078382969658);
  camera->SetFocalDisk(1.0);
  camera->Zoom(5.44139824412335);
  renderWindow->SetSize(900, 522);

  renderer->SetActiveCamera(camera);

  renderWindow->SetStereoTypeToCrystalEyes();

  std::string ext = ".png";
  auto imgwriter = vtkSmartPointer<vtkImageWriter>::New();
  if (ext == ".jpg") {
    imgwriter = vtkSmartPointer<vtkJPEGWriter>::New();
  } else {
    imgwriter = vtkSmartPointer<vtkPNGWriter>::New();
  }

  vtkSmartPointer<vtkPVTrivialProducer> producer =
    vtkSmartPointer<vtkPVTrivialProducer>::New();
  producer->SetOutput(grid);

  vtkSmartPointer<vtkCellDataToPointData> cellToPoint = vtkSmartPointer<vtkCellDataToPointData>::New();
  cellToPoint->SetInputData(grid);
  cellToPoint->Update();

  std::string fieldName = "U";

  vtkSmartPointer<vtkPointData> pointData = cellToPoint->GetOutput()->GetPointData();
  vtkSmartPointer<vtkAbstractArray> scalarRU = pointData->GetAbstractArray(fieldName.c_str());

  vtkSmartPointer<vtkScalarsToColors> scalarToColor = vtkSmartPointer<vtkScalarsToColors>::New();
  // vtkUnsignedCharArray* rULUT = scalarToColor->MapScalars(scalarRU, VTK_COLOR_MODE_DIRECT_SCALARS, 0, VTK_RGB);

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(4);
  lut->SetTableValue(0,1.0,0.0,0.0,1.0);
  lut->SetTableValue(1,0.0,1.0,0.0,1.0);
  lut->SetTableValue(2,0.0,0.0,1.0,1.0);
  lut->SetTableValue(3,1.0,1.0,0.0,1.0);

  vtkSmartPointer<vtkScalarBarActor> rULUTColorBar = vtkSmartPointer<vtkScalarBarActor>::New();
  rULUTColorBar->SetLookupTable(lut);
  rULUTColorBar->SetTitle(fieldName.c_str());

  // Set contour filter
  vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  contourFilter->SetInputConnection(producer->GetOutputPort());
  contourFilter->GenerateValues(5, -0.25, 0.25);

  vtkSmartPointer<vtkPolyDataMapper> contMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  contMapper->SetInputConnection(contourFilter->GetOutputPort());
  contMapper->SetScalarRange(-0.25, 0.25);

  vtkSmartPointer<vtkActor> contActor = vtkSmartPointer<vtkActor>::New();
  contActor->SetMapper(contMapper);

  renderer->AddActor(contActor);
  renderer->AddActor(rULUTColorBar);

  renderWindow->SetFullScreen(false);
  auto window_to_image_filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  window_to_image_filter->SetInput(renderWindow);
  window_to_image_filter->SetScale(1);
  window_to_image_filter->SetInputBufferTypeToRGB();

  std::string fileName = "filename_";
  fileName += std::to_string(count);
  fileName += ext;

  imgwriter->SetFileName(fileName.c_str());
  imgwriter->SetInputConnection(window_to_image_filter->GetOutputPort());
  imgwriter->Write();

  grid->Delete();
}
