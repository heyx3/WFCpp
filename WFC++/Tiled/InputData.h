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
            inline const Set<Tile>& GetMatches(EdgeID srcEdge, Edges destEdge)
            {
                return matchingEdges[TileAndEdge(srcEdge, destEdge)];
            }


	    private:
            
            //A source tile's edge and desired destination edge.
            //Used for quick lookup of what tiles can fit in a given situation.
            struct TileAndEdge
            {
                EdgeID SrcEdge;
                Edges DestEdge;

                TileAndEdge(EdgeID src, Edges dest)
                    : SrcEdge(src), DestEdge(dest) { }

                //Gets the hash value for an instance.
                //Allows it to be used as a Dictionary<> key.
                inline unsigned int operator()(const TileAndEdge& t) const
                {
                    //Use the Vector2i hasher that already exists.
                    Vector2i v2(t.SrcEdge, t.DestEdge);
                    return Vector2i()(v2);
                }

                inline bool operator==(const TileAndEdge& t2) const
                {
                    return (SrcEdge == t2.SrcEdge) & (DestEdge == t2.DestEdge);
                }
                inline bool operator!=(const TileAndEdge& t2) const { return !(operator==(t2)); }
            };


            //A quick lookup of which tiles can match with which environments.
            Dictionary<TileAndEdge, Set<Tile>> matchingEdges;
	    };
    }
}