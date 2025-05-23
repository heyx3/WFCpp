#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"

namespace WFC
{
    namespace Tiled
    {
        //Maps an edge to its reflected version.
        using EdgeReflectionMap = std::unordered_map<EdgeID, EdgeID>;

        //Generates permutations of tiles,
        //    and tracks which tiles are "children" of an original one.
        class WFC_API TilePermutator
        {
        public:

            //Error codes for this class's constructor.
            enum WFC_API ErrorCodes
            {
                //Everything went fine!
                NoError = 0,
                //The "reflectedEdges" parameter has invalid values.
                InvalidReflectionMap,
            };


            //Creates a new instance, including the permutations of the given root tiles.
            //Note that this instance takes ownership over the original root tiles.
            //The "reflectedEdges" parameter is only helpful if reflections are used,
            //    but isn't required (if none of your tiles' edges are already reflections of each other).
            TilePermutator(const std::vector<Tile>& originalTiles,
                           TransformationFlags permutations,
                           ErrorCodes& outErrorCode,
                           const EdgeReflectionMap* reflectedEdges = nullptr);
            //Creates an empty TilePermutator.
            TilePermutator();


            //Gets the number of original tiles that permutations were made from.
            //The first tiles in the "GetAllTiles()" are all these original tiles.
            inline const size_t GetNOriginalTiles() const { return nOriginalTiles; }
            //Gets the number of total tiles, including originals and permutations.
            inline const size_t GetNAllTiles() const { return tiles.size(); }

            //Gets all tiles, including originals and permutations.
            //The originals are at the front of the list.
            inline const std::vector<Tile>& GetTiles() const { return tiles; }
            //Looks up the tile in the tile list.
            inline const Tile& GetTile(TileID id) const { return tiles[id]; }

            //Gets whether the given tile is one of the originals that permutations were made from.
            inline bool IsTileRoot(TileID tile) const { return GetTileRoot(tile) == tile; }
            //Gets the "parent" tile, that the given tile is a permutation of.
            inline TileID GetTileRoot(TileID tile) const { return tileParents[tile].ParentID; }

            //Gets the specific permutation that this tile is.
            //If it's one of the originals, the permutation will be "None".
            inline Transformations GetMyPermutation(TileID tile) const { return tileParents[tile].MyTransform; }

            //Given a root tile, gets all permutations that came from it.
            //Note that the return value includes the original, as the "None" permutation.
            inline const std::vector<TileID>& GetPermutations(TileID original) const { return tilePermutations.at(original); }
            //Given a root tile and a permutation, finds that "child" tile.
            const TileID GetTileChild(TileID original, Transformations permutation) const;

            //Assuming that at least one reflection permutation was used,
            //    this function maps the given edge to its reflected version.
            inline const EdgeID GetReflectedEdge(EdgeID e) const { return edgeReflections.at(e); }
            

        private:

            std::vector<Tile> tiles;
            size_t nOriginalTiles;

            struct ParentData
            {
                TileID ParentID;
                Transformations MyTransform;
            };
            std::vector<ParentData> tileParents;

            std::unordered_map<TileID, std::vector<TileID>> tilePermutations;

            EdgeReflectionMap edgeReflections;
        };
    }
}