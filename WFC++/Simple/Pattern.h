#pragma once

#include "../HelperClasses.h"

#include <functional>


namespace WFC
{
    namespace Simple
    {
	    //Represents a single pixel in an image.
	    using Pixel = uint32_t;

	    //Input image data for the WFC algorithm.
	    class WFC_API InputData;
	    //The current state/output image data for the WFC algorithm.
	    class WFC_API State;

	    //A 2D grid of pixel data that appears in the input at least once.
	    class WFC_API Pattern
	    {
	    public:

		    //The number of times this pattern appears in the input.
		    unsigned int Frequency;

		    //The transformation of the input data to pull from.
		    Transformations InputDataTransform;
		    //The region in the input data this pattern's pixels come from.
		    Region2i InputDataRegion;

        private:
            //The input data this pattern pulls from.
            const InputData* input;
        public:
            const InputData& Input() const { return *input; }


		    Pattern(const InputData& _input, Transformations inputDataTransform, Region2i inputDataRegion,
			        unsigned int frequency = 1)
			    : input(&_input), InputDataTransform(inputDataTransform), InputDataRegion(inputDataRegion),
			      Frequency(frequency) { }


		    //Gets the pixel at the given offset in the pattern.
		    const Pixel& operator[](const Vector2i& patternPos) const;

		    //Gets the hash value for the given instance.
		    size_t GetHashCode() const;
		    //Gets the hash value for the given instance.
		    //Enables this class to be used as a key in Dictionary<> or std::unordered_map<>.
		    size_t operator()(const Pattern& v) const { return v.GetHashCode(); }

		    //Gets whether this pattern is functionally identical to the given one.
		    bool HasSameData(const Pattern& p) const;

		    //Gets whether this pattern can be placed at the given position
		    //    without contradicting an output pixel that is already set.
		    bool DoesFit(Vector2i outputMinCorner, const State& outputState) const;
	    };
    }
    }