#pragma once

#include "../HelperClasses.h"
#include "InputData.h"


namespace WFC
{
    namespace Tiled
    {
	    //The state of a WFC algorithm run.
	    //This class contains the bulk of the algorithm.
	    class WFC_API State
	    {
	    public:

            //A space that may become one of several tiles,
            //    until eventually a single specific tile is chosen.
		    struct WFC_API OutputTile
		    {
		    public:
			    //The chosen tile, if it exists yet.
			    Nullable<TileID> Value;

                //The chances of this space becoming different tiles.
			    Dictionary<TileID, float> ValuePossibilities;
		    };


		    const InputData& Input;
		    Array2D<OutputTile> Output;

		    //Whether the output must wrap along each axis.
		    bool PeriodicX, PeriodicY;

		    //If an area with no matching tiles is found,
		    //    the area surrounding that problem will be cleared out and regenerated.
		    //The size of that area is given in tiles.
		    //If this is set to 0, this generator just fails instead of clearing space.
		    size_t ClearSize;


		    State(const InputData& input, Vector2i outputSize,
			      unsigned int seed, bool periodicX, bool periodicY, size_t clearSize)
			    : Input(input), Output(outputSize), rng(seed),
                  PeriodicX(periodicX), PeriodicY(periodicY), ClearSize(clearSize)
		    {
			    Reset(outputSize);
		    }


		    inline const OutputTile* operator[](Vector2i pos) const
		    {
			    Filter(pos);
			    return (Region2i(Output.GetDimensions()).Contains(pos)) ?
				           &Output[pos] :
					       nullptr;
		    }
		    inline OutputTile* operator[](Vector2i pos)
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
            //After running, "out_changedPos" contains the coordinate of the tile that was changed,
            //    assuming the algorithm didn't fail.
            Nullable<bool> Iterate(Vector2i& out_changedPos, List<Vector2i>& out_failedAt);
		
		    //Sets the given space to use the given tile.
		    //Re-calculates the status of neighboring tiles to take this into account.
		    void SetPixel(Vector2i pixelPos, TileID value);
            //Clears all output tiles surrounding the given pixel.
		    //The size of the area to clear is determined by the "ClearSize" field.
		    //Assumes that ClearSize is greater than 0.
            void ClearArea(Vector2i center);


	    private:

            PRNG rng;


		    static inline int Wrap(int val, int maxExclusive)
		    {
			    //http://stackoverflow.com/questions/3417183/modulo-of-negative-numbers
			    val %= maxExclusive;
			    return (val < 0) ? (val + maxExclusive) : val;
		    }

		    //Gets all output tiles with the fewest number of possible states.
		    //Ignores any tiles whose value is already set.
		    void GetBestTiles(List<Vector2i>& outValues) const;

		    //If the given output tile is unset,
		    //    this function recalculates that space's "ValuePossibilities" field.
		    void RecalculateTileChances(Vector2i pixelPos);
	    };
    }
}