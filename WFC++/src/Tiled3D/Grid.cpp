#include "../../include/Tiled3D/Grid.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


Grid::Grid(const std::vector<Tile>& inputTiles, const Vector3i& outputSize)
    : InputTiles(inputTiles),
      NPermutedTiles(std::accumulate(InputTiles.begin(), InputTiles.end(),
                                     0, [](int sum, const Tile& tile) { return sum + tile.Permutations.Size(); })),
      Cells(outputSize),
      PossiblePermutations({ (int)inputTiles.size(), outputSize }),
      InitialPossiblePermutations({ (int)inputTiles.size(), outputSize })
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

    //Set up the initial possible permutation set.
    for (int tileI = 0; tileI < static_cast<int>(InputTiles.size()); ++tileI)
        for (const Vector3i& cellPos : Region3i(Cells.GetDimensions()))
            InitialPossiblePermutations[Vector4i(tileI, cellPos)] = InputTiles[tileI].Permutations;

    DEBUGMEM_ValidateAll();
    Reset();
}

void Grid::Reset()
{
    //Set up Cells.
    CellState startingCellData;
    startingCellData.NPossibilities = NPermutedTiles;
    Cells.Fill(startingCellData);

    InitialPossiblePermutations.MemCopyInto(PossiblePermutations.GetArray());

    //Clear history.
    ActionHistory.clear();
    StatePreActionHistory.clear();
    //Make sure history buffers have enough space to remember the entire grid history.
    ActionHistory.reserve(Cells.GetNumbElements());
    StatePreActionHistory.reserve(Cells.GetNumbElements() * (N_DIRECTIONS_3D + 1) * InputTiles.size());

    DEBUGMEM_ValidateAll();
}


std::tuple<Vector3i, Vector3i, std::vector<TransformSet>::const_iterator, std::vector<TransformSet>::const_iterator>
    Grid::ActionHistoryNeighborInfo(int cellHistoryIdx, int neighborI) const
{
    Vector3i srcCell = ActionHistory[cellHistoryIdx];

    Vector3i neighborCell = srcCell;
    auto [neighborAxis, neighborDir] = ActionHistoryNeighborsData(neighborI);
    neighborCell[neighborAxis] += neighborDir;

    int entriesPerNeighbor = static_cast<int>(InputTiles.size()),
        entriesPerHistoryEntry = entriesPerNeighbor * (N_DIRECTIONS_3D + 1),
        firstPossibilityIdx = (cellHistoryIdx * entriesPerHistoryEntry) + (neighborI * entriesPerNeighbor);
    auto possibilities = std::span{ &StatePreActionHistory[firstPossibilityIdx],
                                    static_cast<size_t>(entriesPerNeighbor) };

    return std::make_tuple(srcCell, neighborCell,
                           StatePreActionHistory.begin() + firstPossibilityIdx,
                           StatePreActionHistory.begin() + firstPossibilityIdx + entriesPerNeighbor);
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
    //The best and simplest option is to clear this cell so that setting it can only subtract possibliities.
    auto& cell = Cells[pos];
    if (cell.IsSet())
        ClearCell(pos, report, canBeChangedInFuture);
    //Note that there's no chance of reallocation, so 'cell' is still valid!

    //Add this event to the action history, so it can be quickly undone later.
    ActionHistory.push_back(pos);
    for (int neighborI = 0; neighborI < N_DIRECTIONS_3D + 1; ++neighborI)
    {
        //Get the neighbor cell in our history ordering.
        auto [neighborAxis, neighborDir] = ActionHistoryNeighborsData(neighborI);
        auto neighborCellPos = pos;
        neighborCellPos[neighborAxis] += neighborDir;
        neighborCellPos = FilterPos(neighborCellPos);

        if (Cells.IsIndexValid(neighborCellPos))
            for (int inputTileI = 0; inputTileI < InputTiles.size(); ++inputTileI)
                StatePreActionHistory.push_back(PossiblePermutations[Vector4i{ inputTileI, neighborCellPos }]);
        else
            for (int inputTileI = 0; inputTileI < InputTiles.size(); ++inputTileI)
                StatePreActionHistory.emplace_back();
    }

    //Update the cell and its neighbors.
    cell = { tile, tilePermutation, canBeChangedInFuture, 1 };
    for (const auto& [neighborPos, faceTowardsNeighbor] : GetNeighbors(pos))
        if (Cells.IsIndexValid(neighborPos))
            ApplyFilter(pos, neighborPos, faceTowardsNeighbor, report, false);

    DEBUGMEM_ValidateAll();
}

void Grid::ClearCells(const Region3i& region, Report* report,
                      bool includeImmutableCells,
                      bool clearedImmutableCellsAreMutableNow)
{
    //Special case: all cells in the clear region are on the top of the history stack,
    //    allowing us to undo them more efficiently and without losing the rest of the grid's history.
    int nCells = region.GetNumbElements();
    if (ActionHistory.size() >= nCells)
    {
        bool allUnwindable = true;
        for (int i = 0; i < nCells; ++i)
        {
            auto cellPos = ActionHistory[ActionHistory.size() - 1 - i];
            if (!region.Contains(cellPos) || (!includeImmutableCells && !Cells[cellPos].IsChangeable))
            {
                allUnwindable = false;
                break;
            }
        }
        if (allUnwindable)
        {
            UnwindActionHistories(nCells, report);
            return;
        }
    }
    //If not for that special case, all cell-placement history will be invalidated.
    ActionHistory.clear();
    StatePreActionHistory.clear();

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

                            ApplyFilter(outsidePos, clearedPos, GetOpposite(sideTowardsOutside), report, false);
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
                ApplyFilter(cellPos, neighborPos, sideTowardsNeighbor, report, false);
        }
    }

    DEBUGMEM_ValidateAll();
}
void Grid::ClearCell(const Vector3i& cellPos, Report* report,
                     bool becomeMutableIfImmutable)
{
    ClearCells(Region3i(cellPos, cellPos + 1), report, true, becomeMutableIfImmutable);
}

void Grid::SetCellNot(Vector3i pos,
                      TileIdx tile, TransformSet specificPermutations,
                      Report* report)
{
    //Clear action history if any exists, because the cached possibilities may now be incorrect.
    ActionHistory.clear();
    StatePreActionHistory.clear();

    Vector4i key{ static_cast<int>(tile), pos };
    InitialPossiblePermutations[key].Remove(specificPermutations);

    auto& cell = Cells[pos];
    if (cell.IsSet())
        ClearCell(pos, report, false);
    else
    {
        auto nRemoved = PossiblePermutations[key].Remove(specificPermutations);
        WFCPP_ASSERT(cell.NPossibilities >= nRemoved);

        cell.NPossibilities -= nRemoved;
        if (report && nRemoved > 0)
            if (cell.NPossibilities < 1)
                report->GotBoring.push_back(pos);
            else if (nRemoved > 0)
                report->GotInteresting.insert(pos);
    }
}
void Grid::SetFaceImpl(Vector3i pos, Directions3D dir, const FaceIdentifiers& points,
                       Report* report, bool isForbidding)
{
    //Clear action history if any exists, because the cached possibilities may now be incorrect.
    ActionHistory.clear();
    StatePreActionHistory.clear();

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
            if ((chosenFace.Points == points) == isForbidding)
            {
                ClearCell(facePos, report, false);
                needsFiltering = true;
            }
            else //The cell's choice already respects this new constraint
            {
                needsFiltering = false;
            }
        }
        else
        {
            needsFiltering = true;
        }

        FacePermutation perm{ faceDir, points };
        if (needsFiltering)
            ApplyFilter(facePos, perm, cell, report, isForbidding);
        ApplyInitialFilter(facePos, perm, isForbidding);
    }
}

void Grid::ApplyFilter(const Vector3i& cellPos, const FacePermutation& face,
                       CellState& cell, Report* report,
                       bool isForbidding)
{
    cell.DEBUGMEM_Validate();
    if (cell.IsSet())
        return;
    auto initialNPossibilities = cell.NPossibilities;

    //It's possible, if uncommon, that a tileset has no match for a particular face.
    if (!FaceIndices.contains(face))
    {
        if (!isForbidding)
        {
            for (int i = 0; i < static_cast<int>(InputTiles.size()); ++i)
                PossiblePermutations[{ i, cellPos }] = { };
            cell.NPossibilities = 0;
        }
    }
    else
    {
        auto faceIdx = FaceIndices[face];
        for (int tileI = 0; tileI < static_cast<int>(InputTiles.size()); ++tileI)
        {
            const auto& supported = MatchingFaces[{ tileI, faceIdx }];
            auto& available = PossiblePermutations[{ tileI, cellPos }];
            auto nChoicesLost = isForbidding ? available.Remove(supported) : available.Intersect(supported);

            WFCPP_ASSERT(nChoicesLost <= cell.NPossibilities);
            cell.NPossibilities -= nChoicesLost;
        }
    }

    if (report && initialNPossibilities != cell.NPossibilities)
    {
        //If the cell no longer has any tile choices, it's unsolvable.
        if (cell.NPossibilities < 1)
        {
            report->GotUnsolvable.insert(cellPos);
        }
        //Otherwise, it's a candidate of interest since it just had some possibilities narrowed down.
        else
        {
            report->GotInteresting.insert(cellPos);
        }
    }

    DEBUGMEM_ValidateAll();
}
void Grid::ApplyFilter(const Vector3i& cellPos,
                       const Vector3i& neighborPos,
                       Directions3D sideTowardsNeighbor,
                       Report* report, bool isForbidding)
{
    auto& cell = Cells[cellPos];
    if (!cell.IsSet())
        return;

    auto cellFace = GetFace(cell.ChosenTile, cell.ChosenPermutation, sideTowardsNeighbor);
    auto neighborFace = cellFace.Flipped();

    ApplyFilter(neighborPos, neighborFace, report, isForbidding);
}

void Grid::ApplyInitialFilter(const Vector3i& cellPos,
                              const FacePermutation& face,
                              bool isForbidding)
{
    //It's possible, if uncommon, that a tileset has no match for a particular face.
    if (!FaceIndices.contains(face))
    {
        for (int i = 0; i < static_cast<int>(InputTiles.size()); ++i)
            InitialPossiblePermutations[{ i, cellPos }] = { };
    }
    else
    {
        auto faceIdx = FaceIndices[face];
        for (int tileI = 0; tileI < static_cast<int>(InputTiles.size()); ++tileI)
        {
            const auto& supported = MatchingFaces[{ tileI, faceIdx }];
            auto& available = InitialPossiblePermutations[{ tileI, cellPos }];

            if (isForbidding)
                available.Remove(supported);
            else
                available.Intersect(supported);
        }
    }

    DEBUGMEM_ValidateAll();
}

void Grid::ResetCellPossibilities(const Vector3i& cellPos, CellState& cell, Report* report)
{
    //If the cell is already completely empty, don't change anything.
    if (cell.NPossibilities == NPermutedTiles)
        return;

    cell.ChosenTile = TileIdx_INVALID;

    if (report)
        report->GotBoring.push_back(cellPos);

    cell.NPossibilities = 0;
    for (int tileI = 0; tileI < static_cast<int>(InputTiles.size()); ++tileI)
    {
        Vector4i key{ tileI, cellPos };
        auto initialPossibilities = InitialPossiblePermutations[key];

        PossiblePermutations[key] = initialPossibilities;
        cell.NPossibilities += initialPossibilities.Size();
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
            ApplyFilter(neighborPos, cellPos, GetOpposite(sideTowardsNeighbor), report, false);
    }
}

void Grid::UnwindActionHistory(Report* report)
{
    WFCPP_ASSERT(!ActionHistory.empty());
    WFCPP_ASSERT(StatePreActionHistory.size() == ActionHistory.size() * (N_DIRECTIONS_3D + 1) * InputTiles.size());

    decltype(StatePreActionHistory)::const_iterator historyDataStart, historyDataEnd;
    for (int neighborI = 0; neighborI < N_DIRECTIONS_3D + 1; ++neighborI)
    {
        auto [selfCell, neighborCellPos, neighborPermutationsStart, neighborPermutationsEnd] =
            ActionHistoryNeighborInfo(static_cast<int>(ActionHistory.size() - 1), neighborI);
        
        if (neighborI == 0)
            historyDataStart = neighborPermutationsStart;
        else if (neighborI == N_DIRECTIONS_3D)
            historyDataEnd = neighborPermutationsEnd;

        if (Cells.IsIndexValid(neighborCellPos))
        {
            auto& neighborCell = Cells[neighborCellPos];

            int originalNPossibilities = neighborCell.NPossibilities;
            neighborCell.NPossibilities = 0;

            for (int tileI = 0; tileI < InputTiles.size(); ++tileI)
            {
                auto possibilities = *(neighborPermutationsStart + tileI);
                neighborCell.NPossibilities += possibilities.Size();

                //TODO: Try memcpy-ing all tile permutations at once.
                PossiblePermutations[Vector4i{ tileI, neighborCellPos }] = possibilities;
            }

            if (report)
            {
                if (neighborCell.NPossibilities == 0 && originalNPossibilities > 0)
                    report->GotUnsolvable.insert(neighborCellPos);
                else if (neighborCell.NPossibilities == NPermutedTiles && originalNPossibilities < NPermutedTiles)
                    report->GotBoring.push_back(neighborCellPos);
                else if (neighborCell.NPossibilities < originalNPossibilities)
                    report->GotInteresting.insert(neighborCellPos);
            }

        }
    }

    //Unset the cell itself.
    auto& cell = Cells[ActionHistory.back()];
    cell = { TileIdx_INVALID, { }, cell.IsChangeable, cell.NPossibilities };
    if (report)
        report->GotInteresting.insert(ActionHistory.back());

    ActionHistory.pop_back();
    StatePreActionHistory.erase(historyDataStart, historyDataEnd);
}
void Grid::UnwindActionHistories(int n, Report* report)
{
    buffer_unwindCells_originalNPossibilities.clear();
    auto& originalNPossibilitiesPerCell = buffer_unwindCells_originalNPossibilities;

    for (int i = 0; i < n; ++i)
    {
        const auto& cellPos = ActionHistory.back();
        if (report && !originalNPossibilitiesPerCell.contains(cellPos))
            originalNPossibilitiesPerCell[cellPos] = Cells[cellPos].NPossibilities;

        UnwindActionHistory(report);
    }

    //Straighten out any kinks in the report from undoing multiple actions.
    if (report)
    {
        for (const auto& [cellPos, originalNPossibilities] : originalNPossibilitiesPerCell)
        {
            int newNPossibilities = Cells[cellPos].NPossibilities;
            //If we ended up with some possibilities in this cell,
            //     then it doesn't matter if it was unsolvable earlier.
            //This fix probably isn't required, due to the fact that
            //     the cell should also be in 'GotInteresting',
            //     but leaving it in there screws up some sanity checks.
            if (newNPossibilities > 0)
                report->GotUnsolvable.erase(cellPos);
        }
    }
}