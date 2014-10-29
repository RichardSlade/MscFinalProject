#ifndef INCL_MEMORYMAP_HPP
#define INCL_MEMORYMAP_HPP

#include <vector>

struct MapCell
{
    int             ticksVisited;
    int             id;

                    MapCell(int i)
                    : ticksVisited(0)
                    , id(i)
                    {};

};

class MemoryMap
{
    private:
        std::vector<std::vector<MapCell>>   mMapCells;
        int                                 mCellSize;
        int                                 mCellVisitCount;
        int                                 mWorldX;
        int                                 mWorldY;

    public:
                                MemoryMap(){};

                                MemoryMap(int worldX, int worldY, int cellSize)
                                : mCellSize(cellSize)
                                , mCellVisitCount(0)
                                , mWorldX(worldX)
                                , mWorldY(worldY)
                                {
                                    int idCount = 0;

                                    //create the 2d vector of blank segments
                                    for(int y = 0; y < mWorldY; y++)
                                    {
                                        std::vector<MapCell> cellRow;

                                        for(int x = 0; x < mWorldX; x++)
                                        {
                                          cellRow.push_back(MapCell(idCount++));
                                        }

                                        mMapCells.push_back(cellRow);
                                    }
                                }

        int                     getMapReading(int x, int y){return mMapCells[y][x].ticksVisited;}
        int                     getCellId(int x, int y){return mMapCells[y][x].id;}
        int                     cellsVisited(){return mCellVisitCount;}
        void                    setCell(int x, int y, int value){mMapCells[y][x].ticksVisited = value;}
        bool                    visitCell(int x, int y);
        void                    reset();

};

#endif // INCL_MEMORYMAP__HPP
