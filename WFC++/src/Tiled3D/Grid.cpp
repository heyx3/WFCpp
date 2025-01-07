#include "Tiled3D/Grid.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


Grid::Grid(const std::vector<Tile>& inputTiles, const Vector3i& outputSize)
    : InputTiles(inputTiles),
      NPermutedTiles(std::accumulate(InputTiles.begin(), InputTiles.end(),
                                     0, [](int sum, const Tile& tile) { return sum + tile.Permutations.Size(); })),
      Cells(outputSize),
      PossiblePermutations({ (int)inputTiles.size(), outputSize })
{
    WFCPP_ASSERT(inputTiles.size() < TileIdx_INVALID); //The last index is reserved for [null]

    //Set up FaceIndices.
    int32_t nextID = 0;
    for (const auto& tile : InputTiles)
        for (const auto& transform : tile.Permutations)
            for (const auto& face : tile.Data.Faces)
                FaceIndices[transform.ApplyToFace(face)] = nextID++;
    int32_t nFacePermutations = nextID;

    //Set up MatchingFaces.
    MatchingFaces = Array2D<TransformSet>((int)InputTiles.size(), nFacePermutations,
                                          TransformSet());
    for (int tileI = 0; tileI < (int)InputTiles.size(); ++tileI)
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
    for (int tileI = 0; tileI < InputTiles.size(); ++tileI)
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

            auto faceIndex = FaceIndices.at(myRequiredFace);
            Vector2i faceLookup{ tileIdx, faceIndex };
            const auto& faces = MatchingFaces[faceLookup];
            bool hasFace = faces.Contains(tilePermutation);
            if (!hasFace)
                return false;
        }
    }

    return true;
}

void Grid::SetCell(Vector3i pos, TileIdx tile, Transform3D tilePermutation,
                   bool canBeChangedInFuture,
                   Report* report, bool assertLegalPlacement)
{
    pos = FilterPos(pos);

    if (assertLegalPlacement)
        WFCPP_ASSERT(IsLegalPlacement(pos, tile, tilePermutation));

    //If the cell is being *replaced* rather than going from unset to set,
    //    then neighbor data is harder to update seamlessly because
    //    it could add tile possibilities as well as remove them.
    //The best and simplest option is to clear this cell as an intermediate step.
    auto& cell = Cells[pos];
    if (cell.IsSet())
        ClearCells(Region3i(pos, pos + 1), report);
    //Note that there's no chance of reallocation, so 'cell' is still valid!
    cell = { tile, tilePermutation, canBeChangedInFuture, 1 };

    //Update the neighbors.
    for (const auto& [neighborPos, faceTowardsNeighbor] : GetNeighbors(pos))
        if (Cells.IsIndexValid(neighborPos))
            ApplyFilter(pos, neighborPos, faceTowardsNeighbor, report);

    DEBUGMEM_ValidateAll();
}
void Grid::SetFace(Vector3i pos, Directions3D dir, const FaceCorners& points, Report* report)
{
    pos = FilterPos(pos);
    WFCPP_ASSERT(Cells.IsIndexValid(pos));

    auto neighborPos = FilterPos(pos + GetFaceDirection(dir));
    bool hasNeighbor = Cells.IsIndexValid(neighborPos);

    CellState* faceCells[2] = {
        &Cells[pos],
        (hasNeighbor ? &Cells[neighborPos] : nullptr)
    };
    Directions3D faceDirs[2] = {
        dir,
        GetOpposite(dir)
    };
    Vector3i faceCellIdcs[2] = {
        pos, neighborPos
    };

    for (int i = 0; i < 2; ++i)
    {
        if (faceCells[i] == nullptr)
            continue;
        auto& cell = *faceCells[i];
        auto faceDir = faceDirs[i];
        auto facePos = faceCellIdcs[i];

        bool needsFiltering;
        if (cell.IsSet())
        {
            const auto& tile = InputTiles[cell.ChosenTile];
            auto chosenFace = GetFace(cell.ChosenTile, cell.ChosenPermutation, faceDir);
            if (chosenFace.Points != points)
            {
                ClearCell(facePos, report, false);
                needsFiltering = true;
            }
            else
            {
                needsFiltering = false;
            }
        }
        else
        {
            needsFiltering = true;
        }

        FaceConstraints[{ facePos, static_cast<int>(faceDir) }] = points;
        if (needsFiltering)
            ApplyFilter(facePos, { faceDir, points }, cell, report);
    }
}

void Grid::ClearCells(const Region3i& region, Report* report,
                      bool includeImmutableCells,
                      bool clearedImmutableCellsAreMutableNow)
{
    //Use a buffer to track the uncleared cells in the region.
    buffer_clearCells_leftovers.clear();
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
                unclearedCellsInRegion.insert(cellPos);
        }
        else
        {
            cell.IsChangeable |= clearedImmutableCellsAreMutableNow;
            #if defined(WFCPP_DEBUG)
                cell.ChosenPermutation = { }; //Give unset cells a standardized value.
            #endif
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
                            RecalculateCellPossibilities(outsidePos, outsideCell, report);
                        }
                    }
                }
    }

    DEBUGMEM_ValidateAll();

    //Any immutable/uncleared cells will affect their newly-cleared neighbors.
    for (const Vector3i& cellPos : unclearedCellsInRegion)
    {
        const auto& cell = Cells[cellPos];
        WFCPP_ASSERT(cell.IsSet());

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
                     bool becomeMutable)
{
    ClearCells(Region3i(cellPos, cellPos + 1), report,
               true, becomeMutable);
}
void Grid::ClearFace(Vector3i cellPos, Directions3D face, Report* report)
{
    cellPos = FilterPos(cellPos);
    if (!Cells.IsIndexValid(cellPos) || FaceConstraints.find({ cellPos, face }) == FaceConstraints.end())
        return;

    RecalculateCellPossibilities(cellPos, Cells[cellPos], report);
}

void Grid::ApplyFilter(const Vector3i& cellPos, const FacePermutation& face,
                       CellState& cell, Report* report)
{
    cell.DEBUGMEM_Validate();
    if (cell.IsSet())
        return;

    //It's possible, if uncommon, that a tileset has no match for a particular face.
    if (!FaceIndices.contains(face))
    {
        for (int i = 0; i < InputTiles.size(); ++i)
            PossiblePermutations[{ i, cellPos }] = { };
        cell.NPossibilities = 0;
    }
    else
    {
        auto faceIdx = FaceIndices[face];
        for (int tileI = 0; tileI < InputTiles.size(); ++tileI)
        {
            const auto& supported = MatchingFaces[{ tileI, faceIdx }];
            auto& available = PossiblePermutations[{ tileI, cellPos }];
            auto nChoicesLost = available.Intersect(supported);

            WFCPP_ASSERT(nChoicesLost <= cell.NPossibilities);
            cell.NPossibilities -= nChoicesLost;
        }
    }

    //If the cell no longer has any tile choices, it's unsolvable.
    if (cell.NPossibilities < 1)
    {
        if (report)
            report->GotUnsolvable.insert(cellPos);
    }
    //Otherwise, it's a candidate of interest since it just had some possibilities narrowed down.
    else
    {
        if (report)
            report->GotInteresting.insert(cellPos);
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
        report->GotBoring.push_back(cellPos);

    for (int tileI = 0; tileI < InputTiles.size(); ++tileI)
        PossiblePermutations[{tileI, cellPos}] = InputTiles[tileI].Permutations;

    //Apply any relevant face constraints.
    for (int dirI = 0; dirI < N_DIRECTIONS_3D; ++dirI)
    {
        auto constraint = FaceConstraints.find({ cellPos, dirI });
        if (constraint != FaceConstraints.end())
        {
            FacePermutation constrainedFace{ static_cast<Directions3D>(dirI), constraint->second };
            ApplyFilter(cellPos, constrainedFace, cell, report);
        }
    }

    DEBUGMEM_ValidateAll();
}
void Grid::RecalculateCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report)
{
    ResetCellPossibilities(cellPos, cell, report);

    for (const auto& neighborData : GetNeighbors(cellPos))
    {
        Vector3i neighborPos;
        Directions3D sideTowardsNeighbor;
        std::tie(neighborPos, sideTowardsNeighbor) = neighborData;

        if (Cells.IsIndexValid(neighborPos))
            ApplyFilter(neighborPos, cellPos, GetOpposite(sideTowardsNeighbor), report);
    }
}