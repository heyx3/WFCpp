#include "TileFile.h"

#include <cctype>

#include "InputFile.h"


TileFile::TileFile(size_t width, size_t height, uint32_t weight)
    : Pixels((int)width, (int)height), Weight(weight)
{

}

TileFile::TileFile(const std::string& fileContents, const InputFile& data,
                   int& outErrCode)
    : Weight(100)
{
    outErrCode = 0;

    //Get individual lines, ignoring comments and trimming whitespace.
    std::vector<std::string> lines;
    Utils::GetLines(fileContents, lines,
                    [](std::string& s) { Utils::TrimSpaceAndComments(s, "//"); },
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
        Utils::Split(lineStr, lineElements,
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
            bool parseSuccessful = Utils::TryParse(pixelsStr[y][x], asLong);

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
            Utils::SplitFirst(*it, ':', key, val);

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
                if (Utils::TryParse(val, weight))
                {
                    Weight = (uint32_t)weight;
                }
                else
                {
                    outErrCode = 6;
                    return;
                }
            }
            else if (key == "Symmetry")
            {
                WFC::Transformations transf;
                if (Utils::TryParse(val, transf))
                {
                    Symmetries += transf;
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