#pragma once

#include "../HelperClasses.h"

namespace WFC
{
    namespace Tiled
    {
        using TileID = uint16_t;
        using EdgeID = uint16_t;

        enum WFC_API Edges : uint8_t
        {
            MinX = 0,
            MaxX = 1,
            MinY = 2,
            MaxY = 3
        };

        //Gets the direction the given edge points in.
        inline Vector2i GetEdgeDirection(Edges e)
        {
            switch (e)
            {
                default: assert(false);

                case Edges::MaxX: return Vector2i(1, 0);
                case Edges::MaxY: return Vector2i(0, 1);

                case Edges::MinX: return Vector2i(-1, 0);
                case Edges::MinY: return Vector2i(0, -1);
            }
        }
        //Gets the edge on the other side of the given one.
        inline Edges GetOppositeEdge(Edges e)
        {
            switch (e)
            {
                default: assert(false);

                case Edges::MinX: return Edges::MaxX;
                case Edges::MaxX: return Edges::MinX;
                case Edges::MinY: return Edges::MaxY;
                case Edges::MaxY: return Edges::MinY;
            }
        }


        //A single item in a tilemap.
        struct WFC_API Tile
        {
        public:

            //The unique identifier for this tile.
            TileID ID;

            //The ID's for each edge.
            //Used to determine whether this tile can fit alongside another one.
            EdgeID Edges[4];

            //How often this tile should generally appear, relative to the other tiles.
            float Weight = 1;

            //If this tile is a permutation of another one, these fields describe the relationship.
            TileID ParentID = std::numeric_limits<TileID>().max();
            Transformations ParentToMeTransform = Transformations::None;


            bool IsChild() const { return ParentID != std::numeric_limits<TileID>().max(); }

            EdgeID MinX() const { return Edges[Edges::MinX]; }
            EdgeID MaxX() const { return Edges[Edges::MaxX]; }
            EdgeID MinY() const { return Edges[Edges::MinY]; }
            EdgeID MaxY() const { return Edges[Edges::MaxY]; }

            EdgeID& MinX() { return Edges[Edges::MinX]; }
            EdgeID& MaxX() { return Edges[Edges::MaxX]; }
            EdgeID& MinY() { return Edges[Edges::MinY]; }
            EdgeID& MaxY() { return Edges[Edges::MaxY]; }

            
            //Hashes an instance.
            //Allows this class to be used as a Dictionary<> key or Set<> element.
            inline unsigned int operator()(const Tile& t) { return ID; }
            inline bool operator==(const Tile& t2) const { return ID == t2.ID; }
            inline bool operator!=(const Tile& t2) const { return !operator==(t2); }
        };
    }
}