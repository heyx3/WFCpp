#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"

namespace WFC
{
    namespace Tiled3D
    {
        //A collection of tiles, by their ID.
        using TileIDSet = Set<TileID, std::hash<TileID>>;

        //Holds all the tiles that can be used by the algorithm.
        //Provides various queries for them.
        class WFC_API InputData
        {
        public:

            InputData(const List<Tile>& tiles);
            InputData() : InputData(List<Tile>()) { }

            inline const List<Tile>& GetTiles() const { return tiles; }
            inline const Tile& GetTile(TileID id) const { return tiles[id]; }
            inline const TileIDSet& GetTilesWithFace(const FacePermutation& face) const
            {
                return matchingFaces.Get(face, EmptyTileSet);
            }

        private:

            List<Tile> tiles;

            //A quick lookup of which tiles have a certain face type in a certain direction.
            Dictionary<FacePermutation, TileIDSet> matchingFaces;

            //The default response for "GetTilesWithEdge()".
            static const TileIDSet EmptyTileSet;
        };
    }
}