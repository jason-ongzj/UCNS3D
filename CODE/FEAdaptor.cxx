// #include "vtkCPVTKPipeline.h"
#include <vtkCPProcessor.h>
#include <vtkUnstructuredGrid.h>
#include "FEDataStructures.h"
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPDataDescription.h>

#include <vtkPointData.h>
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
    // vtkAOSDataArrayTemplate<ValueTypeT>::SetArray(ValueType* array, vtkIdType size, int save)
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
      vtkIdType tmp[8] = {cellPoints[0], cellPoints[1], cellPoints[2], cellPoints[3], cellPoints[4],
        cellPoints[5], cellPoints[6], cellPoints[7]};
      vtkGrid->InsertNextCell(VTK_HEXAHEDRON, 8, tmp);
    }

    std::cout << "Total number of cells formed in BuildVTKGrid : " << vtkGrid->GetNumberOfCells() << "\n";
  }

  void UpdateVTKAttributes(Grid& grid, vtkCPInputDataDescription* idd)
  {

  }

  void BuildVTKDataStructures(Grid& grid, vtkCPInputDataDescription* idd){
    if (vtkGrid == NULL)
    {
      vtkGrid = vtkUnstructuredGrid::New();
      BuildVTKGrid(grid);
    }
  }
}

namespace FEAdaptor
{
  void Initialize(int outputFrequency)
  {
    if (Processor == NULL){
      Processor = vtkSmartPointer<vtkCPProcessor>::New();
      Processor->Initialize();
    }
    // vtkNew<vtkCPVTKPipeline> pipeline;

    // TODO: Initialize pipeline

    // Processor->AddPipeline(pipeline.GetPointer());
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

  void CoProcess(Grid& grid, double time, unsigned int timeStep)
  {
    vtkNew<vtkCPDataDescription> dataDescription;
    dataDescription->AddInput("input");
    dataDescription->SetTimeData(time, timeStep);
    if (Processor->RequestDataDescription(dataDescription.GetPointer()))
    {
      vtkCPInputDataDescription* idd = dataDescription->GetInputDescriptionByName("input");
      idd->SetGrid(vtkGrid);
      Processor->CoProcess(dataDescription.GetPointer());
    }
  }

}
