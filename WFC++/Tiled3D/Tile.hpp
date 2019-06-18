#pragma once

#include "../HelperClasses.h"
#include "Transform3D.h"

namespace WFC
{
    namespace Tiled3D
    {
        //Unique identifiers for each tile cube.
        //Note that this ID changes under transformation.
        using TileID = uint_fast32_t;
        const TileID TileID_INVALID = std::numeric_limits<TileID>().max();

        //A single item in a tileset.
        struct WFC_API Tile
        {
            //The faces of this tile, indexed by the Directions3D enum.
            FacePermutation Faces[6];

            //The "weight" of this tile.
            //"Heavier" tiles are more likely to be chosen.
            //The default weight is chosen by convention to be 100
            //    so that there's plenty of room to be given a weight under the default.
            uint32_t Weight = 100;
        };
    }
}