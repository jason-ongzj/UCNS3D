// Adaptor for getting Fortran simulation code into ParaView CoProcessor.

// CoProcessor specific headers
#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkCPPythonScriptPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCellData.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellDataToPointData.h"

// Fortran specific header
#include "vtkCPPythonAdaptorAPI.h"

// Creates the data container for the CoProcessor.
extern "C" void creategrid_(double pointSet[][3], int* pointSetSize,
    int vtkCellId[][8], int* vtkCellIdSetSize, int* rank, int* numprocs)
{
  if (!vtkCPPythonAdaptorAPI::GetCoProcessorData())
  {
    vtkGenericWarningMacro("Unable to access CoProcessorData.");
    return;
  }

  vtkSmartPointer<vtkIdList> cellIdList = vtkSmartPointer<vtkIdList>::New();
  cellIdList->SetNumberOfIds(8);

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetNumberOfPoints(*pointSetSize);

  vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
    vtkSmartPointer<vtkUnstructuredGrid>::New();

  vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();

  static vtkIdType ids[8];
  double point[3];

  std::cout << "Process " << *rank << " with " <<  *numprocs << " processes involved."<< "\n";
  std::cout << "Checking size of pointSet: " << *pointSetSize << "\n";
  std::cout << "Checking size of vtkCellId: " << *vtkCellIdSetSize << "\n\n";

  // Serial process of registering point set
  for(int i=0; i < *pointSetSize; i++){
    points->SetPoint(i, pointSet[i][0], pointSet[i][1], pointSet[i][2]);
  }

  unstructuredGrid->Allocate(*vtkCellIdSetSize);
  for(int k = 0; k < *vtkCellIdSetSize; k++){
    for(int i=0; i < 8; i++){
      ids[i] = vtkCellId[k][i];
    }
    unstructuredGrid->InsertNextCell(12, 8, ids);
  }

  std::cout << "Total number of cells : " << unstructuredGrid->GetNumberOfCells() << "\n";

  unstructuredGrid->SetPoints(points);

  vtkCPPythonAdaptorAPI::GetCoProcessorData()->GetInputDescriptionByName("input")->SetGrid(unstructuredGrid);
}

// Add field(s) to the data container.
// Separate from above because this will be dynamic, grid is static.
extern "C" void addfield_(double* scalars, char* name)
{
  vtkCPInputDataDescription* idd =
    vtkCPPythonAdaptorAPI::GetCoProcessorData()->GetInputDescriptionByName("input");

  vtkUnstructuredGrid* unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(idd->GetGrid());

  if (!unstructuredGrid){
    vtkGenericWarningMacro("No adaptor grid to attach field data to.");
    return;
  }

  // Field name must match that in the fortran code.
  if (idd->IsFieldNeeded(name, vtkDataObject::CELL)){
    vtkSmartPointer<vtkDoubleArray> field = vtkSmartPointer<vtkDoubleArray>::New();
    field->SetNumberOfComponents(1);
    field->SetName(name);
    field->SetArray(scalars, unstructuredGrid->GetNumberOfCells(), 1);
    unstructuredGrid->GetCellData()->AddArray(field);
  }

  // Convert from cell to point data to allow contour visualization
  vtkSmartPointer<vtkCellDataToPointData> cellToPoint = vtkSmartPointer<vtkCellDataToPointData>::New();
  cellToPoint->SetInputData(unstructuredGrid);
  cellToPoint->Update();

  vtkPointData* pointData = cellToPoint->GetOutput()->GetPointData();

  vtkSmartPointer<vtkDoubleArray> doubleArray =
    vtkArrayDownCast<vtkDoubleArray>(cellToPoint->GetOutput()->GetPointData()->GetAbstractArray(name));

  if (idd->IsFieldNeeded(name, vtkDataObject::POINT)){
    unstructuredGrid->GetPointData()->AddArray(doubleArray);
  }
}
