#include "../../include/Tiled/TilePermutator.h"

using namespace WFC;
using namespace WFC::Tiled;


TilePermutator::ErrorCodes dummyErrorCodeVar;

TilePermutator::TilePermutator() : TilePermutator(std::vector<Tile>(), 0, dummyErrorCodeVar) { }

TilePermutator::TilePermutator(const std::vector<Tile>& originalTiles,
                               TransformationFlags _permutations,
                               ErrorCodes& outErrorCode,
                               const EdgeReflectionMap* reflectedEdges)
    : tiles(originalTiles), nOriginalTiles(originalTiles.size())
{
    outErrorCode = ErrorCodes::NoError;

    //Fill the original tiles' "parent" data to point to themselves.
    for (TileID parentID = 0; parentID < (TileID)originalTiles.size(); ++parentID)
    {
        tileParents.push_back(ParentData{ parentID, Transformations::None });
        tilePermutations[parentID].push_back(parentID);
    }

    //Set up the edge reflection map.
    if (reflectedEdges != nullptr)
        edgeReflections = *reflectedEdges;

    //Check through all requested permutation types.
    std::vector<Transformations> permutations;
    _permutations.Enumerate(permutations);

    //If any of the permutations is a reflection, make sure every edge has a reflected version.
    if (std::any_of(permutations.begin(), permutations.end(),
                    WFC::IsReflection))
    {
        //Verify the edges already contained in the map.
        //Also find the max ID, so that new IDs can be safely created.
        EdgeID nextEdgeID = 0;
        for (const auto& edgePair : edgeReflections)
        {
            nextEdgeID = std::max({ nextEdgeID, edgePair.first, edgePair.second });

            if (edgeReflections.contains(edgePair.second))
            {
                //Make sure both edges refer to each other.
                auto found = edgeReflections.find(edgePair.second);
                if (found == edgeReflections.end() || found->second != edgePair.first)
                {
                    outErrorCode = ErrorCodes::InvalidReflectionMap;
                    return;
                }
            }
            else
            {
                edgeReflections[edgePair.second] = edgePair.first;
            }
        }

        //Add any missing reflections to the edge map.
        nextEdgeID += 1;
        for (const auto& tile : originalTiles)
        {
            for (EdgeID edge : tile.Edges)
            {
                if (!edgeReflections.contains(edge))
                {
                    EdgeID refEdge = nextEdgeID;
                    nextEdgeID += 1;

                    edgeReflections[edge] = refEdge;
                    edgeReflections[refEdge] = edge;
                }
            }
        }
    }

    //Make each permutation.
    WFCPP_ASSERT(!_permutations.Contains(Transformations::None));
    for (Transformations tr : permutations)
    {
        for (TileID parentID = 0; parentID < (TileID)nOriginalTiles; ++parentID)
        {
            const Tile& tile = originalTiles[parentID];

            //If this tile is symmetric under this permutation, don't make a duplicate.
            if (originalTiles[parentID].Symmetries.Contains(tr))
                continue;

            //If this transformation is a reflection, reflect each edge.
            //Otherwise, leave the edges unchanged.
            EdgeID edges[4];
            for (uint_fast8_t i = 0; i < 4; ++i)
                if (IsReflection(tr))
                    edges[i] = edgeReflections[tile.Edges[i]];
                else
                    edges[i] = tile.Edges[i];

            //Create the permutation.
            Tile childTile = tile;
            switch (tr)
            {
            #define EDGES(transf, newMinX, newMaxX, newMinY, newMaxY) \
                case Transformations::transf: \
                    childTile.Edges[EdgeDirs::MinX] = edges[EdgeDirs::newMinX]; \
                    childTile.Edges[EdgeDirs::MaxX] = edges[EdgeDirs::newMaxX]; \
                    childTile.Edges[EdgeDirs::MinY] = edges[EdgeDirs::newMinY]; \
                    childTile.Edges[EdgeDirs::MaxY] = edges[EdgeDirs::newMaxY]; \
                    break
                EDGES(Rotate90CW, MaxY, MinY, MinX, MaxX);
                EDGES(Rotate180, MaxX, MinX, MaxY, MinY);
                EDGES(Rotate270CW, MinY, MaxY, MaxX, MinX);
                EDGES(FlipX, MaxX, MinX, MinY, MaxY);
                EDGES(FlipY, MinX, MaxX, MaxY, MinY);
                EDGES(FlipDiag1, MinY, MaxY, MinX, MaxX);
                EDGES(FlipDiag2, MaxY, MinY, MaxX, MinX);
            #undef EDGES

                default:
                    WFCPP_ASSERT(false);
            }

            //Register the tile.
            TileID childID = (TileID)tiles.size();
            tiles.push_back(childTile);
            tileParents.push_back(ParentData{ parentID, tr });
            tilePermutations[parentID].push_back(childID);
        }
    }
}

const TileID TilePermutator::GetTileChild(TileID original, Transformations permutation) const
{
    for (auto childID : tilePermutations.at(original))
        if (tileParents[childID].MyTransform == permutation)
            return childID;

    return TileID_INVALID;
}