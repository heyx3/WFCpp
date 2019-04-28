#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <functional>
#include <unordered_set>

#include <filesystem>
namespace fs = std::filesystem;


#include <Tiled/State.h>


//Takes in some input tile-sets as text files, and some parameters.
//"Tile-set" here means a 2D rectangle of chars.
//Outputs to stdout, a 2D grid of chars by assembling tiles together.
//Each output char is separated by a space (or line break).

//The tilesets and parameters file are expected to be alongside this executable;
//    you can change this by passing the directory as an argument: -dir C:\foo\bar

//Returns the following error codes:
// 0: Success.
// 1: Tile data has no rows.
// 2: Tile data has rows of different lengths.
// 3: Tile data does not specify all 4 edge IDs.
// 4: Tile data has unknown edge names.
// 5: Tile data has unknown field name.
// 6: Tile data's "Weight" field has invalid value.
// 7: Invalid command-line arguments.
// 8: Edge name is defined in more than one place.
// 9: Missing SYMMETRIC.txt and/or ASYMMETRIC.txt.
// 10: No tile files found.
// 11: I/O error opening one of the tile data files.
// 12: Tile grid data contains a pixel that isn't an integer from 0-255.



using Pixel_t = uint8_t;
using EdgeID_t = uint16_t;
const EdgeID_t EdgeID_INVALID = -1;

using EdgeToIDLookup = std::unordered_map<std::string, EdgeID_t>;


namespace
{
    void TrimStringStart(std::string& str)
    {
        size_t firstNonSpace = 0;
        while (std::isspace(str[firstNonSpace]))
            firstNonSpace += 1;

        str.erase(str.begin(), str.begin() + firstNonSpace);
    }
    void TrimStringEnd(std::string& str)
    {
        int firstNonSpace = (int)str.size() - 1;
        while (firstNonSpace >= 0 && std::isspace(str[(size_t)firstNonSpace]))
            firstNonSpace -= 1;
        
        str.erase(str.begin() + (firstNonSpace + 1), str.end());
    }
    void TrimString(std::string& str)
    {
        TrimStringStart(str);
        TrimStringEnd(str);
    }

    void GetLines(std::string& strSrc, std::vector<std::string>& outLines,
                  const std::function<void(std::string&)>& filter,
                  const std::function<bool(const std::string&)>& predicate)
    {
        std::stringstream streamSrc(strSrc);
        std::string str, str2;
        while (std::getline(streamSrc, str, '\r'))
        {
            std::stringstream strContents(str);
            while (std::getline(strContents, str2, '\n'))
            {
                filter(str2);
                if (predicate(str2))
                    outLines.push_back(str2);
            }
        }
    }

    void SplitFirst(const std::string& src, char delim,
                    std::string& out1, std::string& out2)
    {
        auto loc = std::find(src.begin(), src.end(), delim);
        out1 = std::string(src.begin(), loc);
        out2 = std::string(loc + 1, src.end());
    }
    void Split(const std::string& src, std::vector<std::string>& result,
               const std::function<bool(char)>& shouldSplitAt)
    {
        bool startedWord = false;
        for (size_t i = 0; i < src.size(); ++i)
        {
            if (startedWord)
            {
                if (shouldSplitAt(src[i]))
                {
                    startedWord = false;
                }
                else
                {
                    result[result.size() - 1] += src[i];
                }
            }
            else
            {
                if (!shouldSplitAt(src[i]))
                {
                    startedWord = true;
                    result.emplace_back();
                    result[result.size() - 1] += src[i];
                }
            }
        }
    }

    void ReadWholeFile(const std::string& path,
                       std::string& out_contents, bool& out_wasSuccessful)
    {
        out_wasSuccessful = false; //Fail by default, to be safe.

        //Get the size of the file.
        uintmax_t fileSize;
        try
        {
            fileSize = fs::file_size(path);
        }
        catch (fs::filesystem_error&)
        {
            out_wasSuccessful = false;
            return;
        }

        //Open the file.
        std::ifstream file(path);
        out_wasSuccessful = file.is_open();
        if (!out_wasSuccessful)
            return;
        
        //Read the contents.
        std::stringstream buffer;
        buffer << file.rdbuf();
        out_contents = buffer.str();
        out_wasSuccessful = true;
    }

    std::string ErrCodeToMsg(int i)
    {
        switch (i)
        {
            case 0: return "Success (no error)";
            case 1: return "Tile needs at least one row";
            case 2: return "Tile data has different lengths in each row";
            case 3: return "Needs to specify all four edge IDs";
            case 4: return "At least one of the edge IDs is unknown (did you make a typo?)";
            case 5: return "Unexpected field; did you mean to comment a line out?";
            case 6: return "'Weight' field doesn't have a number value";
            case 7: return "Invalid command-line arguments";
            case 8: return "Edge's name is defined in more than one place";
            case 9: return "Can't find SYMMETRIC.txt and/or ASYMMETRIC.txt";
            case 10: return "No tile data files were found";
            case 11: return "I/O error when opening file";
            case 12: return "Tile grid data contains an invalid value (must be non-negative integer)";
            default: return "UNKNOWN ERROR CODE " + std::to_string(i);
        }
    }
}


struct CmdArgs
{
    std::string DataDir;

    CmdArgs() { }
    CmdArgs(int nArgs, char** argData,
            int& outErrCode, std::string& outErrMsg)
        : DataDir(argData[0])
    {
        outErrCode = 0;

        for (int i = 1; i < nArgs; ++i)
        {
            if (argData[i] == std::string("-dir"))
            {
                if (i >= nArgs - 1)
                {
                    outErrMsg = "No path given after -dir argument!";
                    outErrCode = 7;
                    return;
                }

                i += 1;
                DataDir = argData[i];
            }
            else
            {
                outErrMsg = std::string("Unexpected argument: ") + argData[i];
                outErrCode = 7;
                return;
            }
        }
    }
};

struct TilesetData
{
    EdgeToIDLookup EdgeIDsByName;
    std::unordered_set<std::string> SymmetricEdges;
    std::unordered_map<std::string, std::string> AsymmetricEdges;

    TilesetData() { }
    TilesetData(std::string& symmetricFileContents,
                std::string& asymmetricFileContents,
                int& outErrCode, std::string& outErrMsg)
    {
        outErrCode = 0; //Success by default.
        std::stringstream fileContents;
        std::string line;

        EdgeID_t nextEdge = 0;
        std::unordered_set<std::string> usedEdges;

        //Read the symmetric edges.
        fileContents = std::stringstream(symmetricFileContents);
        while (std::getline(fileContents, line))
        {
            //Filter the line.
            TrimString(line);
            if (line.compare(0, 2, "//") == 0)
                continue;
            if (usedEdges.find(line) != usedEdges.end())
            {
                outErrCode = 8;
                outErrMsg = std::string("Edge named \"") + line +
                                "\" is defined more than once";
                return;
            }

            //Assign it an ID.
            EdgeIDsByName[line] = nextEdge;
            nextEdge += 1;

            //Store it in relevant data structures.
            usedEdges.insert(line);
            SymmetricEdges.insert(line);
        }

        //Read the asymmetric edges.
        fileContents = std::stringstream(asymmetricFileContents);
        while (std::getline(fileContents, line))
        {
            //Filter the line.
            TrimString(line);
            if (line.compare(0, 2, "//") == 0)
                continue;
            
            //Get the edge pair.
            std::string edges[2];
            SplitFirst(line, ':', edges[0], edges[1]);

            //Process each edge.
            for (uint8_t i = 0; i < 2; ++i)
            {
                auto otherI = (i + 1) % 2;

                //Error-checking.
                if (usedEdges.find(edges[i]) != usedEdges.end())
                {
                    outErrCode = 8;
                    outErrMsg = std::string("Edge named \"") + edges[i] +
                                    "\" is defined more than once";
                    return;
                }

                //Assign it an ID and store it in the relevant data structures.
                EdgeIDsByName[edges[i]] = nextEdge;
                nextEdge += 1;
                usedEdges.insert(edges[i]);
                AsymmetricEdges[edges[i]] = edges[otherI];
            }
        }
    }
};
struct TileFile
{
    std::string Name;
    WFC::Array2D<Pixel_t> Pixels;
    EdgeID_t Edge_MinX = EdgeID_INVALID,
             Edge_MaxX = EdgeID_INVALID,
             Edge_MinY = EdgeID_INVALID,
             Edge_MaxY = EdgeID_INVALID;
    float Weight;

    TileFile(size_t width, size_t height, float weight = 100.0f)
        : Pixels((int)width, (int)height), Weight(weight) { }
    TileFile(std::string& fileContents,
             const EdgeToIDLookup& edgeLookup,
             int& outErrCode)
        : Weight(100.0f)
    {
        outErrCode = 0;

        //Get individual lines, ignoring comments and trimming whitespace.
        std::vector<std::string> lines;
        GetLines(fileContents, lines,
                    TrimString,
                    [](const std::string& str)
                        { return (str.compare(0, 2, "//") != 0); });

        //Pull the tile data out of the first few lines.
        auto tileBegin = std::find_if(lines.begin(), lines.end(),
                                      [](const std::string& str) { return !str.empty(); });
        if (tileBegin == lines.end())
        {
            outErrCode = 1;
            return;
        }
        auto tileEnd = std::find_if(tileBegin, lines.end(),
                                    [](const std::string& str) { return str.empty(); });

        //Separate the individual elements in the tile data grid.
        std::vector<std::vector<std::string>> pixelsStr;
        for (auto it = tileBegin; it != tileEnd; ++it)
        {
            const auto& lineStr = *it;
            auto& lineElements = pixelsStr.emplace_back();
            Split(lineStr, lineElements,
                  [](int c) { return std::isspace((char)c); });
        }

        //Parse the tile data.
        int tileWidth = (int)pixelsStr[0].size(),
            tileHeight = (int)pixelsStr.size();
        Pixels.Reset(tileWidth, tileHeight);
        for (int y = 0; y < tileHeight; ++y)
        {
            //Make sure this row is the right length.
            if ((int)pixelsStr[y].size() != tileWidth)
            {
                outErrCode = 2;
                return;
            }

            //Parse each element in the row.
            for (size_t x = 0; x < pixelsStr[y].size(); ++x)
            {
                const auto& element = pixelsStr[y][x];
                if (std::any_of(element.begin(), element.end(),
                                [](int i) { return !std::isdigit((char)i); }))
                {
                    outErrCode = 12;
                    return;
                }

                long asLong = std::strtol(element.c_str(), nullptr, 10);
                if (asLong < 0 || asLong > std::numeric_limits<Pixel_t>().max())
                {
                    outErrCode = 12;
                    return;
                }

                Pixels[WFC::Vector2i((int)x, (int)y)] = (Pixel_t)asLong;
            }
        }


        //Grab other key/value data.

        //Helper macro that gets the id of the given edge name.
        #define TRY_GET_EDGE(name, outVar) { \
            auto edgeID = edgeLookup.find(name); \
            if (edgeID == edgeLookup.end()) { \
                outErrCode = 4; \
                return; \
            } else { \
                outVar = edgeID->second; \
            } }

        for (auto it = tileEnd; it != lines.end(); ++it)
            if (it->size() > 0)
            {
                std::string key, val;
                SplitFirst(*it, ':', key, val);

                if (key == "Top")
                    TRY_GET_EDGE(val, Edge_MinY)
                else if (key == "Bottom")
                    TRY_GET_EDGE(val, Edge_MaxY)
                else if (key == "Left")
                    TRY_GET_EDGE(val, Edge_MinX)
                else if (key == "Right")
                    TRY_GET_EDGE(val, Edge_MaxX)
                else if (key == "Weight")
                {
                    try
                    {
                        Weight = std::stof(val);
                    }
                    catch (const std::invalid_argument&)
                    {
                        outErrCode = 6;
                        return;
                    }
                }
                else
                {
                    outErrCode = 5;
                    return;
                }
            }
        
        if (Edge_MinX == EdgeID_INVALID || Edge_MaxX == EdgeID_INVALID ||
            Edge_MinY == EdgeID_INVALID || Edge_MaxY == EdgeID_INVALID)
        {
            outErrCode = 3;
            return;
        }
    }

    TileFile(TileFile&& from) = default;
    TileFile& operator=(TileFile&& from) = default;

    TileFile(const TileFile& copy) = delete;
    TileFile& operator=(const TileFile& copy) = delete;
};


std::vector<TileFile> ReadTileFiles(const std::string& directory,
                                    const TilesetData& data,
                                    int& errCode, std::string& errMsg)
{
    std::vector<TileFile> files;
    std::string fileContents;
    for (auto& file : fs::directory_iterator(directory))
    {
        std::cerr << "Reading " << file.path() << "...\n";
        if (file.is_regular_file() &&
            file.path().has_extension() &&
            file.path().extension() == ".txt" &&
            file.path().filename() != "SYMMETRIC.txt" &&
            file.path().filename() != "ASYMMETRIC.txt")
        {
            //Try to read the file.
            bool wasSuccess;
            ReadWholeFile(file.path().string(), fileContents, wasSuccess);
            if (!wasSuccess)
            {
                errMsg = std::string() + "Unable to read " + file.path().string();
                errCode = 11;
                return files;
            }

            //Try to parse the file.
            files.emplace_back(fileContents, data.EdgeIDsByName, errCode);
            files.back().Name = file.path().filename().replace_extension("").string();
            if (errCode != 0)
            {
                errMsg = std::string() + "Error in \"" + file.path().string() +
                         "\": " + ErrCodeToMsg(errCode) + "\n";
                return files;
            }
        }
    }

    //A lack of tile data files is considered a fatal error.
    if (files.size() == 0)
    {
        errCode = 10;
        errMsg = ErrCodeToMsg(errCode);
    }

    return files;
}


int main(int argc, char* argv[])
{
    int errCode;
    std::string errMsg;

    #define CHECK_ERR { \
        if (errCode != 0) { \
            std::cerr << errMsg << "\n"; \
            std::cin >> errCode; \
            return errCode; \
        } \
    }
    
    //Parse command-line options.
    auto args = CmdArgs(argc, argv, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Using directory: " << args.DataDir << "\n\n";

    //Read tileset data files.
    TilesetData tileset;
    {
        bool wasSuccessful;

        //Try to open the symmetric data file.
        auto path_symmetricData = fs::path(args.DataDir);
        path_symmetricData /= "SYMMETRIC.txt";
        std::string symmetricData;
        ReadWholeFile(path_symmetricData.string(), symmetricData, wasSuccessful);
        if (!wasSuccessful)
        {
            std::cerr << "Couldn't find SYMMETRIC.txt\n";
            std::cin >> errCode;
            return 9;
        }

        //Try to open the asymmetric data file.
        auto path_asymmetricData = fs::path(args.DataDir);
        path_asymmetricData /= "ASYMMETRIC.txt";
        std::string asymmetricData;
        ReadWholeFile(path_asymmetricData.string(), asymmetricData, wasSuccessful);
        if (!wasSuccessful)
        {
            std::cerr << "Couldn't find ASYMMETRIC.txt\n";
            std::cin >> errCode;
            return 9;
        }

        std::cerr << "Parsing SYMMETRIC.txt and ASYMMETRIC.txt...\n";
        tileset = TilesetData(symmetricData, asymmetricData,
                              errCode, errMsg);
        CHECK_ERR;
    }


    //Read tile files.
    std::vector<TileFile> tiles = ReadTileFiles(args.DataDir, tileset, errCode, errMsg);
    CHECK_ERR;

    for (const auto& tile : tiles)
    {
        std::cerr << "\"" << tile.Name << "\":\n";
        for (size_t y = 0; y < tile.Pixels.GetHeight(); ++y)
        {
            for (size_t x = 0; x < tile.Pixels.GetWidth(); ++x)
                std::cerr << (int)tile.Pixels[WFC::Vector2i(x, y)] << " ";
            std::cerr << "\n";
        }
        std::cerr << "\n";
    }

    std::cerr << "Done reading data!\n\n";

    //TODO: Run the algo.

    return 0;
}