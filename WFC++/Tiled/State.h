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
                //The different tiles this one might become.
                //If a tile has been chosen, this set will only have one element.
                TileIDSet PossibleTiles;

                bool IsSet() const { return Value.HasValue; }
		    };


		    const InputData& Input;
		    Array2D<OutputTile> Output;

		    //Whether the output must wrap along each axis.
		    bool PeriodicX, PeriodicY;

		    //If an area with no possible tiles is created,
		    //    the area surrounding that problem will be cleared out and regenerated.
		    //The area is a square whose radius is given in tiles
            //    (e.x. 1 is a 3x3 box, 2 is a 5x5, etc).
		    //If this is set to 0, this generator just fails instead of clearing space.
		    size_t ClearSize;


		    State(const InputData& input, Vector2i outputSize,
			      unsigned int seed, bool periodicX, bool periodicY, size_t clearSize)
			    : Input(input), Output(outputSize), rng(seed),
                  PeriodicX(periodicX), PeriodicY(periodicY), ClearSize(clearSize)
		    {
			    Reset(outputSize);
		    }


            //Applies wrapping to each axis if applicable.
		    inline Vector2i Filter(const Vector2i& pos) const
		    {
                return Vector2i(PeriodicX ? Wrap(pos.x, Output.GetWidth()) : pos.x,
                                PeriodicY ? Wrap(pos.y, Output.GetHeight()) : pos.y);
		    }
            //Gets whether the given tile position is a valid output position
            //    (after taking wrapping into account).
            inline bool IsValidPos(Vector2i tilePos) const
            {
                return (PeriodicX | ((tilePos.x >= 0) & (tilePos.y >= 0))) &
                    (PeriodicY | ((tilePos.x < Output.GetWidth()) &
                    (tilePos.y < Output.GetHeight())));
            }

            //Gets the output tile at the given position (after taking wrapping into account).
            //Returns nullptr if the position is invalid.
		    inline const OutputTile* operator[](Vector2i pos) const
		    {
                return IsValidPos(pos) ? &Output[Filter(pos)] : nullptr;
		    }
            //Gets the output tile at the given position (after taking wrapping into account).
            //Returns nullptr if the position is invalid.
		    inline OutputTile* operator[](Vector2i pos)
		    {
                return IsValidPos(pos) ? &Output[Filter(pos)] : nullptr;
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
		    void SetTile(Vector2i tilePos, TileID value);


	    private:

		    static inline int Wrap(int val, int maxExclusive)
		    {
			    //http://stackoverflow.com/questions/3417183/modulo-of-negative-numbers
			    val %= maxExclusive;
			    return (val < 0) ? (val + maxExclusive) : val;
		    }


            TileIDSet allTileIDs, tempTileIdSet;
            PRNG rng;

            //Clears all output tiles surrounding the given pixel.
		    //The size of the area to clear is determined by the "ClearSize" field.
            //Adds the cleared tiles and any adjacent ones to "out_affectedPoses".
		    //Assumes that ClearSize is greater than 0.
            void ClearArea(Vector2i center, Set<Vector2i>& out_affectedPoses);

		    //Gets all output tiles with the fewest number of possible states.
		    //Ignores any tiles whose value is already set.
		    void GetBestTiles(List<Vector2i>& outValues) const;

		    //If the given output tile is unset,
		    //    this function recalculates that space's "ValuePossibilities" field.
		    void RecalculateTileChances(Vector2i tilePos);
	    };
    }
}