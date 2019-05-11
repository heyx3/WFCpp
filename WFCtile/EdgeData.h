#pragma once

#include <unordered_set>
#include <unordered_map>

#include "TileFile.h"


struct EdgeData
{
    EdgeToIDLookup EdgeIDsByName;
    IDToEdgeLookup EdgeNamesByID;
    std::unordered_set<std::string> SymmetricEdges;
    std::unordered_map<std::string, std::string> AsymmetricEdges;

    EdgeData() { }
    EdgeData(const std::string& symmetricFileContents,
             const std::string& asymmetricFileContents,
             const std::vector<TileFile>& tiles,
             int& outErrCode, std::string& outErrMsg);
};