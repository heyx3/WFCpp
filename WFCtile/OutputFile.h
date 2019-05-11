#pragma once

#include <string>
#include <numeric>

#include "Utils.h"
#include <Vector2i.h>

#include <Tiled/InputData.h>
#include <Tiled/State.h>

class EdgeData;
class TileFile;

struct OutputFile
{
    //Pre-determined output values.
    struct Placements
    {
        //The corners of the region to fill.
        //Min is inclusive; max is exclusive.
        WFC::Vector2i MinCorner, MaxCorner;
        //The name of the tile to use.
        std::string TileName;
        //The permutation of the tile to use.
        WFC::Transformations TilePermutation = WFC::Transformations::None;

        Placements() { }
        Placements(const std::string& configFileData,
                   int& outErrCode, std::string& outErrMsg);

        //Applies this instance to the given output array.
        //Returns false if the tile with my name or permutation couldn't be found.
        //Returns true if successful.
        bool Apply(const WFCT::InputData& inputs,
                   const std::vector<TileFile>& tiles,
                   const EdgeData& edges,
                   WFCT::State& algoState) const;
    };


    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max(),
           ClearSize = std::numeric_limits<size_t>().max(),
           NIterations = std::numeric_limits<size_t>().max(),
           Seed;
    bool PeriodicX = false,
         PeriodicY = false;
    std::vector<Placements> InitialPlacements;

    OutputFile();
    OutputFile(const std::string& inputFileContents,
               int& outErrCode, std::string& outErrMsg);
};