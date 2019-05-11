#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"


namespace WFC
{
    namespace Tiled
    {
        using EdgeIDSet = Set<EdgeID, std::hash<EdgeID>>;
        using TileIDSet = Set<TileID, std::hash<TileID>>;
        using EdgeToEdgeMap = Dictionary<EdgeID, EdgeID, std::hash<EdgeID>>;


	    //Tile data for the WFC algorithm to generate from.
	    class WFC_API InputData
	    {
	    public:

            enum WFC_API ErrorCodes
            {
                //Everything went fine!
                NoError = 0,
                //One or more parameters needed for reflecting tiles does not exist.
                MissingReflectionData,
                //The "edgeToReflectedEdge" parameter has invalid values.
                InvalidReflectionMap,
            };

            //A specific edge type and direction.
            struct EdgeInstance
            {
                EdgeID Type;
                EdgeDirs Dir;

                EdgeInstance() : Type(0), Dir((EdgeDirs)0) { }
                EdgeInstance(EdgeID type, EdgeDirs connectionDir)
                    : Type(type), Dir(connectionDir) { }

                //Gets the hash value for an instance.
                //Allows it to be used as a Dictionary<> key.
                inline unsigned int operator()(const EdgeInstance& t) const
                {
                    //Use the Vector2i hasher that already exists.
                    Vector2i v2(t.Type, t.Dir);
                    return Vector2i()(v2);
                }

                inline bool operator==(const EdgeInstance& t2) const
                {
                    return (Type == t2.Type) & (Dir == t2.Dir);
                }
                inline bool operator!=(const EdgeInstance& t2) const { return !(operator==(t2)); }
            };


            //Creates a new set of input data,
            //    potentially including rotated/reflected permutations of tiles.
            //The "symmetricalEdges" and "edgeToReflectedEdge" data parameters are only needed
            //    if reflections are enabled.
		    InputData(const List<Tile>& tiles,
                      bool useRotations, bool useReflections,
                      ErrorCodes& outErrorCode,
                      const EdgeIDSet* symmetricalEdges = nullptr,
                      const EdgeToEdgeMap* edgeToReflectedEdge = nullptr);


            inline const List<Tile>& GetTiles() const { return tiles; }

            //Gets all tiles that have the given edge type on the given side.
            inline const TileIDSet& GetTilesWithEdge(EdgeID type, EdgeDirs side) const
            {
                return matchingEdges.Get(EdgeInstance(type, side),
                                         EmptyTileSet);
            }

            //Gets the tile with the given ID.
            inline const Tile* GetTile(TileID id) const
            {
                assert(tileIndices.Contains(id));
                return &tiles[tileIndices[id]];
            }
            //Gets the given permutation of the given tile, generated in this class's constructor.
            //Returns null if the permutation doesn't exist.
            inline const Tile* GetPermutation(TileID parent, Transformations transform) const
            {
                assert(tilePermutations.Contains(parent));
                assert(tileIndices.Contains(parent));

                if (transform == WFC::Transformations::None)
                    return GetTile(parent);

                for (auto childID : tilePermutations[parent])
                {
                    assert(tileIndices.Contains(parent));
                    const auto* childTile = GetTile(childID);

                    if ((childTile->ParentID == parent) & (childTile->ParentToMeTransform == transform))
                        return childTile;
                }

                return nullptr;
            }
            
            //Gets the reflected version of the given edge.
            inline EdgeID GetReflectedEdge(EdgeID e) const
            {
                //Remember: if it's not in the lookup, then it is symmetrical.
                return reflectedEdgeMap.Get(e, e);
            }


	    private:

            //TODO: Refactor input permutations/lookups into a separate class.

            //All the tiles in the input data, including rotated/reflected permutations.
            List<Tile> tiles;

            //A quick lookup of a tile's index in the "tiles" list.
            Dictionary<TileID, size_t, std::hash<TileID>> tileIndices;

            //A quick lookup of which tiles have a certain edge type in a certain direction.
            Dictionary<EdgeInstance, TileIDSet> matchingEdges;

            //A quick lookup of a tile's children.
            Dictionary<TileID, List<TileID>, std::hash<TileID>> tilePermutations;

            //Maps each edge to the reflected version of itself.
            //Symmetrical edges are not stored.
            EdgeToEdgeMap reflectedEdgeMap;


            //The default response for "GetTilesWithEdge()".
            static const TileIDSet EmptyTileSet;
	    };
    }
}