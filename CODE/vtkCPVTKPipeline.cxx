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

  std::cout << "Current timestep: " << dataDescription->GetTimeStep() << endl;

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

  camera->SetPosition(35.82379571216059, -3.9871568578962098, 329.7784155802543);
  camera->SetFocalPoint(35.82379571216059, -3.9871568578962098, 9.869604110717773);
  // camera->SetViewUp(0.21276442893019504, 0.8819412362199431, -0.4206078382969658);
  camera->SetFocalDisk(1.0);
  camera->Zoom(1.0);

  renderWindow->SetSize(1256, 799);
  renderWindow->SetStereoTypeToCrystalEyes();

  std::string ext = ".png";
  auto imgwriter = vtkSmartPointer<vtkImageWriter>::New();
  if (ext == ".jpg") {
    imgwriter = vtkSmartPointer<vtkJPEGWriter>::New();
  } else {
    imgwriter = vtkSmartPointer<vtkPNGWriter>::New();
  }

  vtkSmartPointer<vtkCellDataToPointData> cellToPoint = vtkSmartPointer<vtkCellDataToPointData>::New();
  cellToPoint->SetInputData(grid);
  cellToPoint->Update();

  std::string fieldName = "U";

  vtkSmartPointer<vtkPointData> pointData = cellToPoint->GetOutput()->GetPointData();
  vtkSmartPointer<vtkDoubleArray> scalarRU = vtkArrayDownCast<vtkDoubleArray> (pointData->GetAbstractArray(fieldName.c_str()));

  double* scalarRU_ptr = scalarRU->GetPointer(0);

  std::cout << "scalarRU output: " << *scalarRU_ptr << "\n";
  std::cout << "UnstructuredGrid number of cells: " << grid->GetNumberOfCells() << "\n";
  vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();

  geometryFilter->SetInputData(grid);
  geometryFilter->Update();

  vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  polyDataMapper->SetInputData(geometryFilter->GetOutput());
  polyDataMapper->Update();

  std::cout << "PolyData number of cells: " << polyDataMapper->GetInput()->GetNumberOfCells() << "\n";

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(polyDataMapper);

  auto colors = vtkSmartPointer<vtkNamedColors>::New();
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());
  actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  renderer->AddActor(actor);
  renderer->SetActiveCamera(camera);
  renderWindow->AddRenderer(renderer);

  renderWindow->Render();

  vtkSmartPointer<vtkWindowToImageFilter> window_to_image_filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  window_to_image_filter->SetInput(renderWindow);
  window_to_image_filter->SetScale(1);
  window_to_image_filter->SetInputBufferTypeToRGB();
  window_to_image_filter->Update();
  //
  std::string fileName = "filename_";
  fileName += std::to_string(dataDescription->GetTimeStep());
  fileName += ext;

  imgwriter->SetFileName(fileName.c_str());
  imgwriter->SetInputConnection(window_to_image_filter->GetOutputPort());
  imgwriter->Write();

  grid->Delete();
}
