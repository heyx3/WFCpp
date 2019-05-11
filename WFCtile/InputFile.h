#pragma once

#include <string>
#include <numeric>


struct InputFile
{
    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max();
    bool UseRotations = false,
         UseReflections = false;

    InputFile() { }
    InputFile(const std::string& inputFileContents,
              int& outErrCode, std::string& outErrMsg);
};