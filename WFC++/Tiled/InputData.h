#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"


namespace WFC
{
    namespace Tiled
    {
        //A collection of tiles, by their ID.
        using TileIDSet = Set<TileID, std::hash<TileID>>;


	    //Tile data for the WFC algorithm to generate from.
        //Pre-computes and caches important queries for the algorithm.
	    class WFC_API InputData
	    {
	    public:
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


		    InputData(const std::vector<Tile>& tiles);
            InputData() : InputData(std::vector<Tile>()) { }


            inline const std::vector<Tile>& GetTiles() const { return tiles; }

            //Gets all tiles that have the given edge type on the given side.
            inline const TileIDSet& GetTilesWithEdge(EdgeID type, EdgeDirs side) const
            {
                return matchingEdges.Get(EdgeInstance(type, side),
                                         EmptyTileSet);
            }

	    private:

            //All the tiles in the input data, including rotated/reflected permutations.
            std::vector<Tile> tiles;

            //A quick lookup of which tiles have a certain edge type in a certain direction.
            Dictionary<EdgeInstance, TileIDSet> matchingEdges;


            //The default response for "GetTilesWithEdge()".
            static const TileIDSet EmptyTileSet;
	    };
    }
}