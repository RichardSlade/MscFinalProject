#include <MemoryMap.hpp>

//------------------------------------------------------------------------
//
//	Name: MemoryMap.cpp
//
//  Author: Mat Buckland 2002
//
//------------------------------------------------------------------------

void MemoryMap::reset()
{
    mCellVisitCount = 0;

    for(int i = 0; i < mWorldY; i++)
    {
        for(int j = 0; j < mWorldX; j++)
        {
            setCell(i, j, 0);
        }
    }
}

bool MemoryMap::visitCell(int x, int y)
{
    if(mMapCells[y][x].ticksVisited == 0)
    {
        mCellVisitCount ++;
        mMapCells[y][x].ticksVisited ++;
        return true;
    }
    else
    {
//        mCellVisitCount ++;
        mMapCells[y][x].ticksVisited ++;
        return false;
    }

}
