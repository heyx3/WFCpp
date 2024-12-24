#pragma once

#include "../HelperClasses.h"
#include "Pattern.h"


namespace WFC
{
    namespace Simple
    {
        using PixelFrequencyLookup = Dictionary<Pixel, size_t, std::hash<Pixel>>;

        //TODO: Support diagonal reflections.

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
			    return (WillSwapAxes(transform) ?
						    Vector2i(originalSize.y, originalSize.x) :
						    originalSize);
		    }

		    //Gets the pixel at the given position using the given transformation of the input data.
		    inline const Pixel& GetPixel(Vector2i inputPos, Transformations transform) const
			    { return pixelDataByTransform[transform][inputPos]; }

		    //Gets the number of times each pixel appears in this input data.
		    const PixelFrequencyLookup& GetPixelFrequencies() const { return pixelFrequencies; }
		    //Gets all patterns contained in this input data.
		    const std::vector<Pattern>& GetPatterns() const { return patterns; }


	    private:

		    //All the patterns this instance contains.
			std::vector<Pattern> patterns;

		    //The number of times each pixel appears in this input data.
		    PixelFrequencyLookup pixelFrequencies;

		    //Different transformed versions of the input pixel data.
		    Array2D<Pixel> pixelDataByTransform[Transformations::Count];

		    const Array2D<Pixel>& getOriginalData() const { return pixelDataByTransform[Transformations::None]; }
	    };
    }
}