#include "grid2d.h"

GridCell::GridCell()
    : r(0), g(0), b(0), alive(false)  {}

GridCell GridCell::operator=(const GridCell &oth)
{
//    GridCell newCell;
//    newCell.r = oth.r;
//    newCell.g = oth.g;
//    newCell.b = oth.b;
//    return newCell;
    return GridCell(oth);
}

Grid2D::Grid2D(int n, int m)
{
    H = n; W = m;
    _cells = vvecGC(H,vecGC(W,GridCell()));
}

GridCell& Grid2D::operator()(int i, int j)
{

    return _cells[(i+H) % H][(j+W) % W];
}

//Grid2D Grid2D::operator=(const Grid2D &oth)
//{
//    auto newGrid = Grid2D(oth.H,oth.W);
//    newGrid._cells = oth._cells;
//    return newGrid;
//}
