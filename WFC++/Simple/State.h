#pragma once

#include "../Platform.h"
#include "../HelperClasses.h"
#include "InputData.h"


namespace WFC
{
    namespace Simple
    {
	    //The state of a WFC algorithm run.
	    //This class contains the bulk of the algorithm.
	    class WFC_API State
	    {
	    public:

		    struct WFC_API OutputPixel
		    {
		    public:
			    //The chosen value for this pixel.
			    Nullable<Pixel> Value;
			    //The number of ways this pixel can become various colors.
			    PixelFrequencyLookup ColorFrequencies;
		    };


		    const InputData& Input;
		    Array2D<OutputPixel> Output;

		    //Whether the output wraps along each axis.
		    bool PeriodicX, PeriodicY;

		    //If a constraint violation is found,
		    //    an area surrounding that violation will be cleared out and regenerated.
		    //The size of that area is [pattern size] * ViolationClearSize.
		    //If this is set to 0, this generator just fails instead of clearing out the violation.
		    size_t ViolationClearSize;


		    State(const InputData& input, Vector2i outputSize,
			      unsigned int seed, bool periodicX, bool periodicY, size_t violationClearSize)
			    : Input(input), Output(outputSize), rng(seed),
                  PeriodicX(periodicX), PeriodicY(periodicY),
                  ViolationClearSize(violationClearSize)
		    {
			    Reset(outputSize);
		    }


		    inline const OutputPixel* operator[](Vector2i pos) const
		    {
			    Filter(pos);
			    return (Region2i(Output.GetDimensions()).Contains(pos)) ?
				           &Output[pos] :
					       nullptr;
		    }
		    inline OutputPixel* operator[](Vector2i pos)
		    {
			    Filter(pos);
			    return (Region2i(Output.GetDimensions()).Contains(pos)) ?
				           &Output[pos] :
					       nullptr;
		    }
		    inline void Filter(Vector2i& pos) const
		    {
			    pos.x = (PeriodicX ? Wrap(pos.x, Output.GetWidth()) : pos.x);
			    pos.y = (PeriodicY ? Wrap(pos.y, Output.GetHeight()) : pos.y);
		    }


		    void Reset(Vector2i newOutputSize);

            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            Nullable<bool> Iterate(List<Vector2i>& out_failedAt) { Vector2i _; return Iterate(_, out_failedAt); }
            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            //After running, "out_changedPos" contains the pixel that was changed,
            //    assuming the algorithm didn't fail.
            Nullable<bool> Iterate(Vector2i& out_changedPos, List<Vector2i>& out_failedAt);
		
		    //Sets the given pixel to have the given value.
		    //Updates the status of neighboring pixels to take this into account.
		    void SetPixel(Vector2i pixelPos, Pixel value);
            //Clears all output pixels surrounding the given pixel.
		    //The size of the area to clear is determined by ViolationClearSize.
		    //Assumes that ViolationClearSize is greater than 0.
            //Returns the range of pixels whose probabilities will be affected by this.
            Region2i ClearArea(Vector2i center);


	    private:

            PRNG rng;


		    static inline int Wrap(int val, int maxExclusive)
		    {
			    //http://stackoverflow.com/questions/3417183/modulo-of-negative-numbers
			    val %= maxExclusive;
			    return (val < 0) ? (val + maxExclusive) : val;
		    }

		    //Gets all output pixels with the fewest number of possible colors.
		    //Ignores any pixels whose color is already set.
		    void GetBestPixels(List<Vector2i>& outValues) const;

		    //If the given output pixel is unset,
		    //    this function recalculates that pixel's "ColorFrequencies" field.
		    void RecalculatePixelChances(Vector2i pixelPos);
	    };
    }
}