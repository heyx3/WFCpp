#pragma once

#include <span>
#include "Grid.h"

namespace WFC::Tiled3D
{
    //Provides a flexible strategy to generate a tile Grid with WFC.
    class WFC_API StandardRunner
    {
    public:

        //In this model, cells have a "temperature", reflecting
        //    how often they have been unsolved.
        //This temperature affects their clear radius the next time they become unsolved,
        //    as well as their priority for being the next tile set.

        //Cells remember their history of being unsolvable and subsequently cleared.
        struct WFC_API CellHistory
        {
            //A measure of how frequently the cell has been unsolvable.
            float BaseTemperature = 0.0f;
            //A "timestamp" of how recently the cell has been unsolvable.
            //This has an implicit cooling effect on the temperature.
            //The special value ~0 (all bits set) means it's never been unsolvable.
            uint32_t LastUnsolvedTime = 0;
        };
        static const uint32_t NEVER_UNSOLVED_TIMESTAMP = -1;


        //TODO: Switch to a Set, since most cells won't need their history tracked.
        Array3D<CellHistory> History;

        uint32_t CurrentTimestamp = 0; //Incremented each time the simulation iterates.

        //The increase in temperature around an unsolved cell.
        //You probably want the increase to be highest in the center.
        //The outermost index is X, the innermost is Z.
        std::array<std::array<std::array<float, 3>, 3>, 3> TempIncreases = {{
            {{
                { 0.125f, 0.25f, 0.125f },
                { 0.25f, 0.5f, 0.25f },
                { 0.125f, 0.25f, 0.125f }
            }},
            {{
                { 0.25f, 0.5f, 0.25f },
                { 0.5f, 1.0f, 0.5f },
                { 0.25f, 0.5f, 0.25f }
            }},
            {{
                { 0.125f, 0.25f, 0.125f },
                { 0.25f, 0.5f, 0.25f },
                { 0.125f, 0.25f, 0.125f }
            }}
        }};

        //The decrease in cell temperature per-tick.
        float CoolOffRate = 0.1f;
        //A jump down in temperature when a cell gets set.
        float CoolOffFromSetting = 0.0f; //Default value of 0 because I'm not actually sure
                                         //    it creates a better outcome.

        //The effect of temperature on a cell's priority for getting set.
        //A value of 1 means "no effect",
        //    and larger values make the temperature more important.
        float TemperaturePriority = 1.0f;
        
        //The influence of temperature on how soon a cell should be set.
        //Note that temperature is usually the size of small integers.
        float PriorityWeightTemperature = 0.2f;
        //The influence on a cell's 'NPermutations' value on how soon it should be set.
        //Note that the 'NPermutations' value is fed in as a float from 0 to 1.
        float PriorityWeightEntropy = 0.8f;

        PRNG Rand;


        Grid Grid;


        //Calculates the temperature of a cell.
        float GetTemperature(const Vector3i& cell) const;
        //Calculates the area to clear around a given (presumably unsolvable) cell.
        Region3i GetClearRegion(const Vector3i& cell) const;
        //Calculates the priority of handling a given cell.
        float GetPriority(const Vector3i& cellPos);

        //Runs one iteration of the algorithm.
        //Returns whether the algorithm is finished.
        bool Tick();
        //Runs the algorithm until finished, or for N ticks.
        //Returns whether the algorithm is finished.
        bool TickN(int n);


        StandardRunner(const List<Tile>& inputTiles, const Vector3i& gridSize,
                       PRNG rand = PRNG(std::random_device()()))
            : Grid(inputTiles, gridSize), Rand(rand),
              History(gridSize, { })
        {

        }


    private:
        Grid::Report report;
        Set<Vector3i> nextCells, unsolvableCells;
        Dictionary<Vector3i, float> buffer_pickCell_priorities;
        List<int> buffer_randomTile_distribution;

        void ClearAround(const Vector3i& centerCellPos);
        void Set(const Vector3i& cellPos, TileIdx tile, Transform3D permutation);

        Vector3i PickNextCellToSet();

        std::tuple<TileIdx, Transform3D> RandomTile(const std::span<TransformSet>& allowedPerTile);
    };
}