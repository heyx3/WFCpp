#include "InputData.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled3D;


namespace
{
    template<typename I>
    I GetMax(I a, I b) { return (a > b) ? a : b; }
}

const TileIDSet InputData::EmptyTileSet;


InputData::InputData(const List<Tile>& _tiles)
    : tiles(_tiles)
{
    //Collect all tiles that fit each type of face.
    for (TileID tileID = 0; tileID < (TileID)tiles.GetSize(); ++tileID)
    {
        const auto& tile = tiles[tileID];
        for (uint_fast8_t faceI = 0; faceI < 6; ++faceI)
        {
            auto key = tile.Data.Faces[faceI];
            matchingFaces[key].Add(tileID);
        }
    }
}