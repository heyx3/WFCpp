#pragma once

#include <unordered_set>
#include <unordered_map>

#include "TileFile.h"


class EdgeData
{
public:

    EdgeToIDLookup EdgeIDsByName;
    IDToEdgeLookup EdgeNamesByID;
    std::unordered_map<std::string, std::string> Pairs;

    EdgeData() { }
    EdgeData(const std::string& edgePairsFileContents,
             const std::vector<TileFile>& tiles,
             int& outErrCode, std::string& outErrMsg);
};