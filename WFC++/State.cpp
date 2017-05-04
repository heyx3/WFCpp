#include "State.h"

#include <unordered_set>

using namespace WFC;


void State::Reset(Vector2i newOutputSize)
{
	//Re-initialize the output array.

	//Set the pixel frequencies for every pixel to the frequencies of the input image itself.
	auto& colorFrequencies = Input.GetPixelFrequencies();

	Output.Reset(newOutputSize.x, newOutputSize.y);
	for (Vector2i pos : Region2i(Output.GetDimensions()))
	{
		Output[pos] = OutputPixel();

		auto& pixel = Output[pos];
        pixel.ColorFrequencies.Clear();

		Input.GetPixelFrequencies().DoToEach([&pixel, &colorFrequencies](Pixel key)
		{
			pixel.ColorFrequencies[key] = colorFrequencies[key];
		});
	}
}

std::function<Vector2i(Vector2i)> State::GetPosFilterer() const
{
	Vector2i outputSize = Output.GetDimensions();
	if (PeriodicX)
		if (PeriodicY)
			return [outputSize](Vector2i p) { return Vector2i(Wrap(p.x, outputSize.x), Wrap(p.y, outputSize.y)); };
		else
			return [outputSize](Vector2i p) { return Vector2i(Wrap(p.x, outputSize.x), p.y); };
	else
		if (PeriodicY)
			return [outputSize](Vector2i p) { return Vector2i(p.x, Wrap(p.y, outputSize.y)); };
		else
			return [outputSize](Vector2i p) { return p; };
}

Nullable<bool> State::Iterate(Vector2i& out_changedPos, List<Vector2i>& out_failedAt)
{
    //Define some useful lambdas.
    auto& outputArray = Output;
    Region2i outputRegion(Output.GetDimensions());
    std::function<Vector2i       (Vector2i)> posFilterer = GetPosFilterer();
    std::function<Nullable<Pixel>(Vector2i)> outputColorGetter =
        [posFilterer, &outputRegion, &outputArray](Vector2i pixelPos)
        {
            pixelPos = posFilterer(pixelPos);
            if (outputRegion.Contains(pixelPos))
                return outputArray[pixelPos].Value;
            else
                return Nullable<Pixel>();
        };

	//Get the pixels that are closest to being certain.
	List<Vector2i> lowestEntropyPixelPoses;
	GetBestPixels(lowestEntropyPixelPoses);

	//If all pixels are aleady set, we're done.
	if (lowestEntropyPixelPoses.GetSize() == 0)
		return true;

	//If any pixels are impossible to solve, handle it.
	size_t nColorChoices = Output[lowestEntropyPixelPoses[0]].ColorFrequencies.GetSize();
	if (nColorChoices == 0)
	{
		//Either clear out the violating pixels, or give up.
		if (ViolationClearSize > 0)
		{
            //Clear all violating pixels, and collect all positions that will be affected by this.
            std::unordered_set<Vector2i, Vector2i> affectedPoses;
			for (size_t i = 0; i < lowestEntropyPixelPoses.GetSize(); ++i)
				for (Vector2i affectedPos : ClearArea(lowestEntropyPixelPoses[i]))
                    affectedPoses.emplace(affectedPos);

            //Recalculate positions that will be affected by this.
            for (Vector2i affectedPos : affectedPoses)
                RecalculatePixelChances(affectedPos, posFilterer, outputColorGetter);

            out_changedPos = Vector2i(-1, -1);
            return Nullable<bool>();
		}
		else
		{
			out_failedAt = std::move(lowestEntropyPixelPoses);
			return false;
		}
	}

	//Pick one of these pixels at random to fill in.
	std::uniform_int_distribution<size_t> pixelIndexRange(0, lowestEntropyPixelPoses.GetSize() - 1);
	auto chosenPixelPos = lowestEntropyPixelPoses[pixelIndexRange(rng)];
	auto& chosenPixel = Output[chosenPixelPos];

	//Pick a color at random for the pixel to have.
	Pixel chosenColor;
	//If there's only one possible color, this is easy.
	if (chosenPixel.ColorFrequencies.GetSize() == 1)
	{
		chosenPixel.ColorFrequencies.DoToEach([&chosenColor](const Pixel& key)
		{
			chosenColor = key;
		});
	}
	//Otherwise, make a weighted random choice.
	else
	{
		//Get a list of values and corresponding weights.
		std::vector<Pixel> colors(chosenPixel.ColorFrequencies.GetSize());
		std::vector<size_t> weights(chosenPixel.ColorFrequencies.GetSize());
		size_t index = 0;
		chosenPixel.ColorFrequencies.DoToEach_Pair(
			[&colors, &weights, &index](const Pixel& color, size_t weight)
			{
				colors[index] = color;
				weights[index] = weight;
				index += 1;
			});

		//Plug that into the RNG.
		std::discrete_distribution<size_t> distribution(weights.begin(), weights.end());
		chosenColor = colors[distribution(rng)];
	}

	//Finally set the pixel and wait for the next iteration.
	SetPixel(chosenPixelPos, chosenColor, posFilterer, outputColorGetter);
    out_changedPos = chosenPixelPos;
	return Nullable<bool>();
}

void State::SetPixel(Vector2i pixelPos, Pixel value)
{
    //Define some useful lambdas.
    auto& outputArray = Output;
    Region2i outputRegion(Output.GetDimensions());
    std::function<Vector2i(Vector2i)> posFilterer = GetPosFilterer();
    std::function<Nullable<Pixel>(Vector2i)> outputColorGetter =
        [posFilterer, &outputRegion, &outputArray](Vector2i pixelPos)
    {
        pixelPos = posFilterer(pixelPos);
        if (outputRegion.Contains(pixelPos))
            return outputArray[pixelPos].Value;
        else
            return Nullable<Pixel>();
    };

    SetPixel(pixelPos, value, posFilterer, outputColorGetter);
}
void State::SetPixel(Vector2i pixelPos, Pixel value,
                     std::function<Vector2i(Vector2i)> posFilterer,
                     std::function<Nullable<Pixel>(Vector2i)> outputColorGetter)
{
	Output[pixelPos].Value = value;

	//Any pixel that could share a pattern with the changed pixel needs to be updated.
	for (Vector2i affectedPixel : Region2i(pixelPos - Input.MaxPatternSize,
							               pixelPos + Input.MaxPatternSize + 1))
    {
        RecalculatePixelChances(affectedPixel, posFilterer, outputColorGetter);
    }
}

Region2i State::ClearArea(Vector2i center)
{
	Vector2i clearSize = Input.MaxPatternSize * (int)ViolationClearSize,
			 halfClearSize = clearSize / 2;
	Vector2i clearCenter = center + (Input.MaxPatternSize / 2);

	auto posFilterer = GetPosFilterer();
	Region2i regionToClear(clearCenter - halfClearSize,
						   clearCenter + halfClearSize + 1);
	for (Vector2i posToClear : regionToClear)
	{
		posToClear = posFilterer(posToClear);
		if (Region2i(Output.GetDimensions()).Contains(posToClear))
			Output[posToClear].Value = Nullable<Pixel>();
	}

	return Region2i(regionToClear.MinInclusive - Input.MaxPatternSize + 1,
				    regionToClear.MaxExclusive + Input.MaxPatternSize - 1);
}

void State::GetBestPixels(List<Vector2i>& outValues) const
{
	//Find the pixels with the smallest "entropy",
	//    where "entropy" is the sum of all the different ways the pixel could be given a color
	//    (i.e. the sum of the values in its "ColorFrequency" dictionary).

	size_t minEntropy = std::numeric_limits<size_t>::max();

	for (Vector2i outputPos : Region2i(Output.GetDimensions()))
	{
		auto& pixel = Output[outputPos];
		if (!pixel.Value.HasValue)
		{
            size_t pixelEntropy = pixel.ColorFrequencies.Sum<size_t>(
                [](const size_t& weight) { return weight; });

			//If it's less than the current minimum, we've found a new minimum.
			if (pixelEntropy < minEntropy)
			{
				minEntropy = pixelEntropy;
				outValues.Clear();
				outValues.PushBack(outputPos);
			}
			//Otherwise, we could still have found one with the SAME entropy.
			else if (pixelEntropy == minEntropy)
			{
				outValues.PushBack(outputPos);
			}
		}
	}
}

void State::RecalculatePixelChances(Vector2i pixelPos,
                                    std::function<Vector2i(Vector2i)> posFilterer,
                                    std::function<Nullable<Pixel>(Vector2i)> outputColorGetter)
{
	Region2i outputRegion(Output.GetDimensions());

	auto& inputData = Input;
    auto& outputArray = Output;
	pixelPos = posFilterer(pixelPos);

	//If the position is outside the output, or the pixel is already set,
	//    nothing needs to be done.
	if (!outputRegion.Contains(pixelPos) || Output[pixelPos].Value.HasValue)
		return;

	auto& pixel = Output[pixelPos];

	//Find any colors that, if placed here, would cause a violation of the WFC constraint.
	List<Pixel> badColors;
    Input.GetPixelFrequencies().DoToEach(
		[&](const Pixel& color)
		{
			//Assume that the color is placed.
			std::function<Nullable<Pixel>(Vector2i)> newOutputColorGetter =
				[&](Vector2i outputPos) -> Nullable<Pixel>
				{
					outputPos = posFilterer(outputPos);
					if (outputPos == pixelPos)
						return color;
					else if (outputRegion.Contains(outputPos))
						return outputArray[outputPos].Value;
					else
						return Nullable<Pixel>();
				};

			//Test the constraint: any NxM pattern in the output
			//    appears at least once in the input.
			Region2i nearbyAffectedPixels(pixelPos - inputData.MaxPatternSize + 1,
											pixelPos + 1);
			for (Vector2i nearbyAffectedPixelPos : nearbyAffectedPixels)
			{
                bool passed = false;
				for (size_t patternI = 0; patternI < inputData.GetPatterns().GetSize(); ++patternI)
				{
					if (inputData.GetPatterns()[patternI].DoesFit(nearbyAffectedPixelPos,
									 							    newOutputColorGetter))
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
            if (index == -1 && pattern.DoesFit(patternMinCorner, outputColorGetter))
                pixel.ColorFrequencies[patternColor] += pattern.Frequency;
        }
    }
}