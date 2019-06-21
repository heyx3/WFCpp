#include "State.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


void State::Reset(Vector3i newOutputSize)
{
    //To start with, all output tiles will share the same chances of being anything.
    allTileIDs.Clear();
    for (TileID id = 0; id < (TileID)Input.GetTiles().GetSize(); ++id)
        allTileIDs.Add(id);

    //Re-initialize the output array.
    Output.Reset(newOutputSize.x, newOutputSize.y, newOutputSize.z);
    for (Vector3i pos : Region3i(Output.GetDimensions()))
    {
        auto& oTile = Output[pos];
        oTile.Value = Nullable<TileID>();
        oTile.PossibleTiles = allTileIDs;
    }
}


Nullable<bool> State::Iterate(Vector3i& out_changedPos, List<Vector3i>& out_failedAt)
{
    //Define some useful data.
    auto& outputArray = Output;
    Region3i outputRegion(Output.GetDimensions());

    //Get the tiles that are closest to being certain.
    List<Vector3i> lowestEntropyTilePoses;
    GetBestTiles(lowestEntropyTilePoses);
    
    //If all tiles are already set, we're done.
    if (lowestEntropyTilePoses.GetSize() == 0)
        return true;

    //If some tiles are impossible to solve, handle it.
    auto entropy = (uint_fast32_t)Output[lowestEntropyTilePoses[0]].PossibleTiles.GetSize();
    if (entropy == 0)
    {
        //Either clear out the area to try again, or give up.
        if (ClearSize == Vector3i())
        {
            //Clear the area.
            for (const auto& tilePos : lowestEntropyTilePoses)
                ClearArea(tilePos, ClearSize);

            out_changedPos = Vector3i(-1, -1, -1);
            return Nullable<bool>();
        }
        else
        {
            out_failedAt = std::move(lowestEntropyTilePoses);
            return false;
        }
    }

    //Pick one of the low-entropy tiles at random to have its value set.
    //The correct way to do this is with std::uniform_int_distribution,
    //    but that incurs a LOT of overhead.
    //In practice, the non-uniform distribution of the simpler "rng() % count" is unnoticeable.
    uint_fast32_t chosenTileI = rng() % lowestEntropyTilePoses.GetSize();
    auto chosenTilePos = lowestEntropyTilePoses[chosenTileI];
    auto& chosenTile = Output[chosenTilePos];

    //Pick a tile randomly, but based on their weights.
    TileID chosenTileID;
    //If there's only one possible tile, this is easy.
    if (chosenTile.PossibleTiles.GetSize() == 1)
    {
        for (auto tile : chosenTile.PossibleTiles)
            chosenTileID = tile;
    }
    //Otherwise, make a weighted random choice.
    else
    {
        //Get a list of the values and of their corresonding weights.
        tempTileIDList.Clear();
        tempUintList.Clear();
        for (TileID tileOptionID : chosenTile.PossibleTiles)
        {
            const Tile& tileOption = Input.GetTiles()[tileOptionID];
            tempTileIDList.PushBack(tileOptionID);
            tempUintList.PushBack(tileOption.Weight);
        }

        //Plug that into the RNG.
        std::discrete_distribution<uint_fast32_t> distribution(tempUintList.begin(),
                                                               tempUintList.end());
        chosenTileID = tempTileIDList[distribution(rng)];
    }

    //Finally, set the pixel.
    SetTile(chosenTilePos, chosenTileID);
    out_changedPos = chosenTilePos;

    return Nullable<bool>();
}

bool State::RunToEnd(List<Vector3i>& out_failedAt)
{
    Nullable<bool> result;
    while (!result.HasValue)
        result = Iterate(out_failedAt);
    return result.Value;
}

void State::SetTile(Vector3i tilePos, TileID value, bool permanent)
{
    //Set the pixel.
    auto& outTile = Output[tilePos];
    outTile.PossibleTiles.Clear();
    outTile.PossibleTiles.Add(value);
    outTile.Value = value;
    outTile.IsClearable = !permanent;

    //Update adjacent tiles.
    if (IsPeriodicX | (tilePos.x > 0))
        RecalculateTileChances(FilterPos(tilePos.LessX()));
    if (IsPeriodicX | (tilePos.x < Output.GetWidth() - 1))
        RecalculateTileChances(FilterPos(tilePos.MoreX()));
    if (IsPeriodicY | (tilePos.y > 0))
        RecalculateTileChances(FilterPos(tilePos.LessY()));
    if (IsPeriodicY | (tilePos.y < Output.GetHeight() - 1))
        RecalculateTileChances(FilterPos(tilePos.MoreY()));
    if (IsPeriodicZ | (tilePos.z > 0))
        RecalculateTileChances(FilterPos(tilePos.LessZ()));
    if (IsPeriodicZ | (tilePos.z < Output.GetDepth() - 1))
        RecalculateTileChances(FilterPos(tilePos.MoreZ()));
}

void State::ClearArea(Vector3i center, Vector3i clearSize)
{
    //Calculate the region to be cleared.
    Region3i clearRegion(center - clearSize,
                         center + clearSize + 1);
    if (!IsPeriodicX)
    {
        clearRegion.MinInclusive.x = Clamp(clearRegion.MinInclusive.x,
            0, Output.GetWidth() - 1);
        clearRegion.MaxExclusive.x = Clamp(clearRegion.MaxExclusive.x,
            1, Output.GetWidth());
    }
    if (!IsPeriodicY)
    {
        clearRegion.MinInclusive.y = Clamp(clearRegion.MinInclusive.y,
                                           0, Output.GetHeight() - 1);
        clearRegion.MaxExclusive.y = Clamp(clearRegion.MaxExclusive.y,
                                           1, Output.GetHeight());
    }
    if (!IsPeriodicZ)
    {
        clearRegion.MinInclusive.z = Clamp(clearRegion.MinInclusive.z,
                                           0, Output.GetDepth() - 1);
        clearRegion.MaxExclusive.z = Clamp(clearRegion.MaxExclusive.z,
                                           1, Output.GetDepth());
    }

    //Clear the area.
    //Keep track of all the positions that have been cleared.
    tempPosSet.Clear();
    for (auto posToClear : clearRegion)
    {
        posToClear = FilterPos(posToClear);
        auto& tile = Output[posToClear];

        if (tempPosSet.Add(posToClear))
        {
            if (tile.IsClearable)
            {
                tile.Value = Nullable<TileID>();
                tile.PossibleTiles = allTileIDs;
            }
            else
            {
                tempPosSet.Erase(posToClear);
            }
        }
    }

    //Add the neighbors of the cleared region to the list of affected tiles.
    bool allowMinX = (IsPeriodicX | (clearRegion.MinInclusive.x > 0)),
         allowMaxX = (IsPeriodicX | (clearRegion.MaxExclusive.x < Output.GetWidth())),
         allowMinY = (IsPeriodicY | (clearRegion.MinInclusive.y > 0)),
         allowMaxY = (IsPeriodicY | (clearRegion.MaxExclusive.y < Output.GetHeight())),
         allowMinZ = (IsPeriodicZ | (clearRegion.MinInclusive.z > 0)),
         allowMaxZ = (IsPeriodicZ | (clearRegion.MaxExclusive.z < Output.GetDepth()));
    Vector3i minEdge = FilterPos(clearRegion.MinInclusive - 1),
             maxEdge = FilterPos(clearRegion.MaxExclusive);
    //Left, right, top, and bottom faces:
    for (int z = clearRegion.MinInclusive.z; z < clearRegion.MaxExclusive.z; ++z)
    {
        //Left and right faces.
        for (int y = clearRegion.MinInclusive.y; y < clearRegion.MaxExclusive.y; ++y)
        {
            if (allowMinX)
                tempPosSet.Add(FilterPos(Vector3i(minEdge.x, y, z)));
            if (allowMaxX)
                tempPosSet.Add(FilterPos(Vector3i(maxEdge.x, y, z)));
        }
        //Top and Bottom faces.
        for (int x = clearRegion.MinInclusive.x; x < clearRegion.MaxExclusive.x; ++x)
        {
            if (allowMinY)
                tempPosSet.Add(FilterPos(Vector3i(x, minEdge.y, z)));
            if (allowMaxY)
                tempPosSet.Add(FilterPos(Vector3i(x, maxEdge.y, z)));
        }
    }
    //Front and back faces.
    for (int y = clearRegion.MinInclusive.y; y < clearRegion.MaxExclusive.y; ++y)
        for (int x = clearRegion.MinInclusive.x; x < clearRegion.MaxExclusive.x; ++x)
        {
            if (allowMinZ)
                tempPosSet.Add(FilterPos(Vector3i(x, y, minEdge.z)));
            if (allowMaxZ)
                tempPosSet.Add(FilterPos(Vector3i(x, y, maxEdge.z)));
        }

    //Recalculate probabilities for all affected positions.
    for (auto& affectedPos : tempPosSet)
        RecalculateTileChances(affectedPos);
}

void State::GetBestTiles(List<Vector3i>& outValues) const
{
    //Find the output spaces with the smallest "entropy",
    //    where "entropy" is the sum of all the different tiles the pixel could still become.
    //    (i.e. the sum of the values in its "ColorFrequency" dictionary).
    
    //TODO: Track the min-entropy tiles every time "RecalculateTileChances()" is called, instead of recalculating from scratch here.

	auto currentMinEntropy = std::numeric_limits<uint_fast32_t>::max();
	for (Vector3i outputPos : Region3i(Output.GetDimensions()))
	{
		auto& outTile = Output[outputPos];
		if (!outTile.IsSet())
		{
            //TODO: If a tile has a higher weight, it's more certain to happen. So scale each possible tile's entropy contribution inversely to its weight. This would imply making the entropy a float, but floating-point error is a problem here. So avoid floats by having InputData cache the max weight and do "PossibleTiles.Sum(tile => maxWeight + 1 - tile.Weight)".
            auto thisEntropy = (uint_fast32_t)outTile.PossibleTiles.GetSize();

			//If it's less than the current minimum, then we've found a new minimum.
			if (thisEntropy < currentMinEntropy)
			{
				currentMinEntropy = thisEntropy;
				outValues.Clear();
				outValues.PushBack(outputPos);
			}
			//Otherwise, if it's equal to the current minimum, add it to the list.
			else if (thisEntropy == currentMinEntropy)
			{
				outValues.PushBack(outputPos);
			}
		}
	}
}


void State::RecalculateTileChances(Vector3i tilePos)
{
	tilePos = FilterPos(tilePos);

    //TODO: I feel like there's room for optimization here.

	Region3i outputRegion(Output.GetDimensions());

	//If the tile is already set, nothing needs to be done.
	if (Output[tilePos].IsSet())
		return;

	auto& tile = Output[tilePos];

    //Find any tiles that may fit here.
    //Do this by starting with all tiles, then eliminating any that don't fit
    //    based on each of the adjacent neighbor tiles.
    tile.PossibleTiles = allTileIDs;
    for (int faceI = 0; faceI < N_DIRECTIONS3D; ++faceI)
    {
        auto side = (Directions3D)faceI;

        //Get the neighbor tile at this face.
        auto neighborPos = FilterPos(tilePos + GetFaceDirection(side));
        if (!IsValidPos(neighborPos))
            continue;
        auto& neighborOutput = Output[neighborPos];

        //If the neighbor isn't set yet, we don't care about it.
        if (!neighborOutput.IsSet())
            continue;

        const auto& neighborTile = Input.GetTiles()[neighborOutput.Value.Value];

        //Get all tiles that fit the neighbor tile at this edge.
        auto oppositeSide = GetOpposite(side);
        const auto& neighborMatches = Input.GetTilesWithFace(neighborTile.Data.Faces[oppositeSide]);

        //Remove tiles that don't exist in this set.
        tempTileIDSet = tile.PossibleTiles; //Making a copy
        for (TileID tileOptionID : tempTileIDSet)
            if (!neighborMatches.Contains(tileOptionID))
                tile.PossibleTiles.Erase(tileOptionID);
    }
}