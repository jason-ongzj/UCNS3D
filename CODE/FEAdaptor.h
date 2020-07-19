#ifndef FEADAPTOR_HEADER
#define FEADAPTOR_HEADER

#include <string>

class Attributes;
class Grid;

namespace FEAdaptor
{
  void Initialize(int outputFrequency);

  void Finalize();

  void BuildGrid(Grid& grid);

  void CoProcess(Grid& grid, Attributes& attributes, double time);
}

#endif
