#pragma once

#include <UnitTest++.h>

#include <Simple/State.h>
#include <Tiled/State.h>
#include <Tiled3D/StandardRunner.h>
#include <WFCppStreamPrinting.hpp>


namespace Tilesets
{
    namespace Simple
    {
        using namespace WFC;
        using namespace WFC::Simple;

        //TODO: Make Simple tests.
    }
    
    namespace Tiled
    {
        using namespace WFC;
        using namespace WFC::Tiled;

        //TODO: Make Tiled tests.
    }

    namespace Tiled3D
    {
        using namespace WFC;
        using namespace WFC::Tiled3D;

        //A single tile, whose opposite faces match up with various symmetry patterns:
        //   * X faces have major-diagonal symmetry
        //         (you can flip the face along its main diagonal).
        //   * Y faces have no symmetries (each point has a different ID).
        //   * Z faces have total symmetry (you can use any transformation).
        template<typename... Transforms>
        inline List<Tile> OneTileArmy(Transforms... transforms)
        {
            auto permutations = TransformSet::Combine(transforms...);
            List<Tile> output;
            auto& tile = output.PushBack(WFC::Tiled3D::Tile{ });

            //Remember point ordering: AA, AB, BA, BB
            tile.Data.Faces[0] = { MinX, { 0, 1, 1, 2 } };
            tile.Data.Faces[1] = { MaxX, { 0, 1, 1, 2 } };
            tile.Data.Faces[2] = { MinY, { 4, 5, 6, 7 } };
            tile.Data.Faces[3] = { MaxY, { 4, 5, 6, 7 } };
            tile.Data.Faces[4] = { MinZ, { 3, 3, 3, 3 } };
            tile.Data.Faces[5] = { MaxZ, { 3, 3, 3, 3 } };
            tile.Permutations = permutations;
            tile.Weight = 100;

            return output;
        }

        //TODO: 2 tiles that line up with each other in strange face pairings

        //TODO: More Tiled3D tests.
    }
}