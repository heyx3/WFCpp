#pragma once

#include "../HelperClasses.h"
#include "Tile.hpp"


namespace WFC
{
    namespace Tiled
    {
	    //Tile data for the WFC algorithm to generate from.
	    class WFC_API InputData
	    {
	    public:

            enum WFC_API ErrorCodes
            {
                //Everything went fine!
                NoError,
                //The dictionary indicating whether edges are symmetrical was missing.
                MissingSymmetricalDict,
                //The dictionary indicating whether edges are symmetrical was missing an edge.
                MissingEdgeID,
            };

            //A specific edge type and direction.
            struct TileConnection
            {
                EdgeID Type;
                Edges Edge;

                TileConnection(EdgeID src, Edges dest)
                    : Type(src), Edge(dest) { }

                //Gets the hash value for an instance.
                //Allows it to be used as a Dictionary<> key.
                inline unsigned int operator()(const TileConnection& t) const
                {
                    //Use the Vector2i hasher that already exists.
                    Vector2i v2(t.Type, t.Edge);
                    return Vector2i()(v2);
                }

                inline bool operator==(const TileConnection& t2) const
                {
                    return (Type == t2.Type) & (Edge == t2.Edge);
                }
                inline bool operator!=(const TileConnection& t2) const { return !(operator==(t2)); }
            };


            //Creates a new set of input data.
            //The "isEdgeSymmetrical" dictionary is only needed if reflections are enabled.
		    InputData(const List<Tile>& tiles,
                      bool useRotations, bool useReflections,
                      ErrorCodes& outErrorCode,
                      const Dictionary<EdgeID, bool>* isEdgeSymmetrical = nullptr);


            //Gets all tiles that can match the given source edge,
            //    connecting to the given edge of the matched tile
            //    (i.e. if your tile is connecting to the right,
            //     "destEdge" should be "left").
            inline const Set<const Tile*>& GetMatches(EdgeID srcEdge, Edges destEdge)
            {
                return matchingEdges[TileConnection(srcEdge, destEdge)];
            }


	    private:

            //All the tiles in the input data, including rotated/reflected permutations.
            List<Tile> tiles;
            //A quick lookup of which tiles can match with which environments.
            Dictionary<TileConnection, Set<const Tile*>> matchingEdges;
	    };
    }
}