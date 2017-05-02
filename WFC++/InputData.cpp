#include "InputData.h"

using namespace WFC;


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
	//Copy the original input data.
	//If periodic, add wrapped data before/after actual pixel data.
	{
		Vector2i extraBorder;
		if (PeriodicX)
			extraBorder.x = MaxPatternSize.x - 1;
		if (PeriodicY)
			extraBorder.y = MaxPatternSize.y - 1;

		Array2D<Pixel> dataCopy(pixelData.GetDimensions() + (extraBorder * 2));
		for (Vector2i copyPos : Region2i(dataCopy.GetDimensions()))
		{
			Vector2i originalPos = pixelData.Wrap(copyPos - extraBorder);
			dataCopy[copyPos] = pixelData[originalPos];
		}

		pixelDataByTransform[Transformations::None] = std::move(dataCopy);
	}

	//Make transformed copies of the input data.
	const Array2D<Pixel>& originalData = getOriginalData();
	if (useRotations)
	{
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
		Vector2i size = originalData.GetDimensions();

		Array2D<Pixel> transformedData_x(size),
					   transformedData_y(size);

		for (Vector2i originalPos : Region2i(size))
		{
			Pixel pixel = originalData[originalPos];

			Vector2i mirroredPos = originalPos.Transform(Transformations::MirrorX, size);
			transformedData_x[mirroredPos] = pixel;

			mirroredPos = originalPos.Transform(Transformations::MirrorY, size);
			transformedData_y[mirroredPos] = pixel;
		}

		pixelDataByTransform[Transformations::MirrorX] = std::move(transformedData_x);
		pixelDataByTransform[Transformations::MirrorY] = std::move(transformedData_y);
	}

	//Count the pixel frequencies.
	for (Vector2i inputPos : Region2i(pixelData.GetDimensions()))
	{
		Pixel pixel = pixelData[inputPos];
		if (!pixelFrequencies.Contains(pixel))
			pixelFrequencies[pixel] = 0;
		pixelFrequencies[pixel] += 1;
	}

	//Create patterns.
	auto& _this = *this;
	List<Pattern>& _patterns = patterns;
	pixelDataByTransform.DoToEach([&_this, &_patterns, patternSize](Transformations transf)
	{
		auto& transformedData = *_this.GetPixels(transf);
		Vector2i transfPatternSize = (transf == Transformations::Rotate90CW ||
							  		  transf == Transformations::Rotate270CW) ?
									     Vector2i(patternSize.y, patternSize.x) :
										 patternSize;

		Vector2i maxPos = transformedData.GetDimensions() - transfPatternSize + 1;
		for (Vector2i transfPos : Region2i(maxPos))
		{
			Region2i patternRange(transfPos, transfPos + transfPatternSize);
			_patterns.PushBack(Pattern(_this, transf, patternRange));
		}
	});

	//Remove duplicate patterns.
	//Hash the patterns for quicker comparisons.
	List<size_t> patternHashes;
	patternHashes.Resize(patterns.GetSize());
	for (size_t i = 0; i < patterns.GetSize(); ++i)
		patternHashes[i] = patterns[i].GetHashCode();
	for (size_t i = 0; i < patterns.GetSize(); ++i)
	{
		for (size_t j = i - 1; j < patterns.GetSize(); ++j)
		{
			if (patternHashes[i] == patternHashes[j] && patterns[i].HasSameData(patterns[j]))
			{
				patterns[i].Frequency += 1;

				patterns.RemoveAt(j);
				patternHashes.RemoveAt(j);
				j -= 1;
			}
		}
	}
}


const Array2D<Pixel>* InputData::GetPixels(Transformations transform) const
{
	if (pixelDataByTransform.Contains(transform))
		return &pixelDataByTransform[transform];
	else
		return nullptr;
}