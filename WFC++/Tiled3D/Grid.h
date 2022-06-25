#pragma once

#include <array>
#include <tuple>
#include <algorithm>

#include "InputData.h"
#include "../Array4D.hpp"

namespace WFC
{
    //TODO: Take advantage of std::span<>

    namespace Tiled3D
    {
        using TileIdx = uint16_t;
        const TileIdx TileIdx_INVALID = 0xffff;

        //A 3D space which tiles can be placed in.
        class WFC_API Grid
        {
        public:

            //TODO: Pull the heavy logic into the cpp.

            //The state of an output cell.
            struct WFC_API CellState
            {
                TileIdx ChosenTile = -1;
                Transform3D ChosenPermutation = Transform3D{};

                //Some tiles are given by the user, and this algorithm has to work around them.
                //Those tiles must not be cleared or otherwise manipulated.
                bool IsChangeable = true;

                //Cached count of the data in "PossiblePermutations" at this cell.
                //A value of 0 means "unsolvable".
                //A value of 1 means that it's set, OR that it only has one possiblity left.
                uint16_t NPossibilities = 0;


                bool IsSet() const { return ChosenTile != (TileIdx)(-1); }
            };

            //A record of what happened during some action.
            struct WFC_API Report
            {
                //NOTE: this report may have redundant entries.
                // A cell in 'GotInteresting' should supercede its entry  in 'GotBoring',
                //    and 'GotUnsolvable' should supercede 'GotInteresting'.

                //Cells that no longer have any contraints on their tile options.
                Set<Vector3i> GotBoring;
                //Cells that have fewer possible options now.
                Set<Vector3i> GotInteresting;
                //Cells that now have zero options for tile placement.
                Set<Vector3i> GotUnsolvable;

                void Clear()
                {
                    GotInteresting.Clear();
                    GotBoring.Clear();
                    GotUnsolvable.Clear();
                }

                template<typename Func>
                void ForeachBoringCell(Func toDo)
                {
                    for (const auto& p : GotBoring)
                        if (!GotInteresting.Contains(p) && !GotUnsolvable.Contains(p))
                            toDo(p);
                }
                template<typename Func>
                void ForeachInterestingCell(Func toDo)
                {
                    for (const auto& p : GotInteresting)
                        if (!GotUnsolvable.Contains(p))
                            toDo(p);
                }
                template<typename Func>
                void ForeachUnsolvableCell(Func toDo)
                {
                    for (const auto& p : GotUnsolvable)
                        toDo(p);
                }
            };


            //The input data:
            const List<Tile> InputTiles;
            const int NPermutedTiles;
            //Assigns a unique, contiguous, 0-based index to every face that appears in the tileset.
            const Dictionary<FacePermutation, int32_t>& GetFaceIndices() const { return FaceIndices; }
            //For each input tile (X) and FacePermutation (Y),
            //    stores which tile permutations contain that face.
            //You can get the index for a FacePermutation with 'FaceIndices'.
            const Array2D<TransformSet>& GetMatchingFaces() const { return MatchingFaces; }
            
            //The output data.
            //While these fields are exposed publicly,
            //    you are strongly encouraged to not modify them directly.
            //TODO: Make them private with const getters, but public in DEBUG builds
            Array3D<CellState> Cells;
            //For each input tile (X), and each cell (YZW),
            //    stores the permutations of that tile
            //    which could possibly be placed at that cell.
            //NOTE: after a cell is set, its entry here no longer gets updated,
            //    so you should check whether a cell is set before paying attention to this data.
            Array4D<TransformSet> PossiblePermutations;

            //TODO: "Policies". Such as:
            //  * BoundaryPolicy (e.x. wrapping, clamping, or assuming a specific edge face)
            //  * SubsetPolicy (only allow some tiles/permutations in specific locations)
            //  * WeightPolicy (change the weights of tiles/permutations, possibly based on location)
            //TODO: "Strategies" that actually run the algorithm with various techniques
            //TOOD: Data about which pointIDs are "friends"

            //The simulation settings:
            //TODO: These shouldn't be publicly non-const, as changing them effects tile possibilities. But don't overdo it, this will be refactored out eventually
            bool IsPeriodicX = false,
                 IsPeriodicY = false,
                 IsPeriodicZ = false;
            inline Vector3i FilterPos(const Vector3i& in) const
            {
                return {
                    (IsPeriodicX ?
                        Math::PositiveModulo(in.x, Cells.GetWidth()) :
                        in.x
                    ),
                    (IsPeriodicY ?
                        Math::PositiveModulo(in.y, Cells.GetHeight()) :
                        in.y
                    ),
                    (IsPeriodicZ ?
                        Math::PositiveModulo(in.z, Cells.GetDepth()) :
                        in.z
                    ),
                };
            }

            
            //Allocates a new state for the given tileset and grid size.
            //These will stay constant through the State's lifetime,
            //    but you can configure it to use only a subset of them.
            Grid(const List<Tile>& inputTiles, const Vector3i& outputSize);

            //Sets up this instance for another run.
            void Reset();

            
            bool IsLegalPlacement(const Vector3i& cellPos,
                                  TileIdx tileIdx, Transform3D tilePermutation) const;


            //Overwrites the tile in the given cell (even if it was marked as not changeable).
            void SetCell(const Vector3i& pos, TileIdx tile, Transform3D tilePermutation,
                         Report* report = nullptr,
                         bool doubleCheckLegalFit = true, bool canBeChangedInFuture = true);

            //Clears out the values of all cells in the given region.
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCells(const Region3i& region, Report* report = nullptr,
                            bool includeImmutableCells = false,
                            bool clearedImmutableCellsAreMutableNow = true);
            //Clears the given cell, even if it's marked as "!IsChangeable".
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCell(const Vector3i& cellPos, Report* report = nullptr,
                           bool isChangeableAfterwards = true);

        private:

            //Gets the position and connecting face towards each neighbor of a cell.
            //The neighbor positions are pre-filtered for convenience.
            inline std::array<std::tuple<Vector3i, Directions3D>, N_DIRECTIONS_3D>
                GetNeighbors(const Vector3i& cellPos) const
            {
                std::array<std::tuple<Vector3i, Directions3D>, N_DIRECTIONS_3D> neighbors;
                neighbors[0] = { FilterPos(cellPos.LessX()), Directions3D::MinX };
                neighbors[1] = { FilterPos(cellPos.MoreX()), Directions3D::MaxX };
                neighbors[2] = { FilterPos(cellPos.LessY()), Directions3D::MinY };
                neighbors[3] = { FilterPos(cellPos.MoreY()), Directions3D::MaxY };
                neighbors[4] = { FilterPos(cellPos.LessZ()), Directions3D::MinZ };
                neighbors[5] = { FilterPos(cellPos.MoreZ()), Directions3D::MaxZ };
                return neighbors;
            }


            //Gets a specific face of a transformed tile, by its side (after transformation).
            inline FacePermutation GetFace(TileIdx tileIdx, Transform3D permutation,
                                           Directions3D sideAfterTransform) const
            {
                return Tiled3D::GetFace(InputTiles[tileIdx].Data,
                                        permutation, sideAfterTransform);
            }

            //Removes tile options from the given cell that do not fit the given face.
            void ApplyFilter(const Vector3i& cellPos,
                             const FacePermutation& face,
                             Report* report);
            //Updates a given neighbor of a cell, based on the given cell (presumably set).
            void ApplyFilter(const Vector3i& cellPos,
                             const Vector3i& neighborPos,
                             Directions3D sideTowardsNeighbor,
                             Report* report);

            inline void ResetCellPossibilities(const Vector3i& cellPos, Report* report) { ResetCellPossibilities(cellPos, Cells[cellPos], report); }
            void ResetCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report);


            Dictionary<FacePermutation, int32_t> FaceIndices;
            Array2D<TransformSet> MatchingFaces;

            Set<Vector3i> buffer_clearCells_leftovers;
        };
    }
}