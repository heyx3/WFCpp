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


        //The state of a WFC algorithm run.
        //This class contains the bulk of the algorithm.
        class WFC_API State
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
                //Cells that have fewer possible options now.
                Set<Vector3i> GotInteresting;
                //Cells that no longer have any contraints on their tile options.
                Set<Vector3i> GotBoring;
                //Cells that now have zero options for tile placement.
                Set<Vector3i> GotUnsolvable;

                void Clear()
                {
                    GotInteresting.Clear();
                    GotBoring.Clear();
                    GotUnsolvable.Clear();
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
            State(const List<Tile>& inputTiles, const Vector3i& outputSize)
                : InputTiles(inputTiles), Cells(outputSize),
                  PossiblePermutations({ (int)inputTiles.GetSize(), outputSize }),
                  NPermutedTiles(std::accumulate(InputTiles.begin(), InputTiles.end(),
                                                 0, [](int sum, const Tile& tile) { return sum + tile.Permutations.Size(); }))
            {
                assert(inputTiles.GetSize() < (TileIdx)(-1)); //The last index is reserved for [null]
                
                //Set up FaceIndices.
                int32_t nextID = 0;
                for (const auto& tile : InputTiles)
                    for (const auto& transform : tile.Permutations)
                        for (const auto& face : tile.Data.Faces)
                            FaceIndices[transform.ApplyToFace(face)] = nextID++;
                int32_t nFacePermutations = nextID;

                //Set up MatchingFaces.
                MatchingFaces = Array2D<TransformSet>((int)InputTiles.GetSize(), nFacePermutations,
                                                      TransformSet());
                for (int tileI = 0; tileI < (int)InputTiles.GetSize(); ++tileI)
                    for (const auto& transform : InputTiles[tileI].Permutations)
                        for (const auto& face : InputTiles[tileI].Data.Faces)
                        {
                            auto transformedFace = transform.ApplyToFace(face);
                            auto facePermIdx = FaceIndices[transformedFace];
                            auto& matches = MatchingFaces[{tileI, facePermIdx}];
                            matches.Add(transform);
                        }

                Reset();
            }

            //Sets up this State for another run.
            void Reset()
            {
                //Set up Cells.
                CellState startingCellData;
                startingCellData.NPossibilities = NPermutedTiles;
                Cells.Fill(startingCellData);

                //Set up PossiblePermutations.
                for (int tileI = 0; tileI < InputTiles.GetSize(); ++tileI)
                    for (const Vector3i& cellPos : Region3i(Cells.GetDimensions()))
                        PossiblePermutations[Vector4i(tileI, cellPos)] = InputTiles[tileI].Permutations;
            }

            
            bool IsLegalPlacement(const Vector3i& cellPos,
                                  TileIdx tileIdx, Transform3D tilePermutation) const
            {
                for (const auto& [neighborPos, mySide] : GetNeighbors(cellPos))
                    if (Cells.IsIndexValid(neighborPos) && Cells[neighborPos].IsSet())
                    {
                        const auto& neighborCell = Cells[neighborPos];
                        auto neighborSide = GetOpposite(mySide);

                        auto neighborFace = GetFace(neighborCell.ChosenTile,
                                                    neighborCell.ChosenPermutation,
                                                    neighborSide);
                        auto myRequiredFace = neighborFace.Flipped();

                        if (!MatchingFaces[{ tileIdx, FaceIndices[myRequiredFace] }].Contains(tilePermutation))
                            return false;
                    }

                return true;
            }


            //Overwrites the tile in the given cell (even if it was marked as not changeable).
            void SetCell(const Vector3i& pos, TileIdx tile, Transform3D tilePermutation,
                         Report* report = nullptr,
                         bool doubleCheckLegalFit = true, bool canBeChangedInFuture = true)
            {
                if (doubleCheckLegalFit)
                    assert(IsLegalPlacement(pos, tile, tilePermutation));

                //If the cell is being *replaced* rather than going from unset to set,
                //    then neighbor data is harder to update seamlessly because
                //    it could add tile possibilities as well as remove them.
                //The best and simplest option is to clear this cell as an intermediate step.
                auto& cell = Cells[pos];
                if (cell.IsSet())
                    ClearCells(Region3i(pos, pos + 1), report);

                cell = { tile, tilePermutation, canBeChangedInFuture, 1 };

                //Update the neighbors.
                for (const auto& [neighborPos, sideTowardsNeighbor] : GetNeighbors(pos))
                    if (Cells.IsIndexValid(neighborPos))
                        ApplyFilter(pos, neighborPos, sideTowardsNeighbor, report);
            }

            //Clears out the values of all cells in the given region.
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCells(const Region3i& region, Report* report = nullptr,
                            bool includeImmutableCells = false,
                            bool clearedImmutableCellsAreMutableNow = true)
            {
                //Use a buffer to track the uncleared cells in the region.
                buffer_clearCells_leftovers.Clear();
                auto& unclearedCellsInRegion = buffer_clearCells_leftovers;

                //Clear the cells.
                for (Vector3i cellPos : region)
                {
                    cellPos = FilterPos(cellPos);
                    auto& cell = Cells[cellPos];

                    if (!cell.IsChangeable && !includeImmutableCells)
                    {
                        //It's not likely for a cell to be immutable and unset, but it's possible.
                        if (cell.IsSet())
                            unclearedCellsInRegion.Add(cellPos);
                    }
                    else
                    {
                        cell.IsChangeable |= clearedImmutableCellsAreMutableNow;
                        cell.ChosenPermutation = { }; //For completeness
                        ResetCellPossibilities(cellPos, cell, report);
                    }
                }

                //The cells on the border of the clear region need to update
                //    their neighbors, and themselves!
                Vector3i outsideCorners[2] = { region.MinInclusive - 1,
                                               region.MaxExclusive };
                for (int axis = 0; axis < 3; ++axis)
                {
                    int faceAxis1 = (axis + 1) % 3,
                        faceAxis2 = (axis + 2) % 3;
                    //Order them for cache-friendly iteration.
                    if (faceAxis1 < faceAxis2)
                        std::swap(faceAxis1, faceAxis2);
                    
                    //Go through every cell on the border of this axis of the clear region.
                    for (int side = 0; side < 2; ++side) // 0 for "min", 1 for "max".
                        for (int faceY = region.MinInclusive[faceAxis1]; faceY < region.MaxExclusive[faceAxis1]; ++faceY)
                            for (int faceX = region.MinInclusive[faceAxis2]; faceX < region.MaxExclusive[faceAxis2]; ++faceX)
                            {
                                Vector3i outsidePos;
                                outsidePos[axis] = outsideCorners[side][axis];
                                outsidePos[faceAxis1] = faceY;
                                outsidePos[faceAxis2] = faceX;
                                outsidePos = FilterPos(outsidePos);

                                if (Cells.IsIndexValid(outsidePos))
                                {
                                    auto& outsideCell = Cells[outsidePos];

                                    //If the outside cell is already set, then its adjacent cleared neighbor
                                    //     should filter out some tile possibilities based on their connecting face.
                                    if (outsideCell.IsSet())
                                    {
                                        Vector3i clearedPos = outsidePos;
                                        int inwardDelta = -((side * 2) - 1);
                                        clearedPos[axis] += inwardDelta;

                                        auto sideTowardsOutside = Tiled3D::MakeDirection3D(side == 0, axis);

                                        ApplyFilter(clearedPos, outsidePos, sideTowardsOutside, report);
                                    }
                                    //Otherwise, the outside cell needs to recompute *its* possibilities
                                    //    because the cleared cell may have opened them back up.
                                    else
                                    {
                                        //It's pretty hard to add tile possibilities back in.
                                        //Much easier is to recompute them from scratch based on *all* neighbors.
                                        ResetCellPossibilities(outsidePos, outsideCell, report);
                                        for (const auto& [neighborPos, sideTowardsNeighbor] : GetNeighbors(outsidePos))
                                            if (Cells.IsIndexValid(neighborPos))
                                                ApplyFilter(neighborPos, outsidePos, GetOpposite(sideTowardsNeighbor), report);
                                    }
                                }
                            }
                }

                //Any immutable/uncleared cells will affect their newly-cleared neighbors.
                for (const Vector3i& cellPos : unclearedCellsInRegion)
                {
                    const auto& cell = Cells[cellPos];
                    assert(cell.IsSet());

                    for (const auto& [neighborPos, sideTowardsNeighbor] : GetNeighbors(cellPos))
                        if (region.Contains(neighborPos) && Cells.IsIndexValid(neighborPos))
                            ApplyFilter(cellPos, neighborPos, sideTowardsNeighbor, report);
                }
            }
            //Clears the given cell, even if it's marked as "!IsChangeable".
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCell(const Vector3i& cellPos, Report* report = nullptr,
                           bool isChangeableAfterwards = true)
            {
                ClearCells(Region3i(cellPos, cellPos + 1), report,
                           true, isChangeableAfterwards);
            }

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
            inline void ApplyFilter(const Vector3i& cellPos,
                                    const FacePermutation& face,
                                    Report* report)
            {
                auto& cell = Cells[cellPos];
                if (cell.IsSet())
                    return;

                //It's possible, if uncommon, that a tileset has no match for a particular face.
                if (!FaceIndices.Contains(face))
                {
                    TransformSet::ClearRow(&PossiblePermutations[{0, cellPos}],
                                           InputTiles.GetSize());
                    cell.NPossibilities = 0;
                }
                else
                {
                    auto faceIdx = FaceIndices[face];
                    for (int tileI = 0; tileI < InputTiles.GetSize(); ++tileI)
                    {
                        const auto& supported = MatchingFaces[{tileI, faceIdx}];
                        auto& available = PossiblePermutations[{tileI, cellPos}];
                        auto nChoicesLost = available.Intersect(supported);

                        assert(nChoicesLost <= cell.NPossibilities);
                        cell.NPossibilities -= nChoicesLost;
                    }
                }

                //If the cell no longer has any tile choices, it's unsolvable.
                if (cell.NPossibilities < 1)
                {
                    if (report)
                        report->GotUnsolvable.Add(cellPos);
                }
                //Otherwise, it's a candidate of interest since it just had some possibilities narrowed down.
                else
                {
                    if (report)
                        report->GotInteresting.Add(cellPos);
                }
            }
            //Updates a given neighbor of a cell, based on the given cell (presumably set).
            inline void ApplyFilter(const Vector3i& cellPos,
                                    const Vector3i& neighborPos,
                                    Directions3D sideTowardsNeighbor,
                                    Report* report)
            {
                auto& cell = Cells[cellPos];
                if (!cell.IsSet())
                    return;

                auto cellFace = GetFace(cell.ChosenTile, cell.ChosenPermutation, sideTowardsNeighbor);
                auto neighborFace = cellFace.Flipped();

                ApplyFilter(neighborPos, neighborFace, report);
            }

            inline void ResetCellPossibilities(const Vector3i& cellPos, Report* report) { ResetCellPossibilities(cellPos, Cells[cellPos], report); }
            void ResetCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report)
            {
                cell.ChosenTile = -1;
                cell.NPossibilities = NPermutedTiles;
                
                if (report)
                    report->GotBoring.Add(cellPos);

                for (int tileI = 0; tileI < InputTiles.GetSize(); ++tileI)
                    PossiblePermutations[{tileI, cellPos}] = InputTiles[tileI].Permutations;
            }


            Dictionary<FacePermutation, int32_t> FaceIndices;
            Array2D<TransformSet> MatchingFaces;

            Set<Vector3i> buffer_clearCells_leftovers;
        };



        class OldState {
        public:
            //A space that may become one of several tiles,
            //    until eventually a single specific tile is chosen.
            struct WFC_API OutputTile
            {
            public:

                //The chosen tile, if it exists yet.
                Nullable<TileID> Value;

                //The different tiles this one might become.
                //If a tile has been chosen, this set will only have one element.
                TileIDSet PossibleTiles;
                //Whether this tile can have its value changed
                //    (e.x. when the algorithm is in an impossible state and needs to back up).
                bool IsClearable = true;

                bool IsSet() const { return Value.HasValue; }
            };


            const InputData& Input;
            Array3D<OutputTile> Output;

            //Whether the output must wrap along each axis.
            bool IsPeriodicX, IsPeriodicY, IsPeriodicZ;

            //If an area with no possible tiles is created,
            //    the area surrounding that problem will be cleared out and regenerated.
            //The area is a cuboid centered on the bad tile,
            //    whose length along each axis is given in tiles (e.x. {0,1,2} means a 1x3x5 box).
            //If this is set to {0,0,0}, this generator just fails intead of clearing space.
            Vector3i ClearSize;


            OldState(const InputData& input, Vector3i outputSize,
                  unsigned int seed, bool periodicX, bool periodicY, bool periodicZ,
                  Vector3i clearSize)
                : Input(input), Output(outputSize),
                  IsPeriodicX(periodicX), IsPeriodicY(periodicY), IsPeriodicZ(periodicZ),
                  rng(seed), ClearSize(clearSize)
            {
                Reset(outputSize);
            }

            void Reset(Vector3i newOutputSize);


            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            Nullable<bool> Iterate(List<Vector3i>& out_failedAt) { Vector3i _; return Iterate(_, out_failedAt); }
            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            //After running, "out_changedPosition" contains the coordinate of the tile that was changed,
            //    assuming the algorithm didn't fail.
            Nullable<bool> Iterate(Vector3i& out_changedPos, List<Vector3i>& out_failedAt);

            //Runs this algorithm to either completion or failure.
            //Returns whether it succeeded.
            bool RunToEnd(List<Vector3i>& out_failedAt);


            //Applies wrapping to each axis as applicable.
            inline Vector3i FilterPos(Vector3i pos) const
            {
                if (IsPeriodicX)
                    pos.x = WFC::Math::PositiveModulo(pos.x, Output.GetWidth());
                if (IsPeriodicY)
                    pos.y = WFC::Math::PositiveModulo(pos.y, Output.GetHeight());
                if (IsPeriodicZ)
                    pos.z = WFC::Math::PositiveModulo(pos.z, Output.GetDepth());
                return pos;
            }
            //Gets whether the given tile position is a valid position in the output array,
            //    taking wrapping into account.
            inline bool IsValidPos(const Vector3i& tilePos) const
            {
                return (IsPeriodicX || ((tilePos.x >= 0) && (tilePos.x < Output.GetWidth()))) &&
                       (IsPeriodicY || ((tilePos.y >= 0) && (tilePos.y < Output.GetHeight()))) &&
                       (IsPeriodicZ || ((tilePos.z >= 0) && (tilePos.z < Output.GetDepth())));
            }

            //Sets the given space to use the given tile.
            //Automatically recalculates probabilities for any neighboring tiles.
            //If "permanent" is true, this tile can never be changed by the algorithm afterwards.
            void SetTile(Vector3i tile, TileID value, bool permanent = false);
            //Clears the given area based on a center and the "clear size".
            //Automatically recalculates probabilities for any neighboring tiles.
            void ClearArea(Vector3i center, Vector3i clearSize);

            //Gets all output tiles that aren't set yet but have extremely few states.
            void GetBestTiles(List<Vector3i>& outPositions) const;


        private:

            TileIDSet allTileIDs;
            PRNG rng;

            //Temp collections that are used inside the algorithm.
            //Allocated once to improve performance.
            TileIDSet tempTileIDSet;
            List<TileID> tempTileIDList;
            List<uint_fast32_t> tempUintList;
            Set<Vector3i> tempPosSet;

            //If the given output tile is unset,
            //    this function recalculates that space's "PossibleTiles" field.
            void RecalculateTileChances(Vector3i tilePos);
        };
    }
}