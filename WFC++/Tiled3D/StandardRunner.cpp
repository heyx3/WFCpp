#include "StandardRunner.h"

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
//Calculates the area to clear around a given (presumably unsolvable) cell.
Region3i StandardRunner::GetClearRegion(const Vector3i& cell) const
{
    float temperature = GetTemperature(cell);

    //The minimum radius is 1.
    //Can't make the cell solvable without removing its neighbors!
    WFCPP_ASSERT(temperature >= 0);
    int32_t radius = 1 + (int32_t)temperature;

    //Turn the radius into a rectangular area.
    Vector3i areaMin = Math::Max(Vector3i::Zero(), cell - radius),
             areaMax = Math::Min(History.GetDimensions(), cell + radius + 1);
    return { areaMin, areaMax };
}
//Calculates the priority of handling a given cell.
float StandardRunner::GetPriority(const Vector3i& cellPos)
{
    float entropy = 1.0f - ((float)Grid.Cells[cellPos].NPossibilities /
                              Grid.NPermutedTiles);
    return (PriorityWeightEntropy * entropy) +
           (PriorityWeightTemperature * GetTemperature(cellPos));
}


void StandardRunner::Reset(const Dictionary<Vector3i, std::tuple<TileIdx, Transform3D>>& constants)
{
    Reset();
    for (const auto& constant : constants)
    {
        Vector3i cellPos = std::get<0>(constant);
        TileIdx tileIdx = std::get<0>(std::get<1>(constant));
        Transform3D tilePermutation = std::get<1>(std::get<1>(constant));
        Set(cellPos, tileIdx, tilePermutation, true);
    }
}

void StandardRunner::ClearAround(const Vector3i& centerCellPos)
{
    auto region = GetClearRegion(centerCellPos);
    report.Clear();
    Grid.ClearCells(region, &report);

    //Process the report.
    //Note that the order is important; the report's collections aren't mutually exclusive.
    nextCells.Erase(report.GotBoring);
    nextCells.Add(report.GotInteresting);
    WFCPP_ASSERT(report.GotUnsolvable.GetSize() == 0); //Removing tiles shouldn't make
                                                       //    something unsolvable.

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
void StandardRunner::Set(const Vector3i& cellPos, TileIdx tile, Transform3D permutation,
                         bool makeImmutable)
{
    report.Clear();
    Grid.SetCell(cellPos, tile, permutation, &report,
                 makeImmutable, !makeImmutable);
    nextCells.Erase(cellPos);
    unsolvableCells.Erase(cellPos);

    //Process the report.
    //Note that the order is important, these collections aren't mutually exclusive.
    nextCells.Erase(report.GotBoring);
    nextCells.Add(report.GotInteresting);
    unsolvableCells.Add(report.GotUnsolvable);
    nextCells.Erase(report.GotUnsolvable);

    //Update the cell history.
    auto& history = History[cellPos];
    history.BaseTemperature = Math::Max(0.0f, history.BaseTemperature - CoolOffFromSetting);
}

Vector3i StandardRunner::PickNextCellToSet()
{
    WFCPP_ASSERT(nextCells.GetSize() > 0);

    //Assign a priority to each cell under consideration.
    buffer_pickCell_options.Clear();
    buffer_pickCell_options.Reserve(nextCells.GetSize());
    auto& cellPriorities = buffer_pickCell_options;

    //Get each cell's priority and add it to the candidate list.
    //Also track the current highest-priority.
    float maxPriority = std::numeric_limits<float>().lowest();
    for (const Vector3i& cellPos : nextCells)
    {
        float priority = GetPriority(cellPos);
        cellPriorities.GetUnderlying().emplace_back(cellPos, priority);
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
    cellPriorities.GetUnderlying().erase(newEndIterator, cellPriorities.end());
    WFCPP_ASSERT(cellPriorities.GetSize() > 0);

    //Randomly choose one max-priority cell.
    return std::get<0>(cellPriorities[
        std::uniform_int_distribution<int>(0, static_cast<int>(cellPriorities.GetSize() - 1))(Rand)
    ]);
}

bool StandardRunner::Tick()
{
    CurrentTimestamp += 1;

    //If cells are unsolvable, clear them.
    bool hasUnsolvable = unsolvableCells.GetSize() > 0;
    for (const Vector3i& cellPos : unsolvableCells)
        ClearAround(cellPos);
    unsolvableCells.Clear();
    if (hasUnsolvable)
    {
        return false;
    }
    //If there's no search frontier, re-scan the grid for options.
    else if (nextCells.GetSize() == 0)
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
                nextCells.Add(cellPos);
        }

        //If every cell was set, then the algorithm is done.
        if (nSetCells == Grid.Cells.GetNumbElements())
        {
            return true;
        }
        //If all cells have an equal chance to be set, then pick one at random.
        else if (nextCells.GetSize() == 0)
        {
            Vector3i cellPos;
            for (int i = 0; i < 3; ++i)
                cellPos[i] = std::uniform_int_distribution<int>(0, Grid.Cells.GetDimensions()[i] - 1)(Rand);
            nextCells.Add(cellPos);
        }
    }

    //Pick the highest-priority cell.
    Vector3i cellPos = PickNextCellToSet();
    TileIdx tileIdx;
    Transform3D tilePermutation;
    auto tryRandomTile = RandomTile(&Grid.PossiblePermutations[{ 0, cellPos }]);
    if (tryRandomTile.HasValue)
    {
        std::tie(tileIdx, tilePermutation) = tryRandomTile.Value;
        Set(cellPos, tileIdx, tilePermutation);
    }
    else
    {
        unsolvableCells.Add(cellPos);
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

WFC::Nullable<std::tuple<TileIdx, Transform3D>> StandardRunner::RandomTile(const TransformSet* allowedPerTile)
{
    auto& distributionWeights = buffer_randomTile_weights;

    //Pick a tile, weighting them by their number of possible permutations
    //     (and of course the user's own weights).
    distributionWeights.Clear();
    for (int tileI = 0; tileI < Grid.InputTiles.GetSize(); ++tileI)
        distributionWeights.PushBack(static_cast<float>(allowedPerTile[tileI].Size() * Grid.InputTiles[tileI].Weight));
    int chosenTileI = PickWeightedRandomIndex(Rand, distributionWeights);
    if (chosenTileI < 0)
        return { };

    //Pick a permutation for the tile.
    const auto& permutations = allowedPerTile[chosenTileI];
    WFCPP_ASSERT(permutations.Size() > 0);
    distributionWeights.Resize(static_cast<size_t>(N_ROTATIONS_3D * 2));
    std::fill(distributionWeights.begin(), distributionWeights.end(), 0.0f);
    for (Transform3D tr : permutations)
        distributionWeights[TransformSet::ToBitIdx(tr)] = 1;
    int chosenTransformI = PickWeightedRandomIndex(Rand, distributionWeights);
    WFCPP_ASSERT(chosenTransformI >= 0);


    return std::make_tuple((TileIdx)chosenTileI, TransformSet::FromBit(chosenTransformI));
}