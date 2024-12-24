#pragma once

#include <tuple>
#include <random>

#include "Grid.h"


namespace WFC
{
namespace Tiled3D
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
            //The special value -1 (all bits set) means it's never been unsolvable.
            uint32_t LastUnsolvedTime = static_cast<uint32_t>(-1);
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
        //Default value of 0 because I'm not actually sure it creates a better outcome.
        float CoolOffFromSetting = 0.0f;
        //From 0 to 1, how fast the clear region increases around tiles that have already been cleared a lot.
        //Use a low value for tilesets with many small errors that need limited clearing.
        float ClearRegionGrowthRateT = 0.5f;

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

        void Reset()
        {
            Grid.ClearCells(Region3i(Grid.Cells.GetDimensions()));
            History.Fill({ });
            report.Clear();
            nextCells.Clear();
            unsolvableCells.Clear();
        }
        void Reset(const Dictionary<Vector3i, std::tuple<TileIdx, Transform3D>>& constants);
        //TODO: Another overload that takes new 'constants'.


        StandardRunner(const List<Tile>& inputTiles, const Vector3i& gridSize,
                       const Dictionary<Vector3i, std::tuple<TileIdx, Transform3D>>* constants = nullptr,
                       PRNG rand = PRNG(std::random_device()()))
            : History(gridSize, { }), Rand(rand), Grid(inputTiles, gridSize)
        {
            if (constants != nullptr)
                for (const auto& constant : *constants)
                {
                    Vector3i cellPos = std::get<0>(constant);
                    TileIdx chosenTile = std::get<0>(std::get<1>(constant));
                    Transform3D chosenPermutation = std::get<1>(std::get<1>(constant));
                    Set(cellPos, chosenTile, chosenPermutation);
                }
        }


    private:
        Grid::Report report;
        Set<Vector3i> nextCells, unsolvableCells;
        List<std::tuple<Vector3i, float>> buffer_pickCell_options;
        List<float> buffer_randomTile_weights;

        void ClearAround(const Vector3i& centerCellPos);
        void Set(const Vector3i& cellPos, TileIdx tile, Transform3D permutation,
                 bool makeImmutable = false);

        Vector3i PickNextCellToSet();

        //Attempts to pick a random tile, given the allowed permutations of each tile.
        //Returns the random selection, or nothing if there were no eligible tiles.
        WFC::Nullable<std::tuple<TileIdx, Transform3D>> RandomTile(const TransformSet* allowedPerTile);
    };
}
}