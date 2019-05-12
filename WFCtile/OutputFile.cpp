#include "OutputFile.h"

#include <random>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "Utils.h"
#include "EdgeData.h"
#include "TileFile.h"


OutputFile::Placements::Placements(const std::string& configFileData,
                                   int& outErrCode, std::string& outErrMsg)
{
    //Split the data into its elements.
    std::vector<std::string> elements;
    Utils::Split(configFileData, elements, [](char c) { return c == ';'; });
    if (elements.size() < 2 || elements.size() > 3)
    {
        outErrCode = 14;
        outErrMsg = "Expected 2 or 3 arguments for Init field; got this: " +
                    configFileData;
        return;
    }

    //Parse the first element: position/region that is set.
    std::vector<std::string> rectElements;
    Utils::Split(elements[0], rectElements, [](char c) { return c == ','; });
    if (rectElements.size() != 2 && rectElements.size() != 4)
    {
        outErrCode = 14;
        outErrMsg = "Expected 2 or 4 arguments for Init field's position; got this: " +
                    elements[0];
        return;
    }
    //Grab the min corner.
    int64_t i;
    if (!Utils::TryParse(rectElements[0], i))
    {
        outErrCode = 14;
        outErrMsg = "Expected MinX coordinate of Init placement; got this: " +
                    rectElements[0];
        return;
    }
    MinCorner.x = (int)i;
    if (!Utils::TryParse(rectElements[1], i))
    {
        outErrCode = 14;
        outErrMsg = "Expected MinY coordinate of Init placement; got this: " +
                    rectElements[1];
        return;
    }
    //Grab the max corner.
    if (rectElements.size() == 4)
    {
        if (!Utils::TryParse(rectElements[2], i))
        {
            outErrCode = 14;
            outErrMsg = "Expected MaxX coordinate of Init placement; got this: " +
                        rectElements[2];
            return;
        }
        MaxCorner.x = (int)i;

        if (!Utils::TryParse(rectElements[3], i))
        {
            outErrCode = 14;
            outErrMsg = "Expected MaxY coordinate of Init placement; got this: " +
                rectElements[3];
            return;
        }
        MaxCorner.y = (int)i;
    }
    else
    {
        MaxCorner = MinCorner + 1;
    }

    //Get the tile's name.
    TileName = elements[1];

    //Get the tile's permutation.
    if (elements.size() > 2)
    {
        const auto& transfStr = elements[2];
        if (!Utils::TryParse(transfStr, TilePermutation))
        {
            outErrCode = 14;
            outErrMsg = "Unknown transformation type: " + transfStr;
            return;
        }
    }
}
bool OutputFile::Placements::Apply(const WFCT::TilePermutator& tileData,
                                   const WFCT::InputData& inputs,
                                   const std::vector<TileFile>& tiles,
                                   const EdgeData& edges,
                                   WFCT::State& algoState) const
{
    //Get the tile with my name.
    auto tileFound = std::find_if(tiles.begin(), tiles.end(),
                                  [&](const TileFile& t) { return t.Name == TileName; });
    if (tileFound == tiles.end())
        return false;

    //Get the child tile with my permutation.
    const auto& parentTile = *tileFound;
    TileID_t parentID = (TileID_t)(tileFound - tiles.begin());
    auto childTileID = tileData.GetTileChild(parentID, TilePermutation);
    if (childTileID == TileID_INVALID)
        return false;

    //Apply that tile to the output.
    for (auto pos : WFC::Region2i(MinCorner, MaxCorner))
        algoState.SetTile(pos, childTileID, true);

    return true;
}


OutputFile::OutputFile()
{
    auto rng = std::random_device();
    auto rngOutput = std::uniform_int_distribution<size_t>(0, 999999999);
    Seed = rngOutput(rng);
}

OutputFile::OutputFile(const std::string& inputFileContents,
                       int& outErrCode, std::string& outErrMsg)
    : OutputFile()
{
    outErrCode = 0; //Success by default.

    //Parse the file.
    std::stringstream fileStream(inputFileContents);
    std::string line;
    while (std::getline(fileStream, line))
    {
        //Trim, and skip comments.
        Utils::TrimSpaceAndComments(line, "//");
        if (line.size() == 0)
            continue;

        //Split into a key and value pair.
        std::string key, val;
        Utils::SplitFirst(line, ':', key, val);

        //Find the key.
        //Helper macro does matching for a specific key/value pair.
        #define MATCH(k, v) \
            if (key == k) \
            { \
                if (!Utils::TryParse(val, v)) \
                { \
                    outErrCode = 14; \
                    outErrMsg = std::string("Couldn't understand \"") + k + \
                                    "\"s value: \"" + val + "\""; \
                    return; \
                } \
            }
        MATCH("Width", Width)
        else MATCH("Height", Height)
        else MATCH("ClearSize", ClearSize)
        else MATCH("PeriodicX", PeriodicX)
        else MATCH("PeriodicY", PeriodicY)
        else MATCH("Seed", Seed)
        else MATCH("GiveUpAfter", NIterations)
        else if (key == "Init")
        {
            InitialPlacements.emplace_back(val, outErrCode, outErrMsg);
            if (outErrCode != 0)
            {
                outErrMsg = "Error parsing Init command \"" + val + "\": " +
                            outErrMsg;
                return;
            }
        }
        else
        {
            outErrCode = 13;
            outErrMsg = std::string("Unknown field \"") + key + "\"";
            return;
        }
        #undef MATCH
    }

    //Check field values.
    if (Width == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'Width' field not given");
        return;
    }
    if (Height == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'Height' field not given");
        return;
    }
    if (ClearSize == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'ClearSize' field not given");
        return;
    }
}