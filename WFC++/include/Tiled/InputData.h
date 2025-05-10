#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"


//The helper struct and its std::hash must be defined first.
namespace WFC
{
    namespace Tiled
    {
        //A specific edge type and direction.
        struct EdgeInstance
        {
            EdgeID Type;
            EdgeDirs Dir;

            EdgeInstance() : Type(0), Dir((EdgeDirs)0) {}
            EdgeInstance(EdgeID type, EdgeDirs connectionDir)
                : Type(type), Dir(connectionDir) {
            }

            inline bool operator==(const EdgeInstance& t2) const
            {
                return (Type == t2.Type) & (Dir == t2.Dir);
            }
            inline bool operator!=(const EdgeInstance& t2) const { return !(operator==(t2)); }
        };
    }
}
template<> struct std::hash<WFC::Tiled::EdgeInstance>
{
    size_t operator()(const WFC::Tiled::EdgeInstance& d) const {
        return WFC::Vector2i(d.Type, d.Dir).GetSTLHashcode();
    }
};

namespace WFC
{
    namespace Tiled
    {
        //A collection of tiles, by their ID.
        using TileIDSet = std::unordered_set<TileID>;


	    //Tile data for the WFC algorithm to generate from.
        //Pre-computes and caches important queries for the algorithm.
	    class WFC_API InputData
	    {
	    public:

		    InputData(const std::vector<Tile>& tiles);
            InputData() : InputData(std::vector<Tile>()) { }


            inline const std::vector<Tile>& GetTiles() const { return tiles; }

            //Gets all tiles that have the given edge type on the given side.
            inline const TileIDSet& GetTilesWithEdge(EdgeID type, EdgeDirs side) const
            {
                auto found = matchingEdges.find({ type, side });
                if (found == matchingEdges.end())
                    return EmptyTileSet;
                else
                    return found->second;
            }

	    private:

            //All the tiles in the input data, including rotated/reflected permutations.
            std::vector<Tile> tiles;

            //A quick lookup of which tiles have a certain edge type in a certain direction.
            std::unordered_map<EdgeInstance, TileIDSet> matchingEdges;


            //The default response for "GetTilesWithEdge()".
            static const TileIDSet EmptyTileSet;
	    };
    }
}