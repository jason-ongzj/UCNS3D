// Adaptor for getting Fortran simulation code into ParaView CoProcessor.

// CoProcessor specific headers
#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCellData.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellDataToPointData.h"
#include "FEDataStructures.h"
#include "FEAdaptor.h"

// These will be called from the Fortran "glue" code"
// Completely dependent on data layout, structured vs. unstructured, etc.
// since VTK/ParaView uses different internal layouts for each.
namespace
{
  Grid grid;
  Attributes attributes;

  const int UX = 1;
  const int UY = 2;
  const int UZ = 3;

  // For conversion of C++ to Fortran functions, function names have to be
  // non-captialized since Fortran subroutines are case-insensitive.
  extern "C" void gridfunction_(double pointSet[][3], int* pointSetSize, int vtkCellId[][8], int* vtkCellIdSetSize, int* rank)
  {
    grid.Initialize(pointSet, pointSetSize, vtkCellId, vtkCellIdSetSize);
    attributes.Initialize(&grid);
    std::cout << "Rank " << *rank << ": Checking size of pointSet in Grid object: " << grid.GetNumberOfPoints() << "\n";
    std::cout << "Rank " << *rank << ": Checking number of cells in Grid object: " << grid.GetNumberOfCells() << "\n";
    // FEAdaptor::BuildGrid(grid);
  }

  extern "C" void densityfunction_(double* scalars)
  {
    attributes.UpdateFields(scalars, DENSITY);
    double* rho = attributes.GetRhoArray();
    int count = 0;
    std::cout << "Density: " << *(rho + 1) << "\n";
  }

  extern "C" void scalarsfunction_(double* scalars)
  {
    attributes.UpdateFields(scalars, VELOCITY_X);
    double* velocity_x = attributes.GetUArray();
    if (velocity_x){
      std::cout << "U: " << *(velocity_x + 1) << "\n";
    }
  }

  extern "C" void coprocessor_initialize_(int outputFrequency)
  {
    FEAdaptor::Initialize(outputFrequency);
  }

  extern "C" void coproc_grid_(double time)
  {
    FEAdaptor::CoProcess(grid, attributes, time);
  }

  extern "C" void coprocessor_finalize_()
  {
    FEAdaptor::Finalize();
  }
}
