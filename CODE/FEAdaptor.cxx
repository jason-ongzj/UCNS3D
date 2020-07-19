#include "FEDataStructures.h"
#include "FEAdaptor.h"

#include <vtkCPProcessor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPDataDescription.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include "vtkCPVTKPipeline.h"
#include <iostream>

namespace
{
  vtkCPProcessor* Processor = NULL;
  vtkUnstructuredGrid* vtkGrid;

  void BuildVTKGrid(Grid& grid)
  {
    // Create points information
    vtkNew<vtkDoubleArray> pointArray;
    pointArray->SetNumberOfComponents(3);

    // From Paraview VTK documentation:
    // vtkAOSDataArrayTemplate<ValueTypeT>::SetArray(ValueType* array,
    //    vtkIdType size, int save)
    // Note: Set save to 1 to prevent points array from being destroyed from
    //       memory.
    pointArray->SetArray(grid.GetPointsArray(),
      static_cast<vtkIdType>(grid.GetNumberOfPoints() * 3), 1);
    vtkNew<vtkPoints> points;
    points->SetData(pointArray.GetPointer());
    vtkGrid->SetPoints(points.GetPointer());

    // Create cell in vtkUnstructuredGrid
    int numCells = grid.GetNumberOfCells();
    vtkGrid->Allocate(static_cast<vtkIdType>(numCells * 8));
    for(int cell = 0; cell < numCells; cell++)
    {
      unsigned int* cellPoints = grid.GetCellPoints(cell);
      vtkIdType tmp[8] = {cellPoints[0], cellPoints[1], cellPoints[2],
        cellPoints[3], cellPoints[4], cellPoints[5], cellPoints[6],
        cellPoints[7]};
      vtkGrid->InsertNextCell(VTK_HEXAHEDRON, 8, tmp);
    }

    std::cout << "Total number of cells formed in BuildVTKGrid : "
      << vtkGrid->GetNumberOfCells() << "\n";
  }

  void UpdateVTKAttributes(Grid& grid, Attributes& attributes, vtkCPInputDataDescription* idd)
  {
    if (idd->IsFieldNeeded("U", vtkDataObject::CELL))
    {
      if (vtkGrid->GetCellData()->GetNumberOfArrays() == 0)
      {
        // velocity X array
        vtkNew<vtkDoubleArray> velocity_x;
        velocity_x->SetName("U");
        velocity_x->SetNumberOfComponents(1);
        vtkGrid->GetCellData()->AddArray(velocity_x.GetPointer());
      }
      vtkDoubleArray* velocity_x =
        vtkDoubleArray::SafeDownCast(vtkGrid->GetCellData()->GetArray("U"));
      double* uxData = attributes.GetUArray();
      velocity_x->SetArray(uxData, vtkGrid->GetNumberOfCells(), 1);
    }

    // Convert from cell to point data to allow contour visualization
    vtkSmartPointer<vtkCellDataToPointData> cellToPoint =
      vtkSmartPointer<vtkCellDataToPointData>::New();
    cellToPoint->SetInputData(vtkGrid);
    cellToPoint->Update();

    vtkPointData* pointData = cellToPoint->GetOutput()->GetPointData();

    vtkSmartPointer<vtkDoubleArray> velocity_x_cellToPoint =
      vtkArrayDownCast<vtkDoubleArray>(cellToPoint->GetOutput()->GetPointData()->GetAbstractArray("U"));

    if (idd->IsFieldNeeded("U", vtkDataObject::POINT)){
      vtkGrid->GetPointData()->AddArray(velocity_x_cellToPoint);
    }
  }

  void GetGridInfo(vtkCPInputDataDescription* idd){
    vtkUnstructuredGrid* iddGrid = vtkUnstructuredGrid::SafeDownCast(idd->GetGrid());
    // std::cout << "Total number of cells formed in IDDVTKGrid : "
    //   << iddGrid->GetNumberOfCells() << "\n";
  }

  void BuildVTKDataStructures(Grid& grid, Attributes& attributes,
    vtkCPInputDataDescription* idd)
  {
    if (vtkGrid == NULL)
    {
      std::cout << "vtkGrid not detected." << "\n";
      vtkGrid = vtkUnstructuredGrid::New();
      BuildVTKGrid(grid);
    }
    // GetGridInfo(idd);
    // UpdateVTKAttributes(grid, attributes, idd);
  }
}

namespace FEAdaptor
{
  unsigned int timeStep = 0;

  void Initialize(int outputFrequency)
  {
    if (Processor == NULL){
      Processor = vtkCPProcessor::New();
      Processor->Initialize();

      std::cout << "Coprocessor initialized." << "\n";
    }
    vtkNew<vtkCPVTKPipeline> pipeline;

    // TODO: Initialize pipeline
    pipeline->Initialize(outputFrequency);
    Processor->AddPipeline(pipeline.GetPointer());
  }

  void BuildGrid(Grid& grid)
  {
    vtkGrid = vtkUnstructuredGrid::New();
    BuildVTKGrid(grid);
  }

  void Finalize()
  {
    if (Processor){
      Processor->Delete();
      Processor = NULL;
    }
    if (vtkGrid){
      vtkGrid->Delete();
      vtkGrid = NULL;
    }
  }

  void CoProcess(Grid& grid, Attributes& attributes, double time)
  {
    vtkCPDataDescription* dataDescription = vtkCPDataDescription::New();
    // int step = *timeStep;
    std::cout << "Coprocessing..." << "\n";
    // if (dataDescription == NULL){
    //   dataDescription = vtkCPDataDescription::New();
    // }
    dataDescription->AddInput("input");
    dataDescription->SetTimeData(time, timeStep);
    std::cout << "Time: " << time << ", timeStep: " << timeStep << "\n";
    if (Processor == NULL)
    {
      std::cout << "Processor is NULL.\n";
    }

    std::cout << "Data description: " << Processor->RequestDataDescription(dataDescription) << "\n";
    if (Processor->RequestDataDescription(dataDescription) != 0)
    {
      vtkCPInputDataDescription* idd = dataDescription->GetInputDescriptionByName("input");
      BuildVTKDataStructures(grid, attributes, idd);
      idd->SetGrid(vtkGrid);
      Processor->CoProcess(dataDescription);
      std::cout << "Data description found." << "\n";
    } else {
      std::cout << "Data description not found. Try again." << "\n";
    }

    timeStep++;

    dataDescription->Delete();
  }
}
