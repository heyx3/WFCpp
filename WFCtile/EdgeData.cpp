#include "EdgeData.h"

#include <sstream>


EdgeData::EdgeData(const std::string& dataFile,
                   const std::vector<TileFile>& tiles,
                   int& outErrCode, std::string& outErrMsg)
{
    outErrCode = 0; //Success by default.

    //Build edge ID lookups.
    EdgeID_t nextEdgeID = 0;
    std::unordered_set<std::string> usedEdges;
    for (auto& tile : tiles)
    {
        for (auto& edge : tile.Edges)
        {
            if (usedEdges.find(edge) == usedEdges.end())
            {
                usedEdges.insert(edge);

                EdgeIDsByName[edge] = nextEdgeID;
                EdgeNamesByID[nextEdgeID] = edge;

                nextEdgeID += 1;
            }
        }
    }

    //Next, read and parse file data.
    std::stringstream fileContents(dataFile);
    std::string line;
    while (std::getline(fileContents, line))
    {
        //Trim, and skip comments.
        Utils::TrimSpaceAndComments(line, "//");
        if (line.size() == 0)
            continue;

        //Get the edge pair.
        std::string edges[2];
        Utils::SplitFirst(line, ':', edges[0], edges[1]);

        //Process each edge.
        for (uint8_t i = 0; i < 2; ++i)
        {
            auto otherI = (i + 1) % 2;

            //Error-checking.
            if (Pairs.find(edges[i]) != Pairs.end() &&
                edges[0] != edges[1])
            {
                outErrCode = 8;
                outErrMsg = std::string("Edge named \"") + edges[i] +
                                "\" appears more than once in EDGE_PAIRS.txt";
                return;
            }

            Pairs[edges[i]] = edges[otherI];
        }
    }
}