#pragma once

#include "../HelperClasses.h"
#include "Transform3D.h"

namespace WFC
{
    namespace Tiled3D
    {
        //Unique identifiers for each tile permutation.
        //TODO: This is way too close to 'TileIdx', rename one of the two and look for bugs from conflation.
        using TileID = uint_fast32_t;
        constexpr TileID TileID_INVALID = std::numeric_limits<TileID>::max(),
                         TileID_FIRST_VALID = 0;

        //A single item in a tileset.
        struct WFC_API Tile
        {
            WFCPP_MEMORY_CHECK_HEADER(16, "Tile struct");
            
            //Information about the faces of this tile.
            CubePermutation Data;

            //The allowed permutations of this tile.
            TransformSet Permutations = TransformSet::Combine(Transform3D());

            //The "weight" of this tile.
            //"Heavier" tiles are more likely to be chosen.
            //The default weight is chosen by convention to be 100
            //    so that there's plenty of room to be given a weight under the default.
            uint32_t Weight = 100;

            WFCPP_MEMORY_CHECK_FOOTER(16, "Tile struct");
            inline void DEBUGMEM_ValidateAll()
            {
                DEBUGMEM_Validate();
                Data.DEBUGMEM_ValidateAll();
                Permutations.DEBUGMEM_Validate();
            }
        };
    }
}