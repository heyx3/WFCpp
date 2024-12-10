#include "Grid.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


Grid::Grid(const List<Tile>& inputTiles, const Vector3i& outputSize)
    : InputTiles(inputTiles),
      NPermutedTiles(std::accumulate(InputTiles.begin(), InputTiles.end(),
                                     0, [](int sum, const Tile& tile) { return sum + tile.Permutations.Size(); })),
      Cells(outputSize),
      PossiblePermutations({ (int)inputTiles.GetSize(), outputSize })
{
    assert(inputTiles.GetSize() < TileIdx_INVALID); //The last index is reserved for [null]

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

    DEBUGMEM_ValidateAll();
    Reset();
}

void Grid::Reset()
{
    //Set up Cells.
    CellState startingCellData;
    startingCellData.NPossibilities = NPermutedTiles;
    Cells.Fill(startingCellData);

    //Set up PossiblePermutations.
    for (int tileI = 0; tileI < InputTiles.GetSize(); ++tileI)
        for (const Vector3i& cellPos : Region3i(Cells.GetDimensions()))
            PossiblePermutations[Vector4i(tileI, cellPos)] = InputTiles[tileI].Permutations;

    DEBUGMEM_ValidateAll();
}


bool Grid::IsLegalPlacement(const Vector3i& cellPos,
                            TileIdx tileIdx, Transform3D tilePermutation) const
{
    DEBUGMEM_ValidateAll();

    for (const auto& neighborData : GetNeighbors(cellPos))
    {
        Vector3i neighborPos;
        Directions3D mySide;
        std::tie(neighborPos, mySide) = neighborData;

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
    }

    return true;
}

void Grid::SetCell(const Vector3i& pos, TileIdx tile, Transform3D tilePermutation,
                   Report* report, bool doubleCheckLegalFit, bool canBeChangedInFuture)
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
    for (const auto& neighborData : GetNeighbors(pos))
        if (Cells.IsIndexValid(std::get<0>(neighborData)))
            ApplyFilter(pos, std::get<0>(neighborData), std::get<1>(neighborData), report);

    DEBUGMEM_ValidateAll();
}


void Grid::ClearCells(const Region3i& region, Report* report,
                      bool includeImmutableCells,
                      bool clearedImmutableCellsAreMutableNow)
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
            cell.ChosenPermutation = { }; //Not important, but it keeps unset cells
                                          //    consistent for debugging
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

                            ApplyFilter(outsidePos, clearedPos, GetOpposite(sideTowardsOutside), report);
                        }
                        //Otherwise, the outside cell needs to recompute *its* possibilities
                        //    because the cleared cell may have opened them back up.
                        else
                        {
                            //It's pretty hard to add tile possibilities back in.
                            //Much easier is to recompute them from scratch based on *all* neighbors.
                            ResetCellPossibilities(outsidePos, outsideCell, report);
                            for (const auto& neighborData : GetNeighbors(outsidePos))
                            {
                                Vector3i neighborPos;
                                Directions3D sideTowardsNeighbor;
                                std::tie(neighborPos, sideTowardsNeighbor) = neighborData;
                                if (Cells.IsIndexValid(neighborPos))
                                    ApplyFilter(neighborPos, outsidePos, GetOpposite(sideTowardsNeighbor), report);
                            }
                        }
                    }
                }
    }

    DEBUGMEM_ValidateAll();

    //Any immutable/uncleared cells will affect their newly-cleared neighbors.
    for (const Vector3i& cellPos : unclearedCellsInRegion)
    {
        const auto& cell = Cells[cellPos];
        assert(cell.IsSet());

        for (const auto& neighborData : GetNeighbors(cellPos))
        {
            Vector3i neighborPos;
            Directions3D sideTowardsNeighbor;
            std::tie(neighborPos, sideTowardsNeighbor) = neighborData;
            if (region.Contains(neighborPos) && Cells.IsIndexValid(neighborPos))
                ApplyFilter(cellPos, neighborPos, sideTowardsNeighbor, report);
        }
    }

    DEBUGMEM_ValidateAll();
}
void Grid::ClearCell(const Vector3i& cellPos, Report* report,
                     bool isChangeableAfterwards)
{
    ClearCells(Region3i(cellPos, cellPos + 1), report,
               true, isChangeableAfterwards);
}

void Grid::ApplyFilter(const Vector3i& cellPos,
                       const FacePermutation& face,
                       Report* report)
{
    auto& cell = Cells[cellPos];
    cell.DEBUGMEM_Validate();
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

    DEBUGMEM_ValidateAll();
}
void Grid::ApplyFilter(const Vector3i& cellPos,
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

void Grid::ResetCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report)
{
    //If the cell is already completely empty, don't change anything.
    if (cell.NPossibilities == NPermutedTiles)
        return;

    cell.ChosenTile = TileIdx_INVALID;
    cell.NPossibilities = NPermutedTiles;

    if (report)
        report->GotBoring.PushBack(cellPos);

    for (int tileI = 0; tileI < InputTiles.GetSize(); ++tileI)
        PossiblePermutations[{tileI, cellPos}] = InputTiles[tileI].Permutations;

    DEBUGMEM_ValidateAll();
}