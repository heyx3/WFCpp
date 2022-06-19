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
        assert(CurrentTimestamp >= history.LastUnsolvedTime); //Hopefully it's not from the future
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
    assert(temperature > 0);
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


void StandardRunner::ClearAround(const Vector3i& centerCellPos)
{
    auto region = GetClearRegion(centerCellPos);
    report.Clear();
    Grid.ClearCells(region, &report);

    //Process the report.
    //Note that the order is important, these collections aren't mutually exclusive.
    nextCells.Erase(report.GotBoring);
    nextCells.Add(report.GotInteresting);
    assert(report.GotUnsolvable.GetSize() == 0); //Removing tiles shouldn't make
                                                 //    something unsolvable.

    //Update the unsolvable cell.
    auto& cellHistory = History[centerCellPos];
    cellHistory.LastUnsolvedTime = CurrentTimestamp;

    //Update neighboring temperatures.
    for (int xI = 0; xI < 2; ++xI)
        for (int yI = 0; yI < 2; ++yI)
            for (int zI = 0; zI < 2; ++zI)
            {
                auto cellPos = Grid.FilterPos(centerCellPos +
                                              Vector3i(xI, yI, zI) - 1);
                History[cellPos].BaseTemperature += TempIncreases[xI][yI][zI];
            }

    //Whatever the algorithm's previous state, it's now unfinished.
    CurrentState = Status::Running;
}
void StandardRunner::Set(const Vector3i& cellPos, TileIdx tile, Transform3D permutation)
{
    report.Clear();
    Grid.SetCell(cellPos, tile, permutation, &report);
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
    //Assign a priority to each cell under consideration.
    buffer_pickCell_priorities.Clear();
    auto& cellPriorities = buffer_pickCell_priorities;

    float maxPriority = std::numeric_limits<float>().lowest();
    for (const Vector3i& cellPos : nextCells)
    {
        float priority = GetPriority(cellPos);
        cellPriorities[cellPos] = priority;
        maxPriority = Math::Max(maxPriority, priority);
    }

    //Filter out the cells of less-than-max priority.
    for (auto iter = cellPriorities.begin(); iter != cellPriorities.end(); ++iter)
        if (iter->second < maxPriority)
            iter = cellPriorities.Unwrap().erase(iter);

    //Pick one cell randomly.
    auto iter = cellPriorities.begin();
    auto idx = std::uniform_int_distribution<int>(0, cellPriorities.GetSize())(Rand);
    std::advance(iter, idx);
    return iter->first;
}

void StandardRunner::Tick()
{
    //If cells are unsolvable, clear them.
    bool hasUnsolvable = unsolvableCells.GetSize() > 0;
    for (const Vector3i& cellPos : unsolvableCells)
        ClearAround(cellPos);
    unsolvableCells.Clear();
    if (hasUnsolvable)
    {
        return;
    }
    //If there's no search frontier, re-scan the grid for options.
    else if (nextCells.GetSize() == 0)
    {
        size_t nSetCells = 0;
        for (const Vector3i& cellPos : Region3i(Grid.Cells.GetDimensions()))
        {
            const auto& cell = Grid.Cells[cellPos];
            if (cell.IsSet())
                nSetCells += 1;
            else if (cell.NPossibilities < Grid.NPermutedTiles)
                nextCells.Add(cellPos);
        }
        
        //If every cell was set, then we're done.
        if (nSetCells == Grid.Cells.GetNumbElements())
        {
            CurrentState = Status::Finished;
            return;
        }
    }

    //Pick the highest-priority cell.
    Vector3i cellPos = PickNextCellToSet();
    const auto& cell = Grid.Cells[cellPos];
    //Pick a tile for it.
    buffer_pickCell_distribution.Clear();
    for (int tileI = 0; tileI < buffer_pickCell_distribution.GetSize(); ++tileI)
        buffer_pickCell_distribution.PushBack(Grid.PossiblePermutations[{ tileI, cellPos }].Size());
    std::discrete_distribution tileDistribution(buffer_pickCell_distribution.begin(),
                                                buffer_pickCell_distribution.end());
    int chosenTileI = tileDistribution(Rand);
    //Pick a permutation for the tile.
    const auto& permutations = Grid.PossiblePermutations[{ chosenTileI, cellPos }];
    buffer_pickCell_distribution.Resize((size_t)N_ROTATIONS_3D * 2);
    std::fill(buffer_pickCell_distribution.begin(), buffer_pickCell_distribution.end(), 0);
    for (Transform3D tr : permutations)
        buffer_pickCell_distribution[TransformSet::ToBitIdx(tr)] = 1;
    std::discrete_distribution permDistribution(buffer_pickCell_distribution.begin(),
                                                buffer_pickCell_distribution.end());
    int chosenTransformI = permDistribution(Rand);
    //Set the tile.
    Set(cellPos, chosenTileI, TransformSet::FromBit(chosenTransformI));
}
void StandardRunner::TickN(int n)
{
    for (int i = 0; i < n && CurrentState != Status::Finished; ++i)
        Tick();
}