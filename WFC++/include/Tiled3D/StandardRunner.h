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
        //Use a low value for tilesets that produce many small errors requiring limited clearing.
        float ClearRegionGrowthRateT = 0.5f;
        
        //If we get right back to an unsolvable state, we'll double the undo size and try again.
        //After the undo size gets too big, we'll switch to clearing cell areas.
        int InitialUnwindingCount = 4;
        //If heuristics are telling us to do this many undo operations or more,
        //    switch to clearing cells instead.
        int MaxUnwindingCount = 64;
        //Set to -1 when not in the middle of redoing some unwinding operations.
        int CurrentUnwindingCount = -1;
        //Tracks how long until we're finished redoing the last group of unwinding operations.
        //Once this hits 0 we can reset the current unwinding count.
        int PlacementsTillFinishedRewinding = -1;

        //The influence of temperature on how soon a cell should be set.
        //Note that temperature is usually the size of small integers.
        float PriorityWeightTemperature = 0.2f;
        //The influence on a cell's 'NPermutations' value on how soon it should be set.
        //Note that the 'NPermutations' value is fed in as a float from 0 to 1.
        float PriorityWeightEntropy = 0.8f;
        //The random fluctuations of a cell's chances of being set.
        //This can cause lower-entropy cells to get set earlier than higher-entropy ones.
        //However it does not play nicely with 'Unwinding' history,
        //     because it reduces coherency in the action history.
        float PriorityWeightRandomness = 0.0f;

        PRNG Rand;

        Grid Grid;


        //Calculates the temperature of a cell.
        float GetTemperature(const Vector3i& cell) const;
        //Calculates the area to clear around a given (presumably unsolvable) cell.
        Region3i GetClearRegion(const Vector3i& cell) const;
        //Calculates the priority of handling a given cell.
        //Will be a bit randomized each time it's called.
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
            nextCells.clear();
            unsolvableCells.clear();
        }
        void Reset(const std::unordered_map<Vector3i, std::tuple<TileIdx, Transform3D>>& constants);
        //TODO: Another overload that takes new 'constants'.
        
        void SetCell(const Vector3i& cellPos, TileIdx tile, Transform3D permutation,
                     bool makeImmutable = false);
        void UnwindCells(int nToUnwind);

        void SetCellConstraintNot(const Vector3i& cellPos, TileIdx tile, TransformSet permutations = TransformSet::All());
        void SetFaceConstraint(const Vector3i& cellPos, Directions3D cellFace,
                               const FaceIdentifiers& facePermutation,
                               bool invert = false);
        void SetFaceConstraintNot(const Vector3i& cellPos, Directions3D cellFace,
                                  const FaceIdentifiers& facePermutation);


        StandardRunner(const std::vector<Tile>& inputTiles, const Vector3i& gridSize,
                       const std::unordered_map<Vector3i, std::tuple<TileIdx, Transform3D>>* constants = nullptr,
                       PRNG rand = { std::random_device{ }() })
            : History(gridSize, { }), Rand(rand), Grid(inputTiles, gridSize)
        {
            if (constants != nullptr)
                for (const auto& constant : *constants)
                {
                    Vector3i cellPos = std::get<0>(constant);
                    TileIdx chosenTile = std::get<0>(std::get<1>(constant));
                    Transform3D chosenPermutation = std::get<1>(std::get<1>(constant));
                    SetCell(cellPos, chosenTile, chosenPermutation);
                }
        }


    private:
        Grid::Report report;
        std::unordered_set<Vector3i> nextCells, unsolvableCells;
        std::vector<std::tuple<Vector3i, float>> buffer_pickCell_options;
        std::vector<float> buffer_randomTile_weights;
        std::unordered_map<Vector3i, int> buffer_unwindCells_originalNPossibilities;

        void ClearAround(const Vector3i& centerCellPos);

        Vector3i PickNextCellToSet();

        //Attempts to pick a random tile, given the allowed permutations of each tile.
        //Returns the random selection, or nothing if there were no eligible tiles.
        std::optional<std::tuple<TileIdx, Transform3D>> RandomTile(const TransformSet* allowedPerTile);
    };
}
}