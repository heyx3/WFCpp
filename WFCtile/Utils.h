#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ctype.h>

#include <Tiled\Tile.hpp>
namespace WFCT = WFC::Tiled;


//Some helpful type definitions:

using Pixel_t = uint8_t;

using EdgeID_t = WFCT::EdgeID;
const auto EdgeID_INVALID = WFCT::EdgeID_INVALID;

using TileID_t = WFCT::TileID;
const auto TileID_INVALID = WFCT::TileID_INVALID;

using EdgeDirs = WFCT::EdgeDirs;

using EdgeToIDLookup = std::unordered_map<std::string, EdgeID_t>;
using IDToEdgeLookup = std::unordered_map<EdgeID_t, std::string>;


namespace Utils
{
    void TrimStringStart(std::string& str);
    void TrimStringEnd(std::string& str);
    void TrimString(std::string& str);

    void TrimSpaceAndComments(std::string& str,
                              const std::string& commentStart);

    bool TryParse(std::string str, WFC::Transformations& outTransf);
    bool TryParse(const std::string& str, size_t& outUint);
    bool TryParse(const std::string& str, int64_t& outInt);
    bool TryParse(const std::string& str, float& outF);
    bool TryParse(const std::string& str, bool& outB);

    void GetLines(const std::string& strSrc, std::vector<std::string>& outLines,
                  const std::function<void(std::string&)>& filter,
                  const std::function<bool(const std::string&)>& predicate);

    void SplitFirst(const std::string& src, char delim,
                    std::string& out1, std::string& out2);
    void Split(const std::string& src, std::vector<std::string>& result,
               const std::function<bool(char)>& shouldSplitAt);

    bool ReadWholeFile(const std::string& path,
                       std::string& out_contents);

}