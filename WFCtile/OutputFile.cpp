#include "OutputFile.h"

#include <random>
#include <sstream>
#include <fstream>

#include "Utils.h"


OutputFile::OutputFile()
{
    auto rng = std::random_device();
    auto rngOutput = std::uniform_int_distribution<size_t>(0, 999999999);
    Seed = rngOutput(rng);
}

OutputFile::OutputFile(const std::string& inputFileContents,
                       int& outErrCode, std::string& outErrMsg)
    : OutputFile()
{
    outErrCode = 0; //Success by default.

    //Parse the file.
    std::stringstream fileStream(inputFileContents);
    std::string line;
    while (std::getline(fileStream, line))
    {
        //Trim, and skip comments.
        Utils::TrimSpaceAndComments(line, "//");
        if (line.size() == 0)
            continue;

        //Split into a key and value pair.
        std::string key, val;
        Utils::SplitFirst(line, ':', key, val);

        //Find the key.
        //Helper macro does matching for a specific key/value pair.
        #define MATCH(k, v) \
            if (key == k) \
            { \
                if (!Utils::TryParse(val, v)) \
                { \
                    outErrCode = 14; \
                    outErrMsg = std::string("Couldn't understand \"") + k + \
                                    "\"s value: \"" + val + "\""; \
                    return; \
                } \
            }
        MATCH("Width", Width)
        else MATCH("Height", Height)
        else MATCH("ClearSize", ClearSize)
        else MATCH("PeriodicX", PeriodicX)
        else MATCH("PeriodicY", PeriodicY)
        else MATCH("Seed", Seed)
        else MATCH("GiveUpAfter", NIterations)
        else
        {
            outErrCode = 13;
            outErrMsg = std::string("Unknown field \"") + key + "\"";
            return;
        }
        #undef MATCH
    }

    //Check field values.
    if (Width == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'Width' field not given");
        return;
    }
    if (Height == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'Height' field not given");
        return;
    }
    if (ClearSize == std::numeric_limits<size_t>().max())
    {
        outErrCode = 13;
        outErrMsg = std::string("'ClearSize' field not given");
        return;
    }
}