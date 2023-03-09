#ifndef GRID2D_H
#define GRID2D_H

#include <QVector>

class GridCell
{
public:
    GridCell();
    float r,g,b;
    bool alive;

    GridCell operator = (const GridCell &oth);
};

using vecGC = QVector<GridCell>;
using vvecGC = QVector<vecGC>;

class Grid2D
{
public:
    Grid2D(int n=10, int m=10);

    int H,W;
    vvecGC _cells;

    GridCell& operator()(int i, int j);

//    Grid2D operator = (const Grid2D &oth);
};

#endif // GRID2D_H
