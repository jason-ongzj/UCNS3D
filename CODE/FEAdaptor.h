#ifndef FEADAPTOR_HEADER
#define FEADAPTOR_HEADER

#include <string>
#include <vtkCPInputDataDescription.h>

class Attributes;
class Grid;

namespace Local
{
  void BuildVTKGrid(Grid& grid);

  void UpdateVTKAttributes(Grid& grid, vtkCPInputDataDescription* idd);

  void BuildVTKDataStructure(Grid& grid, vtkCPInputDataDescription* idd);
}

namespace FEAdaptor
{
  void Initialize(int outputFrequency);

  void Finalize();

  void BuildGrid(Grid& grid);

  void CoProcess();
}

#endif
