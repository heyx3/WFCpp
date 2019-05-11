#pragma once

#include <string>
#include <array>

#include "Utils.h"
#include <Array2D.hpp>

struct InputFile;


struct TileFile
{
    std::string Name;
    WFC::Array2D<Pixel_t> Pixels;
    std::array<std::string, 4> Edges;
    uint32_t Weight;

    TileFile(size_t width, size_t height, uint32_t weight = 100);
    TileFile(const std::string& fileContents, const InputFile& data,
             int& outErrCode);

    TileFile(TileFile&& from) = default;
    TileFile& operator=(TileFile&& from) = default;

    TileFile(const TileFile& copy) = delete;
    TileFile& operator=(const TileFile& copy) = delete;
};