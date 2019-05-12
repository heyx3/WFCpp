#include "InputFile.h"

#include <sstream>
#include "Utils.h"


InputFile::InputFile(const std::string& inputFileContents,
                     int& outErrCode, std::string& outErrMsg)
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
                    outErrCode = 17; \
                    outErrMsg = std::string("Couldn't understand \"") + k + \
                                    "\"s value: \"" + val + "\""; \
                    return; \
                } \
            }
        MATCH("Width", Width)
        else MATCH("Height", Height)
        else if (key == "Permutations")
        {
            std::vector<std::string> transforms;
            Utils::Split(val, transforms, [](char c) { return c == ','; });
            for (std::string transformStr : transforms)
            {
                Utils::TrimString(transformStr);

                WFC::Transformations tr;
                if (!Utils::TryParse(transformStr, tr))
                {
                    outErrCode = 17;
                    outErrMsg = std::string("Couldn't parse transform: " + transformStr);
                    return;
                }

                PermutationsToUse += tr;
            }
        }
        else
        {
            outErrCode = 16;
            outErrMsg = std::string("Unknown field \"") + key + "\"";
            return;
        }
        #undef MATCH
    }

    //Check field values.
    if (Width < 1)
    {
        outErrCode = 17;
        outErrMsg = "Width can't be less than 1!";
        return;
    }
    if (Height < 1)
    {
        outErrCode = 17;
        outErrMsg = "Height can't be less than 1!";
        return;
    }
}