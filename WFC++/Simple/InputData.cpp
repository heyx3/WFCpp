#include "InputData.h"

using namespace WFC;
using namespace WFC::Simple;


namespace
{
	int max(int a, int b) { return (a > b) ? a : b; }
}


InputData::InputData(const Array2D<Pixel>& pixelData, Vector2i patternSize,
					 bool periodicX, bool periodicY, bool useRotations, bool useReflections)
	: PeriodicX(periodicX), PeriodicY(periodicY),
	  OriginalPatternSize(patternSize),
	  MaxPatternSize(useReflections ?
					     Vector2i(max(patternSize.x, patternSize.y),
								  max(patternSize.x, patternSize.y)) :
						 patternSize)
{
	std::vector<Transformations> usedTransformations;

	//Copy the original input data.
	//If periodic, add wrapped data after the actual pixel data to simulate wrapping.
	{
		usedTransformations.push_back(Transformations::None);

		Vector2i extraBorder;
		if (PeriodicX)
			extraBorder.x = MaxPatternSize.x - 1;
		if (PeriodicY)
			extraBorder.y = MaxPatternSize.y - 1;

		Array2D<Pixel> dataCopy(pixelData.GetDimensions() + extraBorder);
		for (Vector2i copyPos : Region2i(dataCopy.GetDimensions()))
		{
			Vector2i originalPos = pixelData.Wrap(copyPos);
			dataCopy[copyPos] = pixelData[originalPos];
		}

		pixelDataByTransform[Transformations::None] = std::move(dataCopy);
	}

	//Make transformed copies of the input data.
	const Array2D<Pixel>& originalData = getOriginalData();
	if (useRotations)
	{
		usedTransformations.push_back(Transformations::Rotate90CW);
		usedTransformations.push_back(Transformations::Rotate180);
		usedTransformations.push_back(Transformations::Rotate270CW);

		Vector2i originalSize = originalData.GetDimensions(),
				 rotatedSize(originalSize.y, originalSize.x);

		Array2D<Pixel> transformedData_90(rotatedSize),
					   transformedData_180(originalSize),
					   transformedData_270(rotatedSize);

		for (Vector2i originalPos : Region2i(originalSize))
		{
			Pixel pixel = originalData[originalPos];

			Vector2i rotatedPos = originalPos.Transform(Transformations::Rotate90CW, originalSize);
			transformedData_90[rotatedPos] = pixel;

			rotatedPos = originalPos.Transform(Transformations::Rotate180, originalSize);
			transformedData_180[rotatedPos] = pixel;

			rotatedPos = originalPos.Transform(Transformations::Rotate270CW, originalSize);
			transformedData_270[rotatedPos] = pixel;
		}

		pixelDataByTransform[Transformations::Rotate90CW] = std::move(transformedData_90);
		pixelDataByTransform[Transformations::Rotate180] = std::move(transformedData_180);
		pixelDataByTransform[Transformations::Rotate270CW] = std::move(transformedData_270);
	}
	if (useReflections)
	{
		usedTransformations.push_back(Transformations::FlipX);
		usedTransformations.push_back(Transformations::FlipY);

		Vector2i size = originalData.GetDimensions();

		Array2D<Pixel> transformedData_x(size),
					   transformedData_y(size);

		for (Vector2i originalPos : Region2i(size))
		{
			Pixel pixel = originalData[originalPos];

			Vector2i mirroredPos = originalPos.Transform(Transformations::FlipX, size);
			transformedData_x[mirroredPos] = pixel;

			mirroredPos = originalPos.Transform(Transformations::FlipY, size);
			transformedData_y[mirroredPos] = pixel;
		}

		pixelDataByTransform[Transformations::FlipX] = std::move(transformedData_x);
		pixelDataByTransform[Transformations::FlipY] = std::move(transformedData_y);
	}

	//Create patterns.
	for (Transformations transf : usedTransformations)
	{
		auto& transformedData = pixelDataByTransform[transf];
		Vector2i transfPatternSize = (transf == Transformations::Rotate90CW ||
							  		  transf == Transformations::Rotate270CW) ?
									     Vector2i(patternSize.y, patternSize.x) :
										 patternSize;

		//Iterate across every pattern-sized chunk of the input and make a pattern out of it.
		Vector2i maxPos = transformedData.GetDimensions() - transfPatternSize + 1;
		for (Vector2i transfPos : Region2i(maxPos))
		{
			Region2i patternRange(transfPos, transfPos + transfPatternSize);
			patterns.push_back(Pattern(*this, transf, patternRange));
		}
	}

	//Remove duplicate patterns.
	//Hash the patterns for quicker comparisons.
	std::vector<size_t> patternHashes;
	patternHashes.resize(patterns.size());
	for (size_t i = 0; i < patterns.size(); ++i)
		patternHashes[i] = patterns[i].GetHashcode();
	for (size_t i = 0; i < patterns.size(); ++i)
	{
		for (size_t j = i + 1; j < patterns.size(); ++j)
		{
			if (patternHashes[i] == patternHashes[j] && patterns[i].HasSameData(patterns[j]))
			{
				patterns[i].Frequency += 1;

				patterns.erase(patterns.begin() + j);
				patternHashes.erase(patternHashes.begin() + j);
				j -= 1;
			}
		}
	}

    //Count the pixel frequencies.
    for (size_t i = 0; i < patterns.size(); ++i)
    {
        auto& pattern = patterns[i];
        for (Vector2i patternPos : Region2i(pattern.InputDataRegion.GetSize()))
        {
            Pixel color = pattern[patternPos];
            pixelFrequencies[color] += pattern.Frequency;
        }
    }
}