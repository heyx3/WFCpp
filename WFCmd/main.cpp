#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <Simple/State.h>

//TODO: Support diagonal reflections.

//Takes in an input image (where each pixel is an unsigned int), and some parameters.
//Generates an output image via WFC.

//If "-shellMode" argument is passed, then nothing is printed to standard output
//    except for the final resulting image.

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
3                   <----- The pattern width.
3                   <----- The pattern height.
true                <----- Make the input periodic horizontally?
true                <----- Make the input periodic vertically?
false               <----- Also use rotated copies of the input?
false               <----- Also use reflected copies of the input?
32                  <----- Width of the output image.
32                  <----- Height of the output image.
41231               <----- PRNG seed.
false               <----- Make the output periodic horizontally?
true                <----- Make the output periodic vertically?
2                   <----- Violation clear size (enter 0 to simply fail on a constraint violation).
-1                  <----- Number of iterations (anything below 0 is interpreted as "infinity").
true                <----- Display each step of the process? (not needed if in shell mode).
    [File end on previous line]
*/

//Returns the following error codes:
// 0: success
// 1: input image is badly-formed
// 2: the WFC constraint was violated and Violation Clear Size was 0


//Parses a pixel, expected to be a uint.
//Returns whether or not the parse succeeded.
bool TryParsePixel(const std::string& pixel, WFC::Simple::Pixel& outValue)
{
    std::stringstream converter(pixel);
    return !(converter >> outValue).fail() &&
            (converter >> std::ws).eof();
}
//Parses a row of pixels, where each pixel is a uint separated by at least one space each.
//Returns whether or not the parse succeeded.
bool TryParsePixelRow(const std::string& rowAsString, std::vector<WFC::Simple::Pixel>& outValue)
{
    std::string number;
    for (char c : rowAsString)
    {
        if (c == ' ')
        {
            if (!number.empty())
            {
                WFC::Simple::Pixel p;
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

    if (!number.empty())
    {
        WFC::Simple::Pixel p;
        if (!TryParsePixel(number, p))
            return false;

        outValue.push_back(p);
        number.clear();
    }

    return true;
}


//The following two functions get data from the given input stream.
//If the given output stream isn't null, it writes instructions to the user as it gets input.

WFC::Simple::InputData GetDataFromStream(std::istream& input, std::ostream* output)
{
    //Let input stream parse "true" as true and "false" as false.
    std::boolalpha(input);

    const size_t maxUint = std::numeric_limits<size_t>().max();

    //Get the input image data.
    WFC::Array2D<WFC::Simple::Pixel> inputPixels;
    {
        size_t nPixelsPerRow = maxUint;
        std::vector<std::vector<WFC::Simple::Pixel>> pixelsByRow;

        if (output != nullptr)
        {
            *output << "Enter rows of input image data, where each pixel is a non-negative integer.\n" <<
                       "To finish the image, enter an empty line.\n" <<
                       "Malformed lines are ignored.\n\n";
        }

        std::string imageLine;
        std::getline(input, imageLine);

        std::vector<WFC::Simple::Pixel> pixelRow;
        while (!imageLine.empty())
        {
            if (!TryParsePixelRow(imageLine, pixelRow))
            {
                if (output != nullptr)
                    *output << "Row is badly formed: \"" << imageLine << "\"\nExiting program...\n\n";
                exit(1);
            }

            //If this is the first row, see how long it is.
            if (nPixelsPerRow == maxUint)
            {
                if (pixelRow.size() == 0)
                {
                    if (output != nullptr)
                        *output << "Input image's first row is empty! Exiting program...\n\n";
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
                if (output != nullptr)
                    *output << "Row is wrong length: \"" << imageLine << "\"\nExiting program...\n\n";
                exit(1);
            }

            //The row is fine, so add it to the image.
            pixelsByRow.push_back(pixelRow);
            pixelRow.clear();

            imageLine.clear();
            std::getline(input, imageLine);
        }

        if (pixelsByRow.size() == 0)
        {
            if (output != nullptr)
                *output << "No rows for input image.\n";
            exit(1);
        }

        //Convert to the Array2D<Pixel>.
        inputPixels = WFC::Array2D<WFC::Simple::Pixel>((int)nPixelsPerRow, (int)pixelsByRow.size());
        for (auto inputPos : WFC::Region2i(inputPixels.GetDimensions()))
            inputPixels[inputPos] = pixelsByRow[inputPos.y][inputPos.x];
    }

    WFC::Vector2i patternSize;
    if (output != nullptr)
        *output << "Enter the width of each pattern:\n";
    input >> patternSize.x;
    if (output != nullptr)
        *output << "Enter the height of each pattern:\n";
    input >> patternSize.y;

    bool periodicX, periodicY;
    if (output != nullptr)
        *output << "Make the input periodic horizontally? (true/false)\n";
    input >> periodicX;
    if (output != nullptr)
        *output << "Make the input periodic vertically? (true/false)\n";
    input >> periodicY;

    bool rotateInput, reflectInput;
    if (output != nullptr)
        *output << "Also use rotated copies of the input? (true/false)\n";
    input >> rotateInput;
    if (output != nullptr)
        *output << "Also use reflected copies of the input? (true/false)\n";
    input >> reflectInput;

    return WFC::Simple::InputData(inputPixels, patternSize, periodicX, periodicY,
                                  rotateInput, reflectInput);
}
WFC::Simple::State InitializeStateFromStream(const WFC::Simple::InputData& wfcInput,
                                             std::istream& input, std::ostream* output)
{
    //Let input stream parse "true" as true and "false" as false.
    std::boolalpha(input);

    WFC::Vector2i size;
    if (output != nullptr)
        *output << "Enter the width of the output image:\n";
    input >> size.x;
    if (output != nullptr)
        *output << "Enter the height of the output image:\n";
    input >> size.y;

    if (output != nullptr)
        *output << "Enter the PRNG seed:\n";
    unsigned int seed;
    input >> seed;

    if (output != nullptr)
        *output << "Make the output periodic horizontally? (true/false)\n";
    bool periodicX, periodicY;
    input >> periodicX;
    if (output != nullptr)
        *output << "Make the output periodic vertically? (true/false)\n";
    input >> periodicY;

    size_t violationClearSize;
    if (output != nullptr)
        *output << "Enter the violation clear size (0 for failure on violations):\n";
    input >> violationClearSize;

    return WFC::Simple::State(wfcInput, size, seed, periodicX, periodicY, violationClearSize);
}


void PrintOutput(const WFC::Simple::State& state, std::ostream& stream)
{
    //Convert each element to a string, and find the longest string.

    std::vector<std::vector<std::string>> elementsByRow(state.Output.GetDimensions().y);
    for (size_t y = 0; y < state.Output.GetDimensions().y; ++y)
        elementsByRow[y].resize(state.Output.GetDimensions().x);

    size_t maxLength = 0;
    for (WFC::Vector2i pos : WFC::Region2i(state.Output.GetDimensions()))
    {
        auto& pixel = state.Output[pos];

        std::string elementStr;
        if (pixel.Value.HasValue)
        {
            elementStr = std::to_string(pixel.Value.Value);
        }
        else if (pixel.ColorFrequencies.GetSize() == 0)
        {
            elementStr = "x";
        }
        else
        {
            for (const auto& kvp : pixel.ColorFrequencies)
            {
                elementStr += "|";
                elementStr += std::to_string(kvp.second);
            }
        }

        maxLength = std::max(maxLength, elementStr.size());
        elementsByRow[pos.y][pos.x] = elementStr;
    }

    //Now print the elements, keeping the output evenly-spaced.
    for (size_t y = 0; y < elementsByRow.size(); ++y)
    {
        if (y > 0)
            stream << "\n";

        size_t linePos = 0,
               targetLinePos = 0;
        for (size_t x = 0; x < elementsByRow[y].size(); ++x)
        {
            //Move up to the correct indentation.
            while (linePos < targetLinePos)
            {
                stream << ' ';
                linePos += 1;
            }

            //Print the element.
            auto& elementStr = elementsByRow[y][x];
            stream << elementStr;
            linePos += elementStr.size();

            //Advance the indentation.
            targetLinePos += maxLength + 1;
        }
    }
}


int main(int argc, char* argv[])
{
    //Parse command-line options.
    bool shellMode = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-shellMode")
            shellMode = true;
    }

    auto outputStream = (shellMode ? nullptr : &std::cout);

    //Get input data:
    auto inputData = GetDataFromStream(std::cin, shellMode ? nullptr : &std::cout);
    if (!shellMode)
        std::cout << "\n\n";
    auto wfcState = InitializeStateFromStream(inputData, std::cin, shellMode ? nullptr : &std::cout);

    //Get output data:
    int _nIterations;
    if (!shellMode)
        std::cout << "Enter the number of iterations (anything less than 0 is interpreted as \"infinity\":\n";
    std::cin >> _nIterations;
    size_t nIterations = (_nIterations < 0 ?
                              std::numeric_limits<size_t>::max() :
                              (size_t)_nIterations + 1);

    //Get whether to print the status at each stage:
    bool printStatus = false;
    if (!shellMode)
    {
        std::cout << "Show the output at each stage? (true/false):\n";
        std::cin >> printStatus;
    }

    //Run the generator loop.
    int errorCode = 0;
    WFC::Vector2i changedPos;
    WFC::List<WFC::Vector2i> failedPoses;
    size_t iterationCount = 0;
    while (nIterations > 0)
    {
        iterationCount += 1;
        nIterations -= 1;
        WFC::Nullable<bool> result = wfcState.Iterate(changedPos, failedPoses);

        if (result.HasValue)
        {
            if (result.Value)
            {
                if (!shellMode)
                    std::cout << "Finished! Took " << iterationCount << " iterations.\n\n";
                break;
            }
            else
            {
                if (!shellMode)
                {
                    std::cout << "Failed at the following output pixels:\n";
                    for (size_t i = 0; i < failedPoses.GetSize(); ++i)
                        std::cout << "{" << failedPoses[i].x << "," << failedPoses[i].y << "} ";
                    std::cout << "\n\n";
                }

                errorCode = 2;
                break;
            }
        }
        else
        {
            //Print the current state of the output.
            if (printStatus)
            {
                std::cout << "#" << iterationCount << "\tChanged {"
                          << changedPos.x << ", " << changedPos.y << "}\n";
                PrintOutput(wfcState, std::cout);
                std::cout << "\n\n";
            }
        }
    }

    //Print the final output.
    PrintOutput(wfcState, std::cout);

    if (!shellMode)
    {
        std::cout << "\n\nEnter anything to exit.\n";

        std::string dummy;
        std::cin >> dummy;
        std::cout << "\n\n\n";
    }

    return errorCode;
}