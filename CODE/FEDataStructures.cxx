#include "FEDataStructures.h"

#include <iostream>
#include <iterator>

Grid::Grid()
{
}

void Grid::Initialize(double pointSet[][3], int* pointSetSize, int vtkCellId[][8],
  int* vtkCellIdSetSize)
{
  double coord[3];
  for(int i = 0; i < *pointSetSize; i++)
  {
    coord[0] = pointSet[i][0];
    coord[1] = pointSet[i][1];
    coord[2] = pointSet[i][2];

    std::copy(coord, coord+3, std::back_inserter(this->Points));
  }

  unsigned int cellPoints[8];
  for(int i = 0; i < *vtkCellIdSetSize; i++)
  {
    cellPoints[0] = vtkCellId[i][0];
    cellPoints[1] = vtkCellId[i][1];
    cellPoints[2] = vtkCellId[i][2];
    cellPoints[3] = vtkCellId[i][3];
    cellPoints[4] = vtkCellId[i][4];
    cellPoints[5] = vtkCellId[i][5];
    cellPoints[6] = vtkCellId[i][6];
    cellPoints[7] = vtkCellId[i][7];

    std::copy(cellPoints, cellPoints + 8, std::back_inserter(this->Cells));
  }

  numberOfPoints = *pointSetSize;
  numberOfCells = *vtkCellIdSetSize;
}

int Grid::GetNumberOfPoints()
{
  return numberOfPoints;
}

int Grid::GetNumberOfCells()
{
  return numberOfCells;
}

double* Grid::GetPointsArray()
{
  if (this->Points.empty())
  {
    return NULL;
  }
  return &(this->Points[0]);
}

double* Grid::GetPoint(size_t pointId)
{
  if (pointId >= this->Points.size())
  {
    return NULL;
  }
  return &(this->Points[pointId * 3]);
}

unsigned int* Grid::GetCellPoints(size_t cellId)
{
  if (cellId >= this->Cells.size())
  {
    return NULL;
  }
  return &(this->Cells[cellId * 8]);
}

Attributes::Attributes()
{
  this->GridPtr = NULL;
}

void Attributes::Initialize(Grid* grid)
{
  this->GridPtr = grid;
}

void Attributes::UpdateFields(double* scalars, Variable var)
{
  // Clear vector arrays before updating to ensure no memory leak
  switch (var) {
    case DENSITY:
      {
        Rho.clear();
        for (int i = 0; i < GridPtr->GetNumberOfCells(); i++)
        {
          Rho.insert(Rho.begin()+i, *(scalars + i));
        }
        break;
      }

    case VELOCITY_X:
      {
        try {
          if (Rho.empty()) {
            throw 'a';
          } else {
            U.clear();
            for (int i = 0; i < GridPtr->GetNumberOfCells(); i++){
              // double rho = Rho.at(i);
              U.insert(U.begin()+i, *(scalars + i));
            }
          }
        } catch (...) {
          std::cout << "Density array not found!" << "\n";
        }
        break;
      }

    case VELOCITY_Y:
      {
        try {
          if (Rho.empty()) {
            throw 'a';
          } else {
            V.clear();
            for (int i = 0; i < GridPtr->GetNumberOfCells(); i++){
              double rho = Rho.at(i);
              V.insert(V.begin()+i, *(scalars + i)/rho);
            }
          }
        } catch (...) {
          std::cout << "Density array not found!" << "\n";
        }
        break;
      }

    case VELOCITY_Z:
      {
        try {
          if (Rho.empty()) {
            throw 'a';
          } else {
            W.clear();
            for (int i = 0; i < GridPtr->GetNumberOfCells(); i++){
              double rho = Rho.at(i);
              W.insert(W.begin()+i, *(scalars + i)/rho);
            }
          }
        } catch (...) {
          std::cout << "Density array not found!" << "\n";
        }
        break;
      }

    case Q_CRITERION:
      Q.clear();
      for (int i = 0; i < GridPtr->GetNumberOfCells(); i++)
      {
        Q.insert(Q.begin()+i, *(scalars + i));
      }
      break;
  }
}

Grid* Attributes::GetGrid()
{
  return this->GridPtr;
}

double* Attributes::GetRhoArray()
{
  return &(this->Rho[0]);
}

double* Attributes::GetUArray()
{
  return &(this->U[0]);
}

double* Attributes::GetVArray()
{
  return &(this->V[0]);
}

double* Attributes::GetWArray()
{
  return &(this->W[0]);
}

double* Attributes::GetQCriterionArray()
{
  return &(this->Q[0]);
}
