#pragma once

#include <string>
#include <numeric>
#include <Helpers/Vector2i.h>


struct InputFile
{
    size_t Width = std::numeric_limits<size_t>().max(),
           Height = std::numeric_limits<size_t>().max();
    WFC::TransformationFlags PermutationsToUse = 0;

    InputFile() { }
    InputFile(const std::string& inputFileContents,
              int& outErrCode, std::string& outErrMsg);
};