#include "InputData.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled;


namespace
{
    template<typename I>
    I getMax(I a, I b) { return (a > b) ? a : b; }
}


InputData::InputData(const List<Tile>& originalTiles,
                     bool useRotations, bool useReflections,
                     ErrorCodes& outErrorCode,
                     const Dictionary<EdgeID, bool>* isEdgeSymmetrical)
{
    //Get the smallest ID that has not been used yet, for tile permutations.
    TileID nextTileID = 0;
    for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        nextTileID = getMax(nextTileID, originalTiles[i].ID);
    nextTileID += 1;

    //Do the same for edge IDs.
    EdgeID nextEdgeID = 0;
    for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        for (int e = 0; e < 4; ++e)
            nextEdgeID = getMax(nextEdgeID, originalTiles[i].Edges[e]);
    nextEdgeID += 1;

    //Create any requested permutations of tiles.
    auto tiles = List<Tile>(originalTiles);

    //Macro helpers:
    #define TRANSFORM_EDGE(srcEdge, destEdge) \
        tile2.Edges[Edges::destEdge] = tile.Edges[Edges::srcEdge]
    #define MAKE_TILE(transform, minXTo, minYTo, maxXTo, maxYTo) \
        tile2.ID = nextTileID; \
        nextTileID += 1; \
        tile2.ParentToMeTransform = Transformations::transform; \
        TRANSFORM_EDGE(MinX, minXTo); \
        TRANSFORM_EDGE(MinY, minYTo); \
        TRANSFORM_EDGE(MaxX, maxXTo); \
        TRANSFORM_EDGE(MaxY, maxYTo); \
        tiles.PushBack(tile2)

    //Rotated tiles:
    if (useRotations)
    {
        for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        {
            const auto& tile = originalTiles[i];

            Tile tile2;
            tile2.ParentID = tile.ID;
            MAKE_TILE(Rotate90CW, MinY, MaxX, MaxY, MinX);
            MAKE_TILE(Rotate180, MaxX, MaxY, MinX, MinY);
            MAKE_TILE(Rotate270CW, MaxY, MinX, MinY, MaxX);
        }
    }
    //Reflected tiles:
    if (useReflections)
    {
        if (isEdgeSymmetrical == nullptr)
        {
            outErrorCode = ErrorCodes::MissingSymmetricalDict;
            return;
        }

        for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        {
            //Get a copy of the source tile.
            auto tile = originalTiles[i];

            //For each edge, if the edge is asymmetrical, then think of it as a new edge.
            for (int edgeI = 0; edgeI < 4; ++edgeI)
            {
                EdgeID edgeType = tile.Edges[edgeI];
                
                const bool* isSymmetrical = isEdgeSymmetrical->TryGet(edgeType);
                if (isSymmetrical == nullptr)
                {
                    outErrorCode = ErrorCodes::MissingEdgeID;
                    return;
                }
                else if (!(*isSymmetrical))
                {
                    tile.Edges[edgeI] = nextEdgeID;
                    nextEdgeID += 1;
                }
            }

            Tile tile2;
            tile2.ParentID = tile.ID;
            MAKE_TILE(MirrorX, MaxX, MinY, MinX, MaxY);
            MAKE_TILE(MirrorY, MinX, MaxY, MaxX, MinY);
        }
    }

    //Now match every tile's edge to a set of tiles that can line up with it.
    //TODO: Implement.
}