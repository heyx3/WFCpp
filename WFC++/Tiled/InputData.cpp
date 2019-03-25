#include "InputData.h"

#include <algorithm>

using namespace WFC;
using namespace WFC::Tiled;


namespace
{
    template<typename I>
    I getMax(I a, I b) { return (a > b) ? a : b; }
}

const TileIDSet InputData::EmptyTileSet;


InputData::InputData(const List<Tile>& originalTiles,
                     bool useRotations, bool useReflections,
                     ErrorCodes& outErrorCode,
                     const EdgeIDSet* symmetricalEdges,
                     const Dictionary<EdgeID, EdgeID>* edgeToReflectedEdge)
{
    //If we were given some edge reflections, copy them over.
    if (edgeToReflectedEdge != nullptr)
    {
        reflectedEdgeMap = *edgeToReflectedEdge;

        //Make sure they're stored in the other direction too.
        for (const auto& kvp : reflectedEdgeMap)
        {
            if (reflectedEdgeMap.Contains(kvp.second))
            {
                //Double-check that the pair is stored correctly.
                if (reflectedEdgeMap[kvp.second] != kvp.first)
                {
                    outErrorCode = ErrorCodes::InvalidReflectionMap;
                    return;
                }
            }
            else
            {
                reflectedEdgeMap[kvp.second] = kvp.first;
            }
        }
    }

    //Get the smallest ID that has not been used yet, for tile permutations.
    TileID nextTileID = 0;
    for (size_t i = 0; i < originalTiles.GetSize(); ++i)
    {
        nextTileID = getMax(nextTileID, originalTiles[i].ID);
        tileIndices[originalTiles[i].ID] = i;
    }
    nextTileID += 1;

    //Do the same for edge IDs.
    EdgeID nextEdgeID = 0;
    for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        for (int e = 0; e < 4; ++e)
            nextEdgeID = getMax(nextEdgeID, originalTiles[i].Edges[e]);
    nextEdgeID += 1;

    //Create any requested permutations of tiles.
    tiles.Insert(0, originalTiles);

    //Macro helpers:
    #define TRANSFORM_EDGE(srcEdge, destEdge) \
        tile2.Edges[EdgeDirs::destEdge] = tile.Edges[EdgeDirs::srcEdge]
    #define MAKE_TILE(transform, minXTo, minYTo, maxXTo, maxYTo) { \
        Tile tile2; \
        tile2.ParentID = tile.ID; \
        tile2.Weight = tile.Weight; \
        tile2.ID = nextTileID; \
        nextTileID += 1; \
        tile2.ParentToMeTransform = Transformations::transform; \
        TRANSFORM_EDGE(MinX, minXTo); \
        TRANSFORM_EDGE(MinY, minYTo); \
        TRANSFORM_EDGE(MaxX, maxXTo); \
        TRANSFORM_EDGE(MaxY, maxYTo); \
        tilePermutations[tile.ID].PushBack(tile2.ID); \
        tileIndices[tile2.ID] = tiles.GetSize(); \
        tiles.PushBack(tile2); \
    }

    //Rotated tiles:
    if (useRotations)
    {
        for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        {
            const auto& tile = originalTiles[i];

            MAKE_TILE(Rotate90CW, MinY, MaxX, MaxY, MinX);
            MAKE_TILE(Rotate180, MaxX, MaxY, MinX, MinY);
            MAKE_TILE(Rotate270CW, MaxY, MinX, MinY, MaxX);
        }
    }
    //Reflected tiles:
    if (useReflections)
    {
        if (symmetricalEdges == nullptr || edgeToReflectedEdge == nullptr)
        {
            outErrorCode = ErrorCodes::MissingReflectionData;
            return;
        }

        for (size_t i = 0; i < originalTiles.GetSize(); ++i)
        {
            //Get a copy of the source tile.
            auto tile = originalTiles[i];

            //For each edge in the original tile, change it to a reflected version.
            //Note that for BOTH types of reflection (X and Y), all 4 edges will become reflected.
            for (int edgeI = 0; edgeI < 4; ++edgeI)
            {
                EdgeID oldEdgeType = tile.Edges[edgeI];
                if (!symmetricalEdges->Contains(oldEdgeType))
                {
                    //If a reflected version of this edge already exists, use it.
                    if (reflectedEdgeMap.Contains(oldEdgeType))
                        tile.Edges[edgeI] = reflectedEdgeMap[oldEdgeType];
                    //Otherwise, create a new edge type for this.
                    else
                    {
                        auto newEdgeType = nextEdgeID;
                        nextEdgeID += 1;

                        tile.Edges[edgeI] = newEdgeType;
                        reflectedEdgeMap[oldEdgeType] = newEdgeType;
                        reflectedEdgeMap[newEdgeType] = oldEdgeType;
                    }
                }
            }

            MAKE_TILE(MirrorX, MaxX, MinY, MinX, MaxY);
            MAKE_TILE(MirrorY, MinX, MaxY, MaxX, MinY);
        }
    }

    //TODO: Rotation+Reflection tiles? At least SOME of them are new transformations in terms of edges, right? Maybe provide an "ignoreRotateReflect" parameter. However, this feature would complicate GetPermutation().

    //Collect all tiles that fit each type of edge.
    for (const auto& tile : tiles)
    {
        for (uint8_t edgeI = 0; edgeI < 4; ++edgeI)
        {
            auto key = EdgeInstance(tile.Edges[edgeI], (EdgeDirs)edgeI);
            matchingEdges[key].Add(tile.ID);
        }
    }
}