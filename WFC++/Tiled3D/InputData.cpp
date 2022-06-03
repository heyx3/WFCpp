#include "InputData.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled3D;


const TileIDSet InputData::EmptyTileSet;


InputData::InputData(const List<Tile>& _tiles)
    : tiles(_tiles)
{
    //Collect all tiles that fit each type of face.
    for (TileID tileID = 0; tileID < (TileID)tiles.GetSize(); ++tileID)
    {
        const auto& tile = tiles[tileID];
        for (uint_fast8_t faceI = 0; faceI < N_DIRECTIONS_3D; ++faceI)
        {
            const auto& faceData = tile.Data.Faces[faceI];
            matchingFaces[faceData].Add(tileID);
        }
    }
}