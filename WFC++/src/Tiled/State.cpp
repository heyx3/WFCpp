#include "../../include/Tiled/State.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled;


void State::Reset(Vector2i newOutputSize)
{
    //To start with, all output tiles will share the same chances of being anything.
    allTileIDs.clear();
    for (TileID id = 0; id < (TileID)Input.GetTiles().size(); ++id)
        allTileIDs.insert(id);

	//Re-initialize the output array.
    Output.Reset(newOutputSize.x, newOutputSize.y);
    for (Vector2i pos : Region2i(Output.GetDimensions()))
    {
        auto& oTile = Output[pos];
        oTile.Value = std::nullopt;
        oTile.PossibleTiles = allTileIDs;
    }
}

std::optional<bool> State::Iterate(Vector2i& out_changedPos, std::vector<Vector2i>& out_failedAt)
{
    //Define some useful data.
    auto& outputArray = Output;
    Region2i outputRegion(Output.GetDimensions());

	//Get the tiles that are closest to being certain.
    std::vector<Vector2i> lowestEntropyTilePoses;
	GetBestTiles(lowestEntropyTilePoses);

	//If all tiles are aleady set, we're done.
	if (lowestEntropyTilePoses.size() == 0)
		return true;

	//If some tiles are impossible to solve, handle it.
	size_t entropy = Output[lowestEntropyTilePoses[0]].PossibleTiles.size();
	if (entropy == 0)
	{
		//Either clear out the area to try again, or give up.
		if (ClearSize > 0)
		{
            //Clear the area.
            std::unordered_set<Vector2i> affectedPoses;
            for (const auto& tilePos : lowestEntropyTilePoses)
                ClearArea(tilePos, affectedPoses);

            //Recalculate output tiles that are affected by this.
            for (auto affectedPos : affectedPoses)
                RecalculateTileChances(affectedPos);

            out_changedPos = Vector2i(-1, -1);
            return std::nullopt;
		}
		else
		{
			out_failedAt = std::move(lowestEntropyTilePoses);
			return false;
		}
	}

	//Pick one of the low-entropy tiles at random ato have its value set.
    //The correct way to do this is with std::uniform_int_distribution,
    //    but that incurs a LOT of overhead.
    //In practice, the non-uniform distribution from the simpler "rng() % count" is unnoticeable.
    size_t chosenTileI = rng() % lowestEntropyTilePoses.size();
    auto chosenTilePos = lowestEntropyTilePoses[chosenTileI];
	auto& chosenTile = Output[chosenTilePos];

	//Pick a tile randomly, but based on their weights.
    TileID chosenTileID;
	//If there's only one possible tile, this is easy.
	if (chosenTile.PossibleTiles.size() == 1)
	{
        for (auto tile : chosenTile.PossibleTiles)
            chosenTileID = tile;
	}
	//Otherwise, make a weighted random choice.
	else
	{
        //Get a list of the values and of their corresonding weights.
        std::vector<TileID> optionValues;
        std::vector<uint32_t> optionWeights;
        for (TileID tileOptionID : chosenTile.PossibleTiles)
        {
            const Tile& tileOption = Input.GetTiles()[tileOptionID];
            optionValues.push_back(tileOptionID);
            optionWeights.push_back(tileOption.Weight);
        }

		//Plug that into the RNG.
		std::discrete_distribution<size_t> distribution(optionWeights.begin(), optionWeights.end());
		chosenTileID = optionValues[distribution(rng)];
	}

	//Finally, set the pixel.
	SetTile(chosenTilePos, chosenTileID);
    out_changedPos = chosenTilePos;

	return std::nullopt;
}

void State::SetTile(Vector2i tilePos, TileID value, bool permanent)
{
    //Set the pixel.
    auto& outTile = Output[tilePos];
    outTile.PossibleTiles.clear();
    outTile.PossibleTiles.insert(value);
    outTile.Value = value;
    outTile.IsDeletable = !permanent;

    //Update adjacent tiles.
    if (PeriodicX | (tilePos.x > 0))
        RecalculateTileChances(Filter(tilePos.LessX()));
    if (PeriodicY | (tilePos.y > 0))
        RecalculateTileChances(Filter(tilePos.LessY()));
    if (PeriodicX | (tilePos.x < Output.GetWidth() - 1))
        RecalculateTileChances(Filter(tilePos.MoreX()));
    if (PeriodicY | (tilePos.y < Output.GetHeight() - 1))
        RecalculateTileChances(Filter(tilePos.MoreY()));
}

void State::ClearArea(Vector2i center, std::unordered_set<Vector2i>& out_affectedPoses)
{
    //Calculate the region to be cleared.
    Region2i clearRegion(center - (int)ClearSize,
                         center + (int)ClearSize + 1);
    if (!PeriodicX)
    {
        clearRegion.MinInclusive.x = Clamp(clearRegion.MinInclusive.x,
                                           0, Output.GetWidth() - 1);
        clearRegion.MaxExclusive.x = Clamp(clearRegion.MaxExclusive.x,
                                           1, Output.GetWidth());
    }
    if (!PeriodicY)
    {
        clearRegion.MinInclusive.y = Clamp(clearRegion.MinInclusive.y,
                                           0, Output.GetHeight() - 1);
        clearRegion.MaxExclusive.y = Clamp(clearRegion.MaxExclusive.y,
                                           1, Output.GetHeight());
    }

    //Clear the area.
    for (auto posToClear : clearRegion)
    {
        posToClear = Filter(posToClear);
        if (out_affectedPoses.insert(posToClear).second)
        {
            auto& tile = Output[posToClear];
            if (tile.IsDeletable)
            {
                tile.Value = std::nullopt;
                tile.PossibleTiles = allTileIDs;
            }
            else
            {
                out_affectedPoses.erase(posToClear);
            }
        }
    }

    //Add the neighbors of the region to the list of affected tiles.
    bool allowMinX = (PeriodicX | (clearRegion.MinInclusive.x > 0)),
         allowMaxX = (PeriodicX | (clearRegion.MaxExclusive.x < Output.GetWidth())),
         allowMinY = (PeriodicY | (clearRegion.MinInclusive.y > 0)),
         allowMaxY = (PeriodicY | (clearRegion.MaxExclusive.y < Output.GetHeight()));
    Vector2i minEdge = Filter(clearRegion.MinInclusive - 1),
             maxEdge = Filter(clearRegion.MaxExclusive);
    //Left and right edges:
    for (int y = clearRegion.MinInclusive.y; y < clearRegion.MaxExclusive.y; ++y)
    {
        if (allowMinX)
            out_affectedPoses.insert(Filter(Vector2i(minEdge.x, y)));
        if (allowMaxX)
            out_affectedPoses.insert(Filter(Vector2i(maxEdge.x, y)));
    }
    //Top and bottom edges:
    for (int x = clearRegion.MinInclusive.x; x < clearRegion.MaxExclusive.x; ++x)
    {
        if (allowMinY)
            out_affectedPoses.insert(Filter(Vector2i(x, minEdge.y)));
        if (allowMaxY)
            out_affectedPoses.insert(Filter(Vector2i(x, maxEdge.y)));
    }
}

void State::GetBestTiles(std::vector<Vector2i>& outValues) const
{
	//Find the output spaces with the smallest "entropy",
	//    where "entropy" is the sum of all the different tiles the pixel could still become.
	//    (i.e. the sum of the values in its "PossibleTiles" dictionary).

    //TODO: Track the min-entropy tiles every time "RecalculateTileChances()" is called, instead of recalculating from scratch here.

	size_t currentMinEntropy = std::numeric_limits<size_t>::max();
	for (Vector2i outputPos : Region2i(Output.GetDimensions()))
	{
		auto& outTile = Output[outputPos];
		if (!outTile.IsSet())
		{
            //TODO: If a tile has a higher weight, it's more certain to happen. So scale each possible tile's entropy contribution inversely to its weight. This would imply making the entropy a float, but floating-point error is a problem here. So avoid floats by having InputData cache the max weight and do "PossibleTiles.Sum(tile => maxWeight + 1 - tile.Weight)".
            size_t thisEntropy = outTile.PossibleTiles.size();

			//If it's less than the current minimum, then we've found a new minimum.
			if (thisEntropy < currentMinEntropy)
			{
				currentMinEntropy = thisEntropy;
				outValues.clear();
				outValues.push_back(outputPos);
			}
			//Otherwise, if it's equal to the current minimum, add it to the list.
			else if (thisEntropy == currentMinEntropy)
			{
				outValues.push_back(outputPos);
			}
		}
	}
}

void State::RecalculateTileChances(Vector2i tilePos)
{
    //TODO: I feel like there's room for optimization here.

	Region2i outputRegion(Output.GetDimensions());

	tilePos = Filter(tilePos);

	//If the tile is already set, nothing needs to be done.
	if (Output[tilePos].IsSet())
		return;

	auto& tile = Output[tilePos];

    //Find any tiles that may fit here.
    //Do this by starting with all tiles, then eliminating any that don't fit
    //    based on each of the four adjacent neighbor tiles.
    tile.PossibleTiles = allTileIDs;
    for (int edgeI = 0; edgeI < 4; ++edgeI)
    {
        EdgeDirs edge = (EdgeDirs)edgeI;

        //Get the neighbor tile at this edge.
        //If it's nonexistent or not set yet, we don't care about it.
        const auto* neighborTileOutput = (*this)[tilePos + GetEdgeDirection(edge)];
        if (neighborTileOutput == nullptr || !neighborTileOutput->IsSet())
            continue;
        const auto& neighborTile = Input.GetTiles()[*neighborTileOutput->Value];

        //Get all tiles that fit the neighbor tile at this edge.
        EdgeDirs neighborEdge = GetOppositeEdge(edge);
        const auto& neighborMatches = Input.GetTilesWithEdge(neighborTile.Edges[neighborEdge],
                                                             edge);

        //Remove tiles that don't exist in this set.
        tempTileIdSet = tile.PossibleTiles; //Making a copy
        for (TileID tileOptionID : tempTileIdSet)
            if (!neighborMatches.contains(tileOptionID))
                tile.PossibleTiles.erase(tileOptionID);
    }
}