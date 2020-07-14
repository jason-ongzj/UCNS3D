#ifndef FEDATASTRUCTURES_HEADER
#define FEDATASTRUCTURES_HEADER

#include <cstddef>
#include <vector>

class Grid
{
  public:
    Grid();
    void Initialize(double pointSet[][3], int* pointSetSize, int vtkCellId[][8],
      int* vtkCellIdSetSize);
    int GetNumberOfPoints();
    int GetNumberOfCells();
    double* GetPointsArray();
    double* GetPoint(size_t pointId);
    unsigned int* GetCellPoints(size_t cellId);

  private:
    // vtkPoints* points;
    std::vector<double> Points;
    std::vector<unsigned int> Cells;
    int numberOfPoints;
    int numberOfCells;
};

enum Variable {DENSITY, VELOCITY_X, VELOCITY_Y, VELOCITY_Z, ENERGY};

class Attributes
{
  public:
    Attributes();
    void Initialize(Grid* grid);

    void UpdateFields(double* scalars, Variable var);
    double* GetRhoArray();
    double* GetUArray();
    double* GetVArray();
    double* GetWArray();
    double* GetEnergyArray();

  private:
    Grid* GridPtr;
    std::vector<double> Rho;
    std::vector<double> U;
    std::vector<double> V;
    std::vector<double> W;
    std::vector<double> E; // Energy
};
#endif
