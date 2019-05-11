#pragma once

#include <string>
#include <numeric>


struct OutputFile
{
    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max(),
           ClearSize = std::numeric_limits<size_t>().max(),
           NIterations = std::numeric_limits<size_t>().max(),
           Seed;
    bool PeriodicX = false,
         PeriodicY = false;

    OutputFile();
    OutputFile(const std::string& inputFileContents,
               int& outErrCode, std::string& outErrMsg);
};