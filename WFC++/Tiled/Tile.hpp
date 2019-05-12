#pragma once

#include "../HelperClasses.h"

namespace WFC
{
    namespace Tiled
    {
        //A tile's index in a list that holds it.
        using TileID = uint_fast32_t;
        const TileID TileID_INVALID = std::numeric_limits<TileID>().max();
        
        //A unique identifier for an edge. Two edges with the same ID can connect to each other.
        using EdgeID = uint16_t;
        const EdgeID EdgeID_INVALID = std::numeric_limits<EdgeID>().max();


        enum WFC_API EdgeDirs : uint8_t
        {
            MinX = 0,
            MaxX = 1,
            MinY = 2,
            MaxY = 3
        };

        //Gets the direction the given edge points in.
        inline Vector2i GetEdgeDirection(EdgeDirs e)
        {
            switch (e)
            {
                default: assert(false);

                case EdgeDirs::MaxX: return Vector2i(1, 0);
                case EdgeDirs::MaxY: return Vector2i(0, 1);

                case EdgeDirs::MinX: return Vector2i(-1, 0);
                case EdgeDirs::MinY: return Vector2i(0, -1);
            }
        }
        //Gets the edge on the other side of the given one.
        inline EdgeDirs GetOppositeEdge(EdgeDirs e)
        {
            switch (e)
            {
                default: assert(false);

                case EdgeDirs::MinX: return EdgeDirs::MaxX;
                case EdgeDirs::MaxX: return EdgeDirs::MinX;
                case EdgeDirs::MinY: return EdgeDirs::MaxY;
                case EdgeDirs::MaxY: return EdgeDirs::MinY;
            }
        }


        //A single item in a tilemap.
        struct WFC_API Tile
        {
        public:

            //Any transformations that would not change this tile at all.
            TransformationFlags Symmetries;

            //The ID's for each edge.
            //Used to determine whether this tile can fit alongside another one.
            EdgeID Edges[4];

            //How often this tile should generally appear.
            //Used as a weight for the random number generator,
            //    so higher values means more appearances.
            //The default weight is set at 100 so that there's plenty of room
            //    to be given a weight under the default.
            uint32_t Weight = 100;

            //If this tile is a permutation of another one, these fields describe the relationship.
            TileID ParentID = TileID_INVALID;
            Transformations ParentToMeTransform = Transformations::None;


            bool IsChild() const { return ParentID != TileID_INVALID; }

            EdgeID MinX() const { return Edges[EdgeDirs::MinX]; }
            EdgeID MaxX() const { return Edges[EdgeDirs::MaxX]; }
            EdgeID MinY() const { return Edges[EdgeDirs::MinY]; }
            EdgeID MaxY() const { return Edges[EdgeDirs::MaxY]; }

            EdgeID& MinX() { return Edges[EdgeDirs::MinX]; }
            EdgeID& MaxX() { return Edges[EdgeDirs::MaxX]; }
            EdgeID& MinY() { return Edges[EdgeDirs::MinY]; }
            EdgeID& MaxY() { return Edges[EdgeDirs::MaxY]; }
        };
    }
}