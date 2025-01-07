#include "Tiled/InputData.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled;


namespace
{
    template<typename I>
    I getMax(I a, I b) { return (a > b) ? a : b; }
}

const TileIDSet InputData::EmptyTileSet;


InputData::InputData(const std::vector<Tile>& _tiles)
    : tiles(_tiles)

{
    //Collect all tiles that fit each type of edge.
    for (TileID tileID = 0; tileID < (TileID)tiles.size(); ++tileID)
    {
        const auto& tile = tiles[tileID];
        for (uint8_t edgeI = 0; edgeI < 4; ++edgeI)
        {
            auto key = EdgeInstance(tile.Edges[edgeI], (EdgeDirs)edgeI);
            matchingEdges[key].insert(tileID);
        }
    }
}