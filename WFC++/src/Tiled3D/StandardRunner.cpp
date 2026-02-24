#include "../../include/Tiled3D/StandardRunner.h"


using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


float StandardRunner::GetTemperature(const Vector3i& cell) const
{
    const auto& history = History[cell];
    float temperature = history.BaseTemperature;

    //Apply a cooling effect over time.
    if (history.LastUnsolvedTime != NEVER_UNSOLVED_TIMESTAMP)
    {
        WFCPP_ASSERT(CurrentTimestamp >= history.LastUnsolvedTime); //Hopefully it's not from the future
        auto elapsed = CurrentTimestamp - history.LastUnsolvedTime;
        temperature -= (elapsed * CoolOffRate);
        temperature = Math::Max(temperature, 0.0f);
    }

    return temperature;
}
Region3i StandardRunner::GetClearRegion(const Vector3i& cell) const
{
    float temperature = GetTemperature(cell);

    //The minimum radius is 1.
    //Can't make the cell solvable without removing its neighbors!
    WFCPP_ASSERT(temperature >= 0);
    int32_t radius;
    if (ClearRegionGrowthRateT <= 0)
        radius = 1;
    else if (ClearRegionGrowthRateT >= 1)
        radius = 1 + (int32_t)temperature;
    else
        radius = 1 + (int32_t)std::pow(temperature, ClearRegionGrowthRateT);

    //Turn the radius into a rectangular area.
    Vector3i areaMin = Math::Max(Vector3i::Zero(), cell - radius),
             areaMax = Math::Min(History.GetDimensions(), cell + radius + 1);
    return { areaMin, areaMax };
}
float StandardRunner::GetPriority(const Vector3i& cellPos)
{
    float entropy = 1.0f - ((float)Grid.Cells[cellPos].NPossibilities /
                              Grid.NPermutedTiles);
    return (PriorityWeightEntropy * entropy) +
           (PriorityWeightTemperature * GetTemperature(cellPos)) +
           (std::uniform_real_distribution<float>{0.0f, PriorityWeightRandomness}(Rand));
}


void StandardRunner::ClearAround(const Vector3i& centerCellPos)
{
    auto region = GetClearRegion(centerCellPos);
    report.Clear();
    Grid.ClearCells(region, &report);

    //Process the report.
    //Note that the order is important; the report's collections aren't mutually exclusive.
    for (const auto& c : report.GotBoring)
        nextCells.erase(c);
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    WFCPP_ASSERT(report.GotUnsolvable.size() == 0); //Removing tiles shouldn't make something unsolvable.

    //Update the unsolvable cell.
    auto& cellHistory = History[centerCellPos];
    cellHistory.LastUnsolvedTime = CurrentTimestamp;

    //Update neighboring temperatures.
    Region3i gridRegion{
        (centerCellPos - 1).Max(       { 0, 0, 0 }        ),
        (centerCellPos + 2).Min(Grid.Cells.GetDimensions())
    };
    for (Vector3i cellPos : gridRegion)
    {
        Vector3i lookupIdx = Vector3i{ 2, 2, 2 } - (gridRegion.MaxExclusive - cellPos - 1);
        auto tempIncrease = TempIncreases[lookupIdx.x][lookupIdx.y][lookupIdx.z];
        History[Grid.FilterPos(cellPos)].BaseTemperature += tempIncrease;
    }
}
void StandardRunner::SetCell(const Vector3i& cellPos, TileIdx tile, Transform3D permutation,
                             bool isPermanent)
{
    report.Clear();
    Grid.SetCell(cellPos, tile, permutation, isPermanent, &report);
    nextCells.erase(cellPos);
    unsolvableCells.erase(cellPos);

    //Process the report.
    //Note that the order is important; these collections aren't mutually exclusive.
    for (const auto& c : report.GotBoring)
        nextCells.erase(c);
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    for (const auto& c : report.GotUnsolvable)
    {
        unsolvableCells.insert(c);
        nextCells.erase(c);
    }

    //Update the cell history.
    //Skip this if we're redoing some unwound history; that shouldn't affect temperature.
    if (CurrentUnwindingCount < 1)
    {
        auto& history = History[cellPos];
        history.BaseTemperature = Math::Max(0.0f, history.BaseTemperature - CoolOffFromSetting);
    }
}
void StandardRunner::UnwindCells(int n)
{
    report.Clear();
    Grid.UnwindActionHistories(n, &report);

    //Process the report.
    //Note that the order is important; these collections aren't mutually exclusive.
    for (const auto& c : report.GotBoring)
        nextCells.erase(c);
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    for (const auto& c : report.GotUnsolvable)
    {
        unsolvableCells.insert(c);
        nextCells.erase(c);
    }
}

void StandardRunner::SetFaceConstraint(const Vector3i& cellPos, Directions3D cellFace,
                                       const FaceIdentifiers& facePermutation,
                                       bool invert)
{
    if (invert)
    {
        SetFaceConstraintNot(cellPos, cellFace, facePermutation);
        return;
    }

    report.Clear();
    Grid.SetFace(cellPos, cellFace, facePermutation, &report);

    //Process the report.
    //Note that the order is important; these collections aren't mutually exclusive.
    WFCPP_ASSERT(report.GotBoring.empty()); //Adding a constraint can't increase possibilities!
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    for (const auto& c : report.GotUnsolvable)
    {
        unsolvableCells.insert(c);
        nextCells.erase(c);
    }
}
void StandardRunner::SetFaceConstraintNot(const Vector3i& cellPos, Directions3D cellFace,
                                          const FaceIdentifiers& facePermutation)
{
    report.Clear();
    Grid.SetFaceNot(cellPos, cellFace, facePermutation, &report);

    //Process the report.
    //Note that the order is important; these collections aren't mutually exclusive.
    WFCPP_ASSERT(report.GotBoring.empty()); //Adding a constraint can't increase possibilities!
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    for (const auto& c : report.GotUnsolvable)
    {
        unsolvableCells.insert(c);
        nextCells.erase(c);
    }
}

void StandardRunner::SetCellConstraintNot(const Vector3i& cellPos, TileIdx tile, TransformSet permutations)
{
    report.Clear();
    Grid.SetCellNot(cellPos, tile, permutations, &report);

    //Process the report.
    //Note that the order is important; these collections aren't mutually exclusive.
    WFCPP_ASSERT(report.GotBoring.empty()); //Adding a constraint can't increase possibilities!
    for (const auto& c : report.GotInteresting)
        nextCells.insert(c);
    for (const auto& c : report.GotUnsolvable)
    {
        unsolvableCells.insert(c);
        nextCells.erase(c);
    }
}

Vector3i StandardRunner::PickNextCellToSet()
{
    WFCPP_ASSERT(nextCells.size() > 0);

    //Assign a priority to each cell under consideration.
    buffer_pickCell_options.clear();
    buffer_pickCell_options.reserve(nextCells.size());
    auto& cellPriorities = buffer_pickCell_options;

    //Get each cell's priority and add it to the candidate list.
    //Also track the current highest-priority.
    float maxPriority = std::numeric_limits<float>().lowest();
    for (const Vector3i& cellPos : nextCells)
    {
        float priority = GetPriority(cellPos);
        cellPriorities.emplace_back(cellPos, priority);
        maxPriority = Math::Max(maxPriority, priority);
    }

    //Filter out the cells of less-than-max priority.
    auto newEndIterator = std::remove_if(
        cellPriorities.begin(), cellPriorities.end(),
        [maxPriority](const std::tuple<Vector3i, float>& option)
        {
            return std::get<1>(option) < maxPriority;
        }
    );
    cellPriorities.erase(newEndIterator, cellPriorities.end());
    WFCPP_ASSERT(cellPriorities.size() > 0);

    //Sort the options, because 'nextCells' is a set and not ordered deterministically.
    std::sort(cellPriorities.begin(), cellPriorities.end(),
              [](const std::tuple<Vector3i, float>& a,
                 const std::tuple<Vector3i, float>& b)
    {
        const auto& vA = std::get<0>(a);
        const auto& vB = std::get<0>(b);
        return (vA.x < vB.x) ||
               (vA.x == vB.x &&
                  (vA.y < vB.y ||
                    (vA.y == vB.y && vA.z < vB.z)));
    });

    //Randomly choose one max-priority cell.
    return std::get<0>(cellPriorities[
        std::uniform_int_distribution<int>(0, static_cast<int>(cellPriorities.size() - 1))(Rand)
    ]);
}

bool StandardRunner::Tick()
{
    CurrentTimestamp += 1;

    //If cells are unsolvable, clear or unwind them.
    bool hasUnsolvable = unsolvableCells.size() > 0;
    if (hasUnsolvable)
    {
        bool usedUnwinding = [&]() {
            if (CurrentUnwindingCount < 1)
                CurrentUnwindingCount = InitialUnwindingCount;
            //If we were redoing previously-undone cells,
            //     and we got at least halfway back towards redoing them all,
            //     then double the unwinding size.
            else if (PlacementsTillFinishedRewinding <= (CurrentUnwindingCount / 2))
                CurrentUnwindingCount *= 2;

            //If the unwinding size got too big, or we can't undo that many cells, give up.
            if (CurrentUnwindingCount >= MaxUnwindingCount || CurrentUnwindingCount < 1 ||
                CurrentUnwindingCount >= Grid.ActionHistory.size())
            {
                CurrentUnwindingCount = -1;
                return false;
            }

            PlacementsTillFinishedRewinding = CurrentUnwindingCount * 2;
            UnwindCells(CurrentUnwindingCount);

            LastAction = StandardRunnerAction_UndoCells{ CurrentUnwindingCount };
            return true;
        }();

        if (!usedUnwinding)
        {
            for (const Vector3i& cellPos : unsolvableCells)
                ClearAround(cellPos);
            LastAction = StandardRunnerAction_ClearCells{ };
        }

        unsolvableCells.clear();
        return false;
    }

    //If there's no search frontier, re-scan the grid for options.
    if (nextCells.size() == 0)
    {
        //Look for any cells with less than full range of possibilities,
        //    and track how many are set.
        size_t nSetCells = 0;
        for (const Vector3i& cellPos : Region3i(Grid.Cells.GetDimensions()))
        {
            const auto& cell = Grid.Cells[cellPos];
            if (cell.IsSet())
                nSetCells += 1;
            else if (cell.NPossibilities < Grid.NPermutedTiles)
                nextCells.insert(cellPos);
        }

        //If every cell was set, then the algorithm is done.
        if (nSetCells == Grid.Cells.GetNumbElements())
        {
            LastAction = StandardRunnerAction_Finish{ };
            return true;
        }
        //If all cells have an equal chance to be set, then pick one at random asap to save memory and time.
        //This doesn't just help the first tick, but any tick after we've given up and cleared everything.
        else if (nextCells.size() == 0)
        {
            //NOTE: there is a small chance of getting into this situation *despite* some cells being set!
            //In particular, if all input tiles/permutations share a particular face,
            //     then there are scenarios where the only boundary between set and unset cells is across that face,
            //     meaning all unset cells have max possibilities.
            //
            //As a result, we need to make sure our random cell isn't one of the set ones.
            //This scenario is very unoptimized, but shouldn't ever happen in the real world.

            Vector3i cellPos;
            do
            {
                for (int i = 0; i < 3; ++i)
                    cellPos[i] = std::uniform_int_distribution<int>(0, Grid.Cells.GetDimensions()[i] - 1)(Rand);
            } while (Grid.Cells[cellPos].IsSet());

            nextCells.insert(cellPos);
        }
    }

    //Pick the highest-priority cell.
    Vector3i cellPos = PickNextCellToSet();
    WFCPP_ASSERT(Grid.Cells.IsIndexValid(cellPos));
    WFCPP_ASSERT(!Grid.Cells[cellPos].IsSet());
    TileIdx tileIdx;
    Transform3D tilePermutation;
    auto tryRandomTile = RandomTile(&Grid.PossiblePermutations[{ 0, cellPos }]);
    if (tryRandomTile.has_value())
    {
        std::tie(tileIdx, tilePermutation) = *tryRandomTile;
        SetCell(cellPos, tileIdx, tilePermutation);
        LastAction = StandardRunnerAction_SetCell{ cellPos, tileIdx, tilePermutation };

        //Update unwiding count logic.
        PlacementsTillFinishedRewinding -= 1;
        if (PlacementsTillFinishedRewinding < 1)
            CurrentUnwindingCount = -1;
    }
    else
    {
        unsolvableCells.insert(cellPos);
        LastAction = StandardRunnerAction_FailedOnCell{ cellPos };
    }

    return false;
}
bool StandardRunner::TickN(int n)
{
    for (int i = 0; i < n; ++i)
        if (Tick())
            return true;
    return false;
}

std::optional<std::tuple<TileIdx, Transform3D>> StandardRunner::RandomTile(const TransformSet* allowedPerTile)
{
    auto& distributionWeights = buffer_randomTile_weights;

    //Pick a tile, weighting them by their number of possible permutations
    //     (and of course the user's own weights).
    distributionWeights.clear();
    for (int tileI = 0; tileI < static_cast<int>(Grid.InputTiles.size()); ++tileI)
        distributionWeights.push_back(static_cast<float>(allowedPerTile[tileI].Size() * Grid.InputTiles[tileI].Weight));
    auto chosenTileI = PickWeightedRandomIndex(Rand, distributionWeights);
    if (chosenTileI < 0)
        return { };

    //Pick a permutation for the tile.
    const auto& permutations = allowedPerTile[chosenTileI];
    WFCPP_ASSERT(permutations.Size() > 0);
    distributionWeights.resize(static_cast<size_t>(N_TRANSFORMS));
    std::fill(distributionWeights.begin(), distributionWeights.end(), 0.0f);
    for (Transform3D tr : permutations)
        distributionWeights[TransformSet::ToBitIdx(tr)] = 1;
    ptrdiff_t chosenTransformI = PickWeightedRandomIndex(Rand, distributionWeights);
    WFCPP_ASSERT(chosenTransformI >= 0);

    return std::make_tuple(
        static_cast<TileIdx>(chosenTileI),
        TransformSet::FromBit(static_cast<uint_fast8_t>(chosenTransformI))
    );
}