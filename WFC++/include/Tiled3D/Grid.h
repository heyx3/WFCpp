#pragma once

#include <array>
#include <tuple>
#include <algorithm>
#include <limits>

#include "Tile.hpp"

namespace WFC
{
    //TODO: Take advantage of std::span<>

    namespace Tiled3D
    {
        using TileIdx = uint16_t;
        constexpr TileIdx TileIdx_INVALID = std::numeric_limits<TileIdx>::max();

        //A 3D space which tiles can be placed in.
        class WFC_API Grid
        {
        public:

            //The state of an output cell.
            struct WFC_API CellState
            {
                WFCPP_MEMORY_CHECK_HEADER(16, "CellState struct");
                
                TileIdx ChosenTile = TileIdx_INVALID;
                Transform3D ChosenPermutation = Transform3D{};

                //Some tiles are given by the user, and this algorithm has to work around them.
                //Those tiles must not be cleared or otherwise manipulated.
                bool IsChangeable = true;

                //Cached count of the data in "PossiblePermutations" at this cell.
                //A value of 0 means "unsolvable".
                //A value of 1 means that it's set, OR that it only has one possiblity left.
                uint16_t NPossibilities = 0;

                //Constructors written explicitly so we can insert breakpoints as needed.
                CellState() { }
                CellState(TileIdx chosenTile, Transform3D chosenPermutation, bool isChangeable, uint16_t nPossibilities)
                    #if !(UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT)
                        : ChosenTile(chosenTile), ChosenPermutation(chosenPermutation), IsChangeable(isChangeable), NPossibilities(nPossibilities)
                    #endif
                {
                    #if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
                        ChosenTile = chosenTile;
                        ChosenPermutation = chosenPermutation;
                        IsChangeable = isChangeable;
                        NPossibilities = nPossibilities;
                    #endif
                }

                bool IsSet() const { return ChosenTile != TileIdx_INVALID; }

                WFCPP_MEMORY_CHECK_FOOTER(16);
            };

            //A record of what happened during some action.
            struct WFC_API Report
            {
                //NOTE: this report may have redundant entries.
                // A cell in 'GotInteresting' should supercede its entry  in 'GotBoring',
                //    and 'GotUnsolvable' should supercede 'GotInteresting'.

                //Cells that no longer have any contraints on their tile options.
                std::vector<Vector3i> GotBoring;
                //Cells that have fewer possible options now.
                std::unordered_set<Vector3i> GotInteresting;
                //Cells that now have zero options for tile placement.
                std::unordered_set<Vector3i> GotUnsolvable;

                void Clear()
                {
                    GotInteresting.clear();
                    GotBoring.clear();
                    GotUnsolvable.clear();
                }
            };


            //The input data:
            const std::vector<Tile> InputTiles;
            const int NPermutedTiles;
            //Assigns a unique, contiguous, 0-based index to every face that appears in the tileset.
            const std::unordered_map<FacePermutation, int32_t>& GetFaceIndices() const { return FaceIndices; }
            //For each input tile (X) and FacePermutation (Y),
            //    stores which tile permutations contain that face.
            //You can get the index for a FacePermutation with 'FaceIndices'.
            const Array2D<TransformSet>& GetMatchingFaces() const { return MatchingFaces; }
            inline void DEBUGMEM_ValidateInputs() const
            {
                //Note: iterate with indices as much as possible so it's clearer in the debugger where validation is failing.
                for (size_t i = 0; i < InputTiles.size(); ++i)
                    InputTiles[i].DEBUGMEM_Validate();
                for (const auto& faceAndIdx : GetFaceIndices())
                    faceAndIdx.first.DEBUGMEM_Validate();
                for (Vector2i idx : Region2i(GetMatchingFaces().GetDimensions()))
                {
                    const auto& data = GetMatchingFaces()[idx];
                    data.DEBUGMEM_Validate();
                }
            }
            
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
            void DEBUGMEM_ValidateOutputs() const
            {
                //Note: iterate with indices as much as possible so it's clearer in the debugger where validation is failing.
                for (Vector3i idx : Region3i(Cells.GetDimensions()))
                    Cells[idx].DEBUGMEM_Validate();
                for (Vector4i idx : Region4i(PossiblePermutations.GetDimensions()))
                    PossiblePermutations[idx].DEBUGMEM_Validate();
            }

            void DEBUGMEM_ValidateAll() const
            {
                #if WFCPP_CHECK_MEMORY
                    DEBUGMEM_ValidateInputs();
                    DEBUGMEM_ValidateOutputs();
                #endif
            }

            //TODO: "Policies". Such as:
            //  * BoundaryPolicy (e.x. wrapping, clamping, or assuming a specific edge face)
            //  * SubsetPolicy (only allow some tiles/permutations in specific locations)
            //  * WeightPolicy (change the weights of tiles/permutations, possibly based on location)

            //The simulation settings:
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
            //NOTE: The above shouldn't be publicly non-const, as changing them affects tile possibilities, but this will be refactored out eventually anyway.

            
            //Allocates a new state for the given tileset and grid size.
            //These will stay constant through the State's lifetime,
            //    but you can configure it to use only a subset of them.
            Grid(const std::vector<Tile>& inputTiles, const Vector3i& outputSize);

            //Sets up this instance for another run.
            void Reset();

            
            bool IsLegalPlacement(const Vector3i& cellPos,
                                  TileIdx tileIdx, Transform3D tilePermutation) const;


            //Overwrites the tile in the given cell (even if it was marked as not changeable).
            void SetCell(Vector3i pos, TileIdx tile, Transform3D tilePermutation,
                         bool canBeChangedInFuture,
                         Report* report = nullptr,
                         bool assertLegalPlacement = true);
            //Forces a particular grid face to fit a specific configuration.
            //
            //If either of the two cells along that face do not fit,
            //     they will be cleared (even if they were marked as not changeable).
            //
            //This constraint will be permanent (until you remove it with 'ClearFace()');
            //    it's not affected by cell clearing.
            void SetFace(Vector3i pos, Directions3D dir,
                         const FaceIdentifiers& points,
                         Report* report = nullptr);

            //Clears out the values of all cells in the given region.
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCells(const Region3i& region, Report* report = nullptr,
                            bool includeImmutableCells = false,
                            bool clearedImmutableCellsAreMutableNow = true);
            //Clears the given cell, even if it's marked as "!IsChangeable".
            //Optionally, even cells marked "!IsChangeable" get cleared.
            void ClearCell(const Vector3i& cellPos, Report* report = nullptr,
                           bool becomeMutable = true);
            //Removes the constraint you previously placed on a specific cell face.
            //If you didn't place a constraint, then nothing happens.
            void ClearFace(Vector3i cell, Directions3D face, Report* report = nullptr);
            //TODO: ClearFace(). Remember to clear both entries, on both sides of the face!


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
            void ApplyFilter(const Vector3i& cellPos, const FacePermutation& chosenFace,
                             CellState& cell,
                             Report* report);
            //Removes tile options from the given cell that do not fit the given face.
            inline void ApplyFilter(const Vector3i& cellPos, const FacePermutation& chosenFace,
                                    Report* report)
            {
                auto& cell = Cells[cellPos];
                ApplyFilter(cellPos, chosenFace, cell, report);
            }
            //Updates a given neighbor of a cell, based on the given cell (presumably set).
            void ApplyFilter(const Vector3i& cellPos,
                             const Vector3i& neighborPos,
                             Directions3D sideTowardsNeighbor,
                             Report* report);

            //Clears a cell's possibilities, resetting it to *all* possible tiles, respecting any FaceConstraints.
            inline void ResetCellPossibilities(const Vector3i& cellPos, Report* report) { ResetCellPossibilities(cellPos, Cells[cellPos], report); }
            void ResetCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report);

            //Recalculates a cell's current possible tiles.
            //Does not bother passing it through the position filter, as it's assumed you already did this.
            //
            //This function should be called after potentially opening up new possibilities,
            //    because finding those is actually harder than recomputing from scratch.
            void RecalculateCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report);
            void RecalculateCellPossibilities(const Vector3i& cellPos, Report* report)
            {
                if (!Cells.IsIndexValid(cellPos))
                    return;
                RecalculateCellPossibilities(cellPos, Cells[cellPos], report);
            }


            //Assigns a unique index to every kind of tile face that appears in the tileset.
            std::unordered_map<FacePermutation, int32_t> FaceIndices;

            //For a tile X and face Y (see 'FaceIndices'),
            //    caches all permutations of the tile which possess that face.
            Array2D<TransformSet> MatchingFaces;

            //Hard-coded constraints for specific faces of specific grid cells.
            //The key's W component is the face Direction3D.
            std::unordered_map<Vector4i, FaceIdentifiers> FaceConstraints;

            std::unordered_set<Vector3i> buffer_clearCells_leftovers;
        };
    }
}