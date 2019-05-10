#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <functional>
#include <unordered_set>
#include <array>

#include <filesystem>
namespace fs = std::filesystem;

//TODO: Replace all "size_t" with "uint_fast32_t" or "uint_fast16_t".


#include <Tiled/State.h>
namespace WFCT = WFC::Tiled;


//Takes in some input tile-sets as text files, and some parameters.
//"Tile-set" here means a 2D rectangle of bytes (0-255).
//Outputs to stdout, in order:
//    1. A byte set to 1 if the algorithm finished, or 0 if it failed.
//    2. The resulting 2D grid of bytes by assembling tiles together, by row, from min Y to max Y.

//The tilesets and parameters file are expected to be alongside this executable;
//    you can change this by passing the directory as an argument: -dir C:\foo\bar

//You can also make the program log its progress every N iterations with -progress N

//Finally, you can change the output format from binary to a PGM image file with "-pgm".


//Returns the following error codes:
// 0: Success.
// 1: Tile data has no rows.
// 2: Tile data has rows of different lengths.
// 3: Tile data does not specify all 4 edges.
// 4: Tile data is wrong size compared to INPUT.txt.
// 5: Tile data has unknown field name.
// 6: Tile data's "Weight" field has invalid value.
// 7: Invalid command-line arguments.
// 8: Edge name is defined in more than one place across SYMMETRIC.txt and ASYMMETRIC.txt.
// 9: Missing one of the key text files: SYMMETRIC.txt, ASYMMETRIC.txt, OUTPUT.txt, INPUT.txt.
// 10: No tile files found.
// 11: I/O error opening one of the tile data files.
// 12: Tile grid data contains a pixel that isn't an integer from 0-255.
// 13: OUTPUT.txt doesn't specify a required field, or uses an unknown field.
// 14: OUTPUT.txt gives an invalid value for a field.
// 15: Generation failed, because the number of iterations ran out
//       or we couldn't fix a contradiction.
// 16: INPUT.txt doesn't specify a required field, or uses an unknown field.
// 17: INPUT.txt gives an invalid value for a field.
// 100+: Error setting up WFC input data structures;
//           subtract 100 and look up the WFC::Tiled::InputData::ErrorCodes enum.


using Pixel_t = uint8_t;

using EdgeID_t = WFCT::EdgeID;
#define EdgeID_INVALID WFCT::EdgeID_INVALID

using TileID_t = WFCT::TileID;
#define TileID_INVALID WFCT::TileID_INVALID

using EdgeDirs = WFCT::EdgeDirs;

using EdgeToIDLookup = std::unordered_map<std::string, EdgeID_t>;
using IDToEdgeLookup = std::unordered_map<EdgeID_t, std::string>;


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

    void TrimSpaceAndComments(std::string& str)
    {
        auto commentStart = str.find("//");
        if (commentStart != str.npos)
            str.erase(str.begin() + commentStart, str.end());

        TrimString(str);
    }

    bool TryParse(const std::string& str, size_t& outUint)
    {
        //Fails if:
        //  1. The string is empty.
        //  2. Any characters aren't a digit.
        if (str.size() < 1 ||
            std::any_of(str.begin(), str.end(),
                        [](char c) { return !std::isdigit(c); }))
        {
            return false;
        }

        outUint = std::strtoull(str.c_str(), nullptr, 10);
        return true;
    }
    bool TryParse(const std::string& str, int64_t& outInt)
    {
        //Fails if:
        //  1. The string is empty.
        //  2. The first character is not a digit and not a negative sign.
        //  3. Subsequent characters aren't a digit.
        if (str.size() < 1 ||
            ((str[0] != '-') & !std::isdigit(str[0])) ||
            std::any_of(str.begin() + 1, str.end(),
                        [](char c) { return !std::isdigit(c); }))
        {
            return false;
        }

        outInt = std::strtoll(str.c_str(), nullptr, 10);
        return true;
    }
    bool TryParse(const std::string& str, float& outF)
    {
        //Fails if:
        //  1. The string is empty.
        //  2. There is more than one dot in the string.
        //  2. The first character is not a digit, dot, or negative sign.
        //  3. Subsequent characters aren't a digit or dot.
        //Note that we aren't bothing to capture fancy things like scientific notation.
        if (str.size() < 1 ||
            std::count_if(str.begin(), str.end(),
                          [](char c) { return c = '.'; }) > 1 ||
            ((str[0] != '.') & (str[0] != '-') & !std::isdigit(str[0])) ||
            std::any_of(str.begin() + 1, str.end(),
                        [](char c) { return (c != '.') & !std::isdigit(c); }))
        {
            return false;
        }

        outF = std::strtof(str.c_str(), nullptr);
        return true;
    }
    bool TryParse(const std::string& str, bool& outB)
    {
        //Force to lower-case.
        auto str2 = str;
        for (auto& c : str2)
            if (std::isupper(c))
                c = std::tolower(c);

        if (str == "true" || str == "1" || str == "yes" || str == "y")
            outB = true;
        else if (str == "false" || str == "0" || str == "no" || str == "n")
            outB = false;
        else
            return false;

        return true;
    }

    void GetLines(const std::string& strSrc, std::vector<std::string>& outLines,
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

    bool ReadWholeFile(const std::string& path,
                       std::string& out_contents)
    {
        //Get the size of the file.
        uintmax_t fileSize;
        try
        {
            fileSize = fs::file_size(path);
        }
        catch (fs::filesystem_error&)
        {
            return false;
        }

        //Open the file.
        std::ifstream file(path);
        if (!file.is_open())
            return false;
        
        //Read the contents.
        std::stringstream buffer;
        buffer << file.rdbuf();
        out_contents = buffer.str();
        return true;
    }

    std::string ErrCodeToMsg(int i)
    {
        switch (i)
        {
            case 0: return "Success (no error)";
            case 1: return "Tile needs at least one row";
            case 2: return "Tile data has different lengths in each row";
            case 3: return "Needs to specify all four edge IDs";
            case 4: return "This tile's width oe height doesn't match up with the Width/Height fields in INPUT.txt";
            case 5: return "Unexpected field; did you mean to comment a line out?";
            case 6: return "'Weight' field doesn't have a number value";
            case 7: return "Invalid command-line arguments";
            case 8: return "Edge's name is defined in more than one place";
            case 9: return "Can't find SYMMETRIC.txt and/or ASYMMETRIC.txt and/or OUTPUT.txt";
            case 10: return "No tile data files were found";
            case 11: return "I/O error when opening file";
            case 12: return "Tile grid data contains an invalid value (must be non-negative integer)";
            default: return "UNKNOWN ERROR CODE " + std::to_string(i);
        }
    }
}


struct CmdArgs
{
    fs::path DataDir;
    size_t ProgressInterval = 0;
    bool PgmMode = false;

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
            else if (argData[i] == std::string("-progress"))
            {
                if (i >= nArgs - 1)
                {
                    outErrMsg = "No number given after -progress argument!";
                    outErrCode = 7;
                    return;
                }

                i += 1;
                std::string progressStr = argData[i];

                //Try to parse the value.
                if (!TryParse(progressStr, ProgressInterval))
                {
                    outErrCode = 7;
                    outErrMsg = "-progress argument \"" + progressStr +
                                    "\" isn't a valid non-negative integer";
                    return;
                }
            }
            else if (argData[i] == std::string("-pgm"))
            {
                PgmMode = true;
            }
            else
            {
                outErrMsg = std::string("Unexpected argument: ") + argData[i];
                outErrCode = 7;
                return;
            }
        }
    }

    bool ReadDataFile(const std::string& name, std::string& outContents) const
    {
        return ReadWholeFile((DataDir / name).string(), outContents);
    }
};

struct InputData
{
    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max();
    bool UseRotations = false,
         UseReflections = false;

    InputData() { }
    InputData(const std::string& inputFileContents,
              int& outErrCode, std::string& outErrMsg)
    {
        outErrCode = 0; //Success by default.

        //Parse the file.
        std::stringstream fileStream(inputFileContents);
        std::string line;
        while (std::getline(fileStream, line))
        {
            //Trim, and skip comments.
            TrimSpaceAndComments(line);
            if (line.size() == 0)
                continue;

            //Split into a key and value pair.
            std::string key, val;
            SplitFirst(line, ':', key, val);

            //Find the key.
            //Helper macro does matching for a specific key/value pair.
            #define MATCH(k, v) \
                if (key == k) \
                { \
                    if (!TryParse(val, v)) \
                    { \
                        outErrCode = 17; \
                        outErrMsg = std::string("Couldn't understand \"") + k + \
                                       "\"s value: \"" + val + "\""; \
                        return; \
                    } \
                }
            MATCH("Width", Width)
            else MATCH("Height", Height)
            else MATCH("UseRotations", UseRotations)
            else MATCH("UseReflections", UseReflections)
            else
            {
                outErrCode = 16;
                outErrMsg = std::string("Unknown field \"") + key + "\"";
                return;
            }
            #undef MATCH
        }

        //Check field values.
        if (Width < 1)
        {
            outErrCode = 17;
            outErrMsg = "Width can't be less than 1!";
            return;
        }
        if (Height < 1)
        {
            outErrCode = 17;
            outErrMsg = "Height can't be less than 1!";
            return;
        }
    }
};
struct OutputData
{
    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max(),
           ClearSize = std::numeric_limits<size_t>().max(),
           NIterations = std::numeric_limits<size_t>().max(),
           Seed;
    bool PeriodicX = false,
         PeriodicY = false;

    OutputData()
    {
        auto rng = std::random_device();
        auto rngOutput = std::uniform_int_distribution<size_t>(0, 999999999);
        Seed = rngOutput(rng);
    }
    OutputData(const std::string& inputFileContents,
               int& outErrCode, std::string& outErrMsg)
        : OutputData()
    {
        outErrCode = 0; //Success by default.

        //Parse the file.
        std::stringstream fileStream(inputFileContents);
        std::string line;
        while (std::getline(fileStream, line))
        {
            //Trim, and skip comments.
            TrimSpaceAndComments(line);
            if (line.size() == 0)
                continue;

            //Split into a key and value pair.
            std::string key, val;
            SplitFirst(line, ':', key, val);

            //Find the key.
            //Helper macro does matching for a specific key/value pair.
            #define MATCH(k, v) \
                if (key == k) \
                { \
                    if (!TryParse(val, v)) \
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
};


//TODO: Read tiles before tile-set.
struct TileFile
{
    std::string Name;
    WFC::Array2D<Pixel_t> Pixels;
    std::array<std::string, 4> Edges;
    uint32_t Weight;

    TileFile(size_t width, size_t height, uint32_t weight = 100)
        : Pixels((int)width, (int)height), Weight(weight) { }
    TileFile(const std::string& fileContents, const InputData& data,
             int& outErrCode)
        : Weight(100)
    {
        outErrCode = 0;

        //Get individual lines, ignoring comments and trimming whitespace.
        std::vector<std::string> lines;
        GetLines(fileContents, lines,
                 TrimSpaceAndComments,
                 [](const std::string& str) { return true; });

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
        if (tileWidth != data.Width || tileHeight != data.Height)
        {
            outErrCode = 4;
            return;
        }
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
                size_t asLong;
                bool parseSuccessful = TryParse(pixelsStr[y][x], asLong);

                if (parseSuccessful && asLong <= std::numeric_limits<Pixel_t>().max())
                {
                    Pixels[WFC::Vector2i((int)x, (int)y)] = (Pixel_t)asLong;
                }
                else
                {
                    outErrCode = 12;
                    return;
                }
            }
        }


        //Grab other key/value data.
        for (auto it = tileEnd; it != lines.end(); ++it)
            if (it->size() > 0)
            {
                std::string key, val;
                SplitFirst(*it, ':', key, val);

                if (key == "Top")
                    Edges[EdgeDirs::MinY] = val;
                else if (key == "Bottom")
                    Edges[EdgeDirs::MaxY] = val;
                else if (key == "Left")
                    Edges[EdgeDirs::MinX] = val;
                else if (key == "Right")
                    Edges[EdgeDirs::MaxX] = val;
                else if (key == "Weight")
                {
                    size_t weight;
                    if (TryParse(val, weight))
                    {
                        Weight = (uint32_t)weight;
                    }
                    else
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
        
        //If any of the edges isn't set, that's considered an error.
        if (std::any_of(Edges.begin(), Edges.end(),
                        [](const std::string& str) { return str == ""; }))
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
struct TilesetData
{
    EdgeToIDLookup EdgeIDsByName;
    IDToEdgeLookup EdgeNamesByID;
    std::unordered_set<std::string> SymmetricEdges;
    std::unordered_map<std::string, std::string> AsymmetricEdges;

    TilesetData() { }
    TilesetData(const std::string& symmetricFileContents,
                const std::string& asymmetricFileContents,
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
        std::stringstream fileContents;
        std::string line;

        //Read the symmetric edges.
        fileContents = std::stringstream(symmetricFileContents);
        while (std::getline(fileContents, line))
        {
            //Trim, and skip comments.
            TrimSpaceAndComments(line);
            if (line.size() == 0)
                continue;

            //Error-checking.
            if (SymmetricEdges.find(line) != SymmetricEdges.end())
            {
                outErrCode = 8;
                outErrMsg = std::string("Edge named \"") + line +
                                "\" appears more than once in SYMMETRIC.txt";
                return;
            }

            SymmetricEdges.insert(line);
        }

        //Read the asymmetric edges.
        fileContents = std::stringstream(asymmetricFileContents);
        while (std::getline(fileContents, line))
        {
            //Trim and ignore comments.
            TrimSpaceAndComments(line);
            if (line.size() == 0)
                continue;
            
            //Get the edge pair.
            std::string edges[2];
            SplitFirst(line, ':', edges[0], edges[1]);

            //Process each edge.
            for (uint8_t i = 0; i < 2; ++i)
            {
                auto otherI = (i + 1) % 2;

                //Error-checking.
                if (AsymmetricEdges.find(edges[i]) != AsymmetricEdges.end())
                {
                    outErrCode = 8;
                    outErrMsg = std::string("Edge named \"") + edges[i] +
                                    "\" appears more than once in ASYMMETRIC.txt";
                    return;
                }
                if (SymmetricEdges.find(edges[i]) != SymmetricEdges.end())
                {
                    outErrCode = 8;
                    outErrMsg = std::string("Edge named \"") + edges[i] +
                                    "\" appears in both SYMMETRIC.txt and ASYMMETRIC.txt";
                }
                
                AsymmetricEdges[edges[i]] = edges[otherI];
            }
        }
    }
};


std::vector<TileFile> ReadTileFiles(const fs::path& directory, const InputData& data,
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
            file.path().filename() != "ASYMMETRIC.txt" &&
            file.path().filename() != "INPUT.txt" &&
            file.path().filename() != "OUTPUT.txt")
        {
            //Try to read the file.
            if (!ReadWholeFile(file.path().string(), fileContents))
            {
                errMsg = std::string() + "Unable to read " + file.path().string();
                errCode = 11;
                return files;
            }

            //Try to parse the file.
            files.emplace_back(fileContents, data, errCode);
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

WFCT::InputData MakeAlgoInput(const std::vector<TileFile>& tileFiles,
                              const TilesetData& tileset,
                              const InputData& inputData,
                              int& outErrCode)
{
    //Convert the tile data into WFC's data format.
    WFC::List<WFCT::Tile> algoInputTiles;
    for (size_t i = 0; i < tileFiles.size(); ++i)
    {
        const auto& tileFile = tileFiles[i];
        WFCT::Tile tile;

        tile.Weight = tileFile.Weight;
        tile.ID = (WFCT::TileID)i;
        for (int edgeI = 0; edgeI < 4; ++edgeI)
            tile.Edges[edgeI] = tileset.EdgeIDsByName.at(tileFile.Edges[edgeI]);

        algoInputTiles.PushBack(tile);
    }

    //Convert the symmetric edge data into WFC's data format.
    WFCT::EdgeIDSet algoSymmetricTiles;
    for (const auto& symmetricEdge : tileset.SymmetricEdges)
        algoSymmetricTiles.Add(tileset.EdgeIDsByName.at(symmetricEdge));

    //Convert the asymmetric edge data into WFC's data format.
    WFCT::EdgeToEdgeMap algoAsymmetricTiles;
    for (const auto& asymmetricEdge : tileset.AsymmetricEdges)
        algoAsymmetricTiles[tileset.EdgeIDsByName.at(asymmetricEdge.first)] =
            tileset.EdgeIDsByName.at(asymmetricEdge.second);

    //Construct the input instance.
    WFCT::InputData::ErrorCodes algoInputErr;
    WFCT::InputData algoInput(algoInputTiles,
                              inputData.UseRotations, inputData.UseReflections,
                              algoInputErr, &algoSymmetricTiles, &algoAsymmetricTiles);

    //Check the result.
    if (algoInputErr != WFCT::InputData::ErrorCodes::NoError)
        outErrCode = 100 + algoInputErr;
    return algoInput;
}


int main(int argc, char* argv[])
{
    int errCode;
    std::string errMsg;

    #define CHECK_ERR { \
        if (errCode != 0) { \
            std::cerr << errMsg << "\n"; \
            return errCode; \
        } \
    }
    
    //Parse command-line options.
    CmdArgs args(argc, argv, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Using directory: " << args.DataDir << "\n\n";

    //Read data files.
    std::string fileContents;
    #define READ_DATA_FILE(name) \
        if (!args.ReadDataFile(name, fileContents)) \
        { \
            std::cerr << "Unable to open " << name << "\n"; \
            return 9; \
        }

    //Input:
    READ_DATA_FILE("INPUT.txt");
    InputData inputData(fileContents, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Read INPUT.txt\n";
    
    //Output:
    READ_DATA_FILE("OUTPUT.txt");
    OutputData outData(fileContents, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Read OUTPUT.txt\n";

    //Symmetric and Asymmetric edges:
    std::string edgeData_symmetric, edgeData_asymmetric;
    READ_DATA_FILE("SYMMETRIC.txt");
    edgeData_symmetric = fileContents;
    READ_DATA_FILE("ASYMMETRIC.txt");
    edgeData_asymmetric = fileContents;
    std::cerr << "Read SYMMETRIC.txt and ASYMMETRIC.txt\n";

    //Read tile files.
    std::vector<TileFile> tiles = ReadTileFiles(args.DataDir, inputData, errCode, errMsg);
    CHECK_ERR;

    //Print the tile data for testing.
    for (const auto& tile : tiles)
    {
        std::cerr << "\"" << tile.Name << "\":\n";
        for (int y = 0; y < tile.Pixels.GetHeight(); ++y)
        {
            for (int x = 0; x < tile.Pixels.GetWidth(); ++x)
                std::cerr << (int)tile.Pixels[WFC::Vector2i(x, y)] << " ";
            std::cerr << "\n";
        }
        std::cerr << "\n";
    }

    //Read tileset data files.
    std::cerr << "Parsing SYMMETRIC.txt and ASYMMETRIC.txt...\n";
    TilesetData tileset(edgeData_symmetric, edgeData_asymmetric, tiles,
                        errCode, errMsg);
    CHECK_ERR;

    std::cerr << "Done reading data!\n\n";

    //Set up the WFC library data.
    auto algoInput = MakeAlgoInput(tiles, tileset, inputData, errCode);
    if (errCode != 0)
    {
        std::cerr << "WFC input error: " << (errCode - 100) << "\n";
        return errCode;
    }
    WFCT::State algoState(algoInput,
                          WFC::Vector2i((int)outData.Width, (int)outData.Height),
                          (unsigned int)outData.Seed,
                          outData.PeriodicX, outData.PeriodicY,
                          outData.ClearSize);

    //Run the algorithm.
    std::cerr << "Running at most " << outData.NIterations << " iterations of algorithm...\n";
    WFC::Nullable<bool> isFinished;
    WFC::List<WFC::Vector2i> failedPoses;
    size_t iterI = 0;
    while (iterI < outData.NIterations && !isFinished.HasValue)
    {
        isFinished = algoState.Iterate(failedPoses);

        iterI += 1;
        if (args.ProgressInterval > 0 && (iterI % args.ProgressInterval) == 0)
            std::cerr << "Completed iteration " << iterI << "\n";
    }

    //Output the result.
    if (!isFinished.HasValue)
    {
        std::cerr << "Ran out of iterations before finishing!\n";

        if (!args.PgmMode)
            std::cout << (unsigned char)0;
        return 15;
    }
    else if (isFinished.Value)
    {
        std::cerr << "Completed successfully in " << iterI << " iterations! Writing result...\n";

        //Output the results header.
        if (args.PgmMode)
        {
            std::cout << "P2\n" << (outData.Width * inputData.Width) << " " <<
                                   (outData.Height * inputData.Height) << "\n";

            Pixel_t maxVal = 0;
            for (auto& tile : tiles)
                for (auto tilePos : WFC::Region2i(tile.Pixels.GetDimensions()))
                    maxVal = std::max(maxVal, tile.Pixels[tilePos]);

            std::cout << (int)maxVal << "\n\n";
        }
        else
        {
            std::cout << (unsigned char)1;
        }

        //Output the results data.
        std::function<void(Pixel_t)> outputPixel;
        if (args.PgmMode)
            outputPixel = [](Pixel_t p) { std::cout << (int)p << ' '; };
        else
            outputPixel = [](Pixel_t p) { std::cout << p; };
        for (int pY = 0; pY < algoState.Output.GetHeight() * inputData.Height; ++pY)
        {
            int tY = pY / inputData.Height;
            int tpY = pY % inputData.Height;

            for (int pX = 0; pX < algoState.Output.GetWidth() * inputData.Width; ++pX)
            {
                int tX = pX / inputData.Width;
                int tpX = pX % inputData.Width;

                //Get the pixel array of the original untransformed version of this tile.
                //Look up the correct pixel with the current transformed version.
                auto tileID = algoState.Output[WFC::Vector2i(tX, tY)].Value.Value;
                const auto* tile = algoInput.GetTile(tileID);
                auto parentID = (tile->ParentID == TileID_INVALID) ?
                                    tileID : tile->ParentID;
                const auto& parentTile = tiles[parentID];
                const auto& tilePixels = parentTile.Pixels;

                auto pixel = tilePixels[WFC::Vector2i(tpX, tpY)
                                            .Transform(WFC::Invert(tile->ParentToMeTransform),
                                                       tilePixels.GetDimensions())];

                outputPixel(pixel);
                std::cerr << (int)pixel << ' ';

                //For the log output, add padding spaces.
                if (pixel < 10)
                    std::cerr << "  ";
                else if (pixel < 100)
                    std::cerr << ' ';
            }

            std::cerr << '\n';
            if (args.PgmMode)
                std::cout << '\n';
        }

        return 0;
    }
    else
    {
        std::cout << (unsigned char)0;
        
        std::cerr << "Failed! At the following positions:\n";
        for (const auto& failPos : failedPoses)
            std::cerr << "\t{" << failPos.x << ", " << failPos.y << "}\n";

        return 15;
    }

    #undef CHECK_ERR
    #undef READ_DATA_FILE
}