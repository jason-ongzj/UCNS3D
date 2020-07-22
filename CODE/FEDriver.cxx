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
    if (attributes.GetGrid() == NULL){
      grid.Initialize(pointSet, pointSetSize, vtkCellId, vtkCellIdSetSize);
      attributes.Initialize(&grid);
      std::cout << "Rank " << *rank << ": Checking size of pointSet in Grid object: " << grid.GetNumberOfPoints() << "\n";
      std::cout << "Rank " << *rank << ": Checking number of cells in Grid object: " << grid.GetNumberOfCells() << "\n";
      // FEAdaptor::BuildGrid(grid);
    }
  }

  extern "C" void densityfunction_(double* scalars)
  {
    attributes.UpdateFields(scalars, DENSITY);
    double* rho = attributes.GetRhoArray();
    // int count = 0;
    std::cout << "Density: " << *(rho + 1) << "\n";

    // Release rho pointer memory
    rho = NULL;
    delete rho;
  }

  extern "C" void scalarsfunction_(double* scalars, int* scalarDimension)
  {
    double* velocity_x = attributes.GetUArray();
    double* velocity_y = attributes.GetVArray();
    double* velocity_z = attributes.GetVArray();
    switch(*scalarDimension){
      case UX:
        attributes.UpdateFields(scalars, VELOCITY_X);
        if (velocity_x){
          std::cout << "U: " << *(velocity_x) << "\n";
        }
        break;
      case UY:
        attributes.UpdateFields(scalars, VELOCITY_Y);
        if (velocity_y){
          std::cout << "V: " << *(velocity_y) << "\n";
        }
        break;
      case UZ:
        attributes.UpdateFields(scalars, VELOCITY_Z);
        if (velocity_z){
          std::cout << "W: " << *(velocity_z + 1) << "\n";
        }
        break;
    }
    
    // Release memory for velocity pointers
    velocity_x = NULL;
    velocity_y = NULL;
    velocity_z = NULL;

    delete velocity_x;
    delete velocity_y;
    delete velocity_z;
  }

  extern "C" void coprocessor_initialize_(int* outputFrequency)
  {
    FEAdaptor::Initialize(*outputFrequency);
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
