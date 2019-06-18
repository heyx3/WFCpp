#pragma once

#include "InputData.h"

namespace WFC
{
    namespace Tiled3D
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
                //Whether this tile can have its value changed
                //    (e.x. when the algorithm is in an impossible state and needs to back up).
                bool IsClearable = true;

                bool IsSet() const { return Value.HasValue; }
            };


            const InputData& Input;
            Array3D<OutputTile> Output;

            //Whether the output must wrap along each axis.
            bool IsPeriodicX, IsPeriodicY, IsPeriodicZ;

            //If an area with no possible tiles is created,
            //    the area surrounding that problem will be cleared out and regenerated.
            //The area is a cuboid centered on the bad tile,
            //    whose length along each axis is given in tiles (e.x. {0,1,2} means a 1x3x5 box).
            //If this is set to {0,0,0}, this generator just fails intead of clearing space.
            Vector3i ClearSize;


            State(const InputData& input, Vector3i outputSize,
                  unsigned int seed, bool periodicX, bool periodicY, bool periodicZ,
                  Vector3i clearSize)
                : Input(input), Output(outputSize), rng(seed),
                  IsPeriodicX(periodicX), IsPeriodicY(periodicY), IsPeriodicZ(periodicZ),
                  ClearSize(clearSize)
            {
                Reset(outputSize);
            }

            void Reset(Vector3i newOutputSize);


            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            Nullable<bool> Iterate(List<Vector3i>& out_failedAt) { Vector3i _; return Iterate(_, out_failedAt); }
            //Runs one iteration. Returns true (success), false (failure), or null (not done yet).
            //If the algorithm failed, "out_failedAt" will contain
            //    the positions that the algorithm failed at.
            //After running, "out_changedPosition" contains the coordinate of the tile that was changed,
            //    assuming the algorithm didn't fail.
            Nullable<bool> Iterate(Vector3i& out_changedPos, List<Vector3i>& out_failedAt);

            //Runs this algorithm to either completion or failure.
            //Returns whether it succeeded.
            bool RunToEnd(List<Vector3i>& out_failedAt);


            //Applies wrapping to each axis as applicable.
            inline Vector3i FilterPos(Vector3i pos) const
            {
                if (IsPeriodicX)
                    pos.x = WFC::Math::PositiveModulo(pos.x, Output.GetWidth());
                if (IsPeriodicY)
                    pos.y = WFC::Math::PositiveModulo(pos.y, Output.GetHeight());
                if (IsPeriodicZ)
                    pos.z = WFC::Math::PositiveModulo(pos.z, Output.GetDepth());
                return pos;
            }
            //Gets whether the given tile position is a valid position in the output array,
            //    taking wrapping into account.
            inline bool IsValidPos(const Vector3i& tilePos) const
            {
                return (IsPeriodicX | ((tilePos.x >= 0) & (tilePos.x < Output.GetWidth()))) &
                       (IsPeriodicY | ((tilePos.y >= 0) & (tilePos.y < Output.GetHeight()))) &
                       (IsPeriodicZ | ((tilePos.z >= 0) & (tilePos.z < Output.GetDepth())));
            }

            //Sets the given space to use the given tile.
            //Automatically recalculates probabilities for any neighboring tiles.
            //If "permanent" is true, this tile can never be changed by the algorithm afterwards.
            void SetTile(Vector3i tile, TileID value, bool permanent = false);
            //Clears the given area based on a center and the "clear size".
            //Automatically recalculates probabilities for any neighboring tiles.
            void ClearArea(Vector3i center, Vector3i clearSize);

            //Gets all output tiles that aren't set yet but have extremely few states.
            void GetBestTiles(List<Vector3i>& outPositions) const;


        private:

            TileIDSet allTileIDs;
            PRNG rng;

            //Temp collections that are used inside the algorithm.
            //Allocated once to improve performance.
            TileIDSet tempTileIDSet;
            List<TileID> tempTileIDList;
            List<uint_fast32_t> tempUintList;
            Set<Vector3i> tempPosSet;

            //If the given output tile is unset,
            //    this function recalculates that space's "PossibleTiles" field.
            void RecalculateTileChances(Vector3i tilePos);
        };
    }
}