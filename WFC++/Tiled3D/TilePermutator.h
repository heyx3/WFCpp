#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"

namespace WFC
{
    namespace Tiled3D
    {
        //Generates permutations of tiles,
        //    and tracks which tiles are "children" of an original one.
        class WFC_API TilePermutator
        {
        public:

            TilePermutator();
            TilePermutator(const List<Tile>& originalTiles,
                           List<Transform3D>& permutations);

            //Gets the number of original tiles that permutations were made from.
            inline const size_t GetNOriginalTiles() const { return nOriginalTiles; }
            //Gets the number of total tiles, including originals and permutations.
            inline const size_t GetNAllTiles() const { return tiles.GetSize(); }

            //Gets all tiles, including originals and permutations.
            //The originals are all at the front of the list.
            inline const List<Tile>& GetTiles() const { return tiles; }
            //Gets the tile with the given ID.
            inline const Tile& GetTile(TileID id) const { return tiles[id]; }

            //Gets whether the given tile is one of the originals that permutations were made from.
            inline bool IsAnOriginal(TileID tile) const { return FindOriginal(tile) == tile; }
            //Gets whether the given tile is a permutation of an "original" tile.
            inline bool IsAPermutation(TileID tile) const { return !IsAnOriginal(tile); }

            //Finds the original tile that the given one is a permutation of.
            inline TileID FindOriginal(TileID permutation) const { return tileParents[permutation].ParentID; }
            //Finds the given permutation of the given original tile.
            TileID FindPermutation(TileID original, Transform3D permutation) const;

            //Gets the specific permutation that the given tile is.
            inline Transform3D GetMyPermutation(TileID tile) const { return tileParents[tile].MyTransform; }

            //Given an original tile, gets all permutations that came from it.
            //Note that the return value includes the original un-transformed tile.
            inline const List<TileID>& GetMyPermutations(TileID original) const { return *permutationsPerParent.TryGet(original); }


        private:

            List<Tile> tiles;
            size_t nOriginalTiles;

            struct ParentData
            {
                TileID ParentID;
                Transform3D MyTransform;
            };
            List<ParentData> tileParents;

            Dictionary<TileID, List<TileID>, std::hash<TileID>> permutationsPerParent;
        };
    }
}