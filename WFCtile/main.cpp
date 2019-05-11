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

#include "Utils.h"
#include "CmdArgs.h"
#include "InputFile.h"
#include "TileFile.h"
#include "OutputFile.h"
#include "EdgeData.h"

//TODO: Replace all "size_t" with "uint_fast32_t" or "uint_fast16_t".
//TODO: Remove "symmetric" data; just use asymmetric to map edges to themselves.


#include <Tiled/State.h>
namespace WFCT = WFC::Tiled;


//Takes in some input tile-sets as text files, and some parameters.
//"Tile-set" here means a 2D rectangle of bytes (0-255).
//Outputs to stdout, in order:
//    1. A byte set to 1 if the algorithm finished, or 0 if it failed.
//    2. The resulting 2D grid of bytes by assembling tiles together, by row, from min Y to max Y.

//Extra text files are expected to be alongside this executable;
//    you can change this by passing the directory as an argument: -dir C:\foo\bar.
//  * INPUT.txt: Data about the input tileset.
//  * OUTPUT.txt: Data about what kind of output to generate.
//  * SYMMETRIC.txt: (optional) A line-break-separated list of all edges
//        that are symmetric under reflection.
//  * ASYMMETRIC.txt: (optional) A line-break-separated list of pairs of edges,
//        separated with a colon, who are each others' reflection.
//  * INIT.txt: (optional) A line-break-separated list of tiles that should be pre-placed. E.x.:
//        0,1:MyTileFileName:Rot90CW
//        The available transformations are: None, Rot90CW, Rot270CW, Rot180, FlipX, FlipY.

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


namespace
{
    std::string ErrCodeToMsg(int i)
    {
        switch (i)
        {
            case 0: return "Success (no error)";
            case 1: return "Tile needs at least one row";
            case 2: return "Tile data has different lengths in each row";
            case 3: return "Needs to specify all four edge IDs";
            case 4: return "This tile's width or height doesn't match up with the Width/Height fields in INPUT.txt";
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


std::vector<TileFile> ReadTileFiles(const fs::path& directory, const InputFile& data,
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
            file.path().filename() != "OUTPUT.txt" &&
            file.path().filename() != "INIT.txt")
        {
            //Try to read the file.
            if (!Utils::ReadWholeFile(file.path().string(), fileContents))
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
                              const EdgeData& tileset,
                              const InputFile& inputData,
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
    bool success;
    #define READ_DATA_FILE_SOFT(name) \
        success = args.ReadDataFile(name, fileContents)
    #define READ_DATA_FILE(name) \
        READ_DATA_FILE_SOFT(name); \
        if (!success) \
        { \
            std::cerr << "Unable to open " << name << "\n"; \
            return 9; \
        }

    //Input:
    READ_DATA_FILE("INPUT.txt");
    InputFile inputData(fileContents, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Read INPUT.txt\n";
    
    //Output:
    READ_DATA_FILE("OUTPUT.txt");
    OutputFile outData(fileContents, errCode, errMsg);
    CHECK_ERR;
    std::cerr << "Read OUTPUT.txt\n";

    //Symmetric and Asymmetric edges:
    std::string edgeData_symmetric, edgeData_asymmetric;
    READ_DATA_FILE_SOFT("SYMMETRIC.txt");
    if (success)
        edgeData_symmetric = fileContents;
    READ_DATA_FILE_SOFT("ASYMMETRIC.txt");
    if (success)
        edgeData_asymmetric = fileContents;
    std::cerr << "Finished looking for SYMMETRIC.txt and ASYMMETRIC.txt\n";

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
    EdgeData tileset(edgeData_symmetric, edgeData_asymmetric, tiles,
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