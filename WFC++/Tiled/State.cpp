#include "State.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled;


namespace
{
    template<typename T>
    T Min(T a, T b) { return (a < b) ? a : b; }

    template<typename T>
    T Max(T a, T b) { return (a > b) ? a : b; }

    template<typename T>
    T Clamp(T x, T min, T max) { return Min(max, Max(min, x)); }
}


void State::Reset(Vector2i newOutputSize)
{
    //To start with, all output tiles will have a chance of being anything.
    allTileIDs.Clear();
    for (const auto& tile : Input.GetTiles())
        allTileIDs.Add(tile.ID);

	//Re-initialize the output array.
    Output.Reset(newOutputSize.x, newOutputSize.y);
    for (Vector2i pos : Region2i(Output.GetDimensions()))
    {
        auto& oTile = Output[pos];
        oTile.PossibleTiles = allTileIDs;
    }
}

Nullable<bool> State::Iterate(Vector2i& out_changedPos, List<Vector2i>& out_failedAt)
{
    //Define some useful data.
    auto& outputArray = Output;
    Region2i outputRegion(Output.GetDimensions());

	//Get the tiles that are closest to being certain.
	List<Vector2i> lowestEntropyTilePoses;
	GetBestTiles(lowestEntropyTilePoses);

	//If all tiles are aleady set, we're done.
	if (lowestEntropyTilePoses.GetSize() == 0)
		return true;

	//If some tiles are impossible to solve, handle it.
	size_t entropy = Output[lowestEntropyTilePoses[0]].PossibleTiles.GetSize();
	if (entropy == 0)
	{
		//Either clear out the area to try again, or give up.
		if (ClearSize > 0)
		{
            //Clear the area.
            Set<Vector2i> affectedPoses;
            for (const auto& tilePos : lowestEntropyTilePoses)
                ClearArea(tilePos, affectedPoses);

            //Recalculate output tiles that are affected by this.
            for (auto affectedPos : affectedPoses)
                RecalculateTileChances(affectedPos);

            out_changedPos = Vector2i(-1, -1);
            return Nullable<bool>();
		}
		else
		{
			out_failedAt = std::move(lowestEntropyTilePoses);
			return false;
		}
	}

	//Pick one of the low-entropy tiles at random and set its value.
	std::uniform_int_distribution<size_t> pixelIndexRange(0, lowestEntropyTilePoses.GetSize() - 1);
	auto chosenTilePos = lowestEntropyTilePoses[pixelIndexRange(rng)];
	auto& chosenTile = Output[chosenTilePos];

	//Pick a tile based on their weights.
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
		//Get a list of values and corresponding weights.
        List<TileID> optionValues;
        List<uint32_t> optionWeights;
        for (TileID tileOptionID : chosenTile.PossibleTiles)
        {
            const Tile* tileOption = Input.GetTile(tileOptionID);
            optionValues.PushBack(tileOptionID);
            optionWeights.PushBack(tileOption->Weight);
        }

		//Plug that into the RNG.
		std::discrete_distribution<size_t> distribution(optionWeights.begin(), optionWeights.end());
		chosenTileID = optionValues[distribution(rng)];
	}

	//Finally, set the pixel.
	SetTile(chosenTilePos, chosenTileID);
    out_changedPos = chosenTilePos;

	return Nullable<bool>();
}

void State::SetTile(Vector2i tilePos, TileID value)
{
    //Set the pixel.
    auto& outTile = Output[tilePos];
    outTile.PossibleTiles.Clear();
    outTile.PossibleTiles.Add(value);
    outTile.Value = value;

	//Adjacent tiles need to be updated.
    if (tilePos.x > 0)
        RecalculateTileChances(tilePos.LessX());
    if (tilePos.y > 0)
        RecalculateTileChances(tilePos.LessY());
    if (tilePos.x < Output.GetWidth() - 1)
        RecalculateTileChances(tilePos.MoreX());
    if (tilePos.y < Output.GetHeight() - 1)
        RecalculateTileChances(tilePos.MoreY());
}

void State::ClearArea(Vector2i center, Set<Vector2i>& out_affectedPoses)
{
    //Calculate the region to be cleared.
    Region2i clearRegion(center - ClearSize,
                         center + ClearSize + 1);
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

    //Clear it.
    for (const auto& posToClear : clearRegion)
    {
        out_affectedPoses.Add(posToClear);

        auto& tile = Output[Filter(posToClear)];
        tile.Value = Nullable<TileID>();
        tile.PossibleTiles = allTileIDs;
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
            out_affectedPoses.Add(Vector2i(minEdge.x, y));
        if (allowMaxX)
            out_affectedPoses.Add(Vector2i(maxEdge.x, y));
    }
    //Top and bottom edges:
    for (int x = clearRegion.MinInclusive.x; x < clearRegion.MaxExclusive.x; ++x)
    {
        if (allowMinY)
            out_affectedPoses.Add(Vector2i(x, minEdge.y));
        if (allowMaxY)
            out_affectedPoses.Add(Vector2i(x, maxEdge.y));
    }
}

void State::GetBestTiles(List<Vector2i>& outValues) const
{
	//Find the output spaces with the smallest "entropy",
	//    where "entropy" is the sum of all the different tiles the pixel could still become.
	//    (i.e. the sum of the values in its "ColorFrequency" dictionary).

    //TODO: Track the min-entropy tiles every time "RecalculateTileChances()" is called, instead of recalculating from scratch here.

	size_t currentMinEntropy = std::numeric_limits<size_t>::max();
	for (Vector2i outputPos : Region2i(Output.GetDimensions()))
	{
		auto& outTile = Output[outputPos];
		if (!outTile.IsSet())
		{
            //TODO: If a tile has a higher weight, it's more certain to happen. So scale each potential tile's entropy contribution inversely to its weight.
            size_t thisEntropy = outTile.PossibleTiles.GetSize();

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

//TODO: This method! Then it should be done.
void State::RecalculatePixelChances(Vector2i pixelPos)
{
	Region2i outputRegion(Output.GetDimensions());

	auto& inputData = Input;
    auto& outputArray = Output;
	Filter(pixelPos);

	//If the position is outside the output, or the pixel is already set,
	//    nothing needs to be done.
	if (!outputRegion.Contains(pixelPos) || Output[pixelPos].IsSet())
		return;

	auto& pixel = Output[pixelPos];

	//Find any colors that, if placed here, would cause a violation of the WFC constraint.
	List<Pixel> badColors;
    Input.GetPixelFrequencies().DoToEach(
		[&](const Pixel& color)
		{
			//Assume that the color is placed.
            Output[pixelPos].Value = color;

			//Test the constraint: any NxM pattern in the output
			//    appears at least once in the input.
			Region2i nearbyAffectedPixels(pixelPos - inputData.MaxPatternSize + 1,
											pixelPos + 1);
			for (Vector2i nearbyAffectedPixelPos : nearbyAffectedPixels)
			{
                bool passed = false;
				for (size_t patternI = 0; patternI < inputData.GetPatterns().GetSize(); ++patternI)
				{
					if (inputData.GetPatterns()[patternI].DoesFit(nearbyAffectedPixelPos, *this))
					{
                        passed = true;
                        break;
					}
				}
                if (!passed)
                {
                    badColors.PushBack(color);
                    break;
                }
			}

            //Undo the color placement.
            Output[pixelPos].Value = Nullable<Pixel>();
		});

	//Check which patterns can be applied at which positions around this pixel.
    pixel.ColorFrequencies.Clear();
    for (size_t patternI = 0; patternI < Input.GetPatterns().GetSize(); ++patternI)
    {
        //Try placing this pattern everywhere that touches the pixel.

        auto& pattern = Input.GetPatterns()[patternI];
        Vector2i patternSize = pattern.InputDataRegion.GetSize();
        Region2i patternMinCorners(pixelPos - patternSize + 1,
                                    pixelPos + 1);

        for (Vector2i patternMinCorner : patternMinCorners)
        {
            Vector2i patternPos = pixelPos - patternMinCorner;
            Pixel patternColor = pattern[patternPos];

            size_t index = badColors.IndexOf(
                [&patternColor](const Pixel& p) { return p == patternColor; });
            if (index == -1 && pattern.DoesFit(patternMinCorner, *this))
                pixel.ColorFrequencies[patternColor] += pattern.Frequency;
        }
    }
}