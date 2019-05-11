#pragma once

#include <string>
#include <array>

#include "Utils.h"
#include <Array2D.hpp>

struct InputFile;


class TileFile
{
public:

    std::string Name;
    uint32_t Weight;

    WFC::Array2D<Pixel_t> Pixels;

    std::array<std::string, 4> Edges;
    WFC::TransformationFlags Symmetries = WFC::Transformations::None;


    TileFile(size_t width, size_t height, uint32_t weight = 100);
    TileFile(const std::string& fileContents, const InputFile& data,
             int& outErrCode);

    TileFile(TileFile&& from) = default;
    TileFile& operator=(TileFile&& from) = default;

    //Force move operators by deleting copy operators to ensure we're not being wasteful.
    TileFile(const TileFile& copy) = delete;
    TileFile& operator=(const TileFile& copy) = delete;
};