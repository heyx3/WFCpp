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
            //All the transformations that would leave this tile unchanged.
            List<Transform3D> Symmetries;

            //The faces of this tile, indexed by the Directions3D enum.
            FacePermutation Faces[6];
        };
    }
}