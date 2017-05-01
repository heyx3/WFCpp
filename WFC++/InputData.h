#pragma once

#include "HelperClasses.h"
#include "Pattern.h"


namespace WFC
{
	//Input image data for the WFC algorithm.
	class WFC_API InputData
	{
	public:

		//Whether the input data wraps around an axis.
		const bool PeriodicX, PeriodicY;

		//The size of the patterns, without any transformations.
		const Vector2i OriginalPatternSize;
		//The maximum-possible size of a pattern along each axis.
		//This is different from "OriginalPatternSize" if
		//    the patterns aren't square and rotations on the input data are enabled.
		const Vector2i MaxPatternSize;


		InputData(const Array2D<Pixel>& pixelData, Vector2i patternSize,
				  bool periodicX, bool periodicY, bool useRotations, bool useReflections);


		//Gets the size of the input data (after applying the given transformation).
		inline Vector2i GetSize(Transformations transform = Transformations::None) const
		{
			Vector2i originalSize = getOriginalData().GetDimensions();
			bool isQuarterTurn = (transform == Transformations::Rotate90CW ||
								  transform == Transformations::Rotate270CW);

			return (isQuarterTurn ?
						Vector2i(originalSize.y, originalSize.x) :
						originalSize);
		}
		//Gets the input data after the given transformation.
		//Returns "nullptr" if the given transformation of the data doesn't exist.
		const Array2D<Pixel>* GetPixels(Transformations transform) const;

		//Gets the pixel at the given position using the given transformation of the input data.
		inline const Pixel& GetPixel(Vector2i inputPos, Transformations transform) const
			{ return pixelDataByTransform[transform][inputPos]; }

		//Gets the number of times each pixel appears in this input data.
		const Dictionary<Pixel, size_t>& GetPixelFrequencies() const { return pixelFrequencies; }
		//Gets all patterns contained in this input data.
		const List<Pattern>& GetPatterns() const { return patterns; }


	private:

		//All the patterns this instance contains.
		List<Pattern> patterns;

		//The number of times each pixel appears in this input data.
		Dictionary<Pixel, size_t> pixelFrequencies;

		//Different transformed versions of the input pixel data.
		Dictionary<Transformations, Array2D<Pixel>> pixelDataByTransform;

		const Array2D<Pixel>& getOriginalData() const { return pixelDataByTransform[Transformations::None]; }
	};
}