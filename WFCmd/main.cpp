#include <iostream>
#include <sstream>
#include <string>

#include <State.h>


//Takes in an input image (where each pixel is an unsigned int), and some parameters.
//Generates an output image via WFC.

//The program settings are read from standard input.
//The resulting image is sent to standard output as a series of rows separated by line breaks.
//Each output row is a set of pixels separated by single spaces.
//Each "pixel" is an unsigned integer.

//If piping input in via a file or something, use the following template:
/*
    [File start on next line]
1 4 3 4 3 4 1       <----- This is the first row of the input image. Each pixel is an integer.
1 4 4 4 4 4 1       <----- This is the second row of the input image.
1 4 4 4 4 4 1       <----- etc.
1 4 3 4  3 4 1      <----- Spaces don't mess anything up; use as many of them as you want here!
                    <----- Empty line; the image is completed.
TODO: MORE DATA HERE.
32                  <----- Width of the output image.
32                  <----- Height of the output image.
41231               <----- PRNG seed.
false               <----- Make the output periodic horizontally?
true                <----- Make the output periodic vertically?
2                   <----- Violation clear size (enter 0 to just fail on a constraint violation)
    [File end on previous line]
*/

//Outputs the following error codes:
// 0: everything went fine.
// 1: input image is badly-formed.


//Parses a pixel, expected to be a uint.
//Returns whether or not the parse succeeded.
bool TryParsePixel(const std::string& pixel, WFC::Pixel& outValue)
{
    std::stringstream converter(pixel);
    return !(converter >> outValue).fail() &&
            (converter >> std::ws).eof();
}
//Parses a row of pixels, where each pixel is a uint separated by at least one space each.
//Returns whether or not the parse succeeded.
bool TryParsePixelRow(const std::string& rowAsString, std::vector<WFC::Pixel>& outValue)
{
    std::string number;
    for (char c : rowAsString)
    {
        if (c == ' ')
        {
            if (!number.empty())
            {
                WFC::Pixel p;
                if (!TryParsePixel(number, p))
                    return false;

                outValue.push_back(p);
                number.clear();
            }
        }
        else if (c < '0' || c > '9')
        {
            return false;
        }
        else
        {
            number += c;
        }
    }

    return true;
}


WFC::InputData GetDataFromStream(std::istream& input, std::ostream& output)
{
    const size_t maxUint = std::numeric_limits<size_t>().max();

    //Get the input image data.
    WFC::Array2D<WFC::Pixel> inputPixels;
    {
        size_t nPixelsPerRow = maxUint;
        std::vector<std::vector<WFC::Pixel>> pixelsByRow;

        output << "Enter rows of input image data, where each pixel is a non-negative integer.\n" <<
                  "To finish the image, enter an empty line.\n" <<
                  "Malformed lines are ignored.\n\n";

        std::string imageLine;
        input >> imageLine;
        std::vector<WFC::Pixel> pixelRow;
        while (!imageLine.empty())
        {
            if (!TryParsePixelRow(imageLine, pixelRow))
            {
                output << "Row is badly formed: \"" << imageLine << "\"\nExiting program...\n\n";
                exit(1);
            }

            //If this is the first row, see how long it is.
            if (nPixelsPerRow == maxUint)
            {
                if (pixelRow.size() == 0)
                {
                    output << "Input image's first row is empty! Exiting program...\n\n";
                    exit(1);
                }
                else
                {
                    nPixelsPerRow = pixelRow.size();
                }
            }
            //Otherwise, make sure the length is same as the last row.
            else if (pixelRow.size() != nPixelsPerRow)
            {
                output << "Row is wrong length: \"" << imageLine << "\"\nExiting program...\n\n";
                exit(1);
            }

            //The row is fine, so add it to the image.
            pixelsByRow.push_back(pixelRow);
            pixelRow.clear();

            input >> imageLine;
        }

        //Convert to the Array2D<Pixel>.
        inputPixels = WFC::Array2D<WFC::Pixel>(nPixelsPerRow, pixelsByRow.size());
        for (WFC::Vector2i inputPos : WFC::Region2i(inputPixels.GetDimensions()))
            inputPixels[inputPos] = pixelsByRow[inputPos.y][inputPos.x];
    }

    //TODO: Finish. Update example file above.
}
WFC::State InitializeStateFromStream(const WFC::InputData& wfcInput,
                                     std::istream& input, std::ostream& output)
{
    //Let input stream parse "true" as true and "false" as false.
    std::boolalpha(input);

    WFC::Vector2i size;
    output << "Enter the width of the output image:\n";
    input >> size.x;
    output << "Enter the height of the output image:\n";
    input >> size.y;

    output << "Enter the PRNG seed:\n";
    unsigned int seed;
    input >> seed;

    output << "Make the output periodic horizontally? (true/false)\n";
    bool periodicX, periodicY;
    input >> periodicX;
    output << "Make the output periodic vertically? (true/false)\n";
    input >> periodicY;

    size_t violationClearSize;
    output << "Enter the violation clear size (0 for failure on violations):\n";
    input >> violationClearSize;

    return WFC::State(wfcInput, size, seed, periodicX, periodicY, violationClearSize);
}


int main()
{
    auto inputData = GetDataFromStream(std::cin, std::cout);

    //TODO: Finish.

    return 0;
}