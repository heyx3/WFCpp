#include "Utils.h"

#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>

#include <filesystem>
namespace fs = std::filesystem;


void Utils::TrimStringStart(std::string& str)
{
    size_t firstNonSpace = 0;
    while (std::isspace(str[firstNonSpace]))
        firstNonSpace += 1;

    str.erase(str.begin(), str.begin() + firstNonSpace);
}
void Utils::TrimStringEnd(std::string& str)
{
    int firstNonSpace = (int)str.size() - 1;
    while (firstNonSpace >= 0 && std::isspace(str[(size_t)firstNonSpace]))
        firstNonSpace -= 1;

    str.erase(str.begin() + (firstNonSpace + 1), str.end());
}
void Utils::TrimString(std::string& str)
{
    TrimStringStart(str);
    TrimStringEnd(str);
}

void Utils::TrimSpaceAndComments(std::string& str, const std::string& commentStart)
{
    auto startI = str.find(commentStart);
    if (startI != str.npos)
        str.erase(str.begin() + startI, str.end());

    TrimString(str);
}

bool Utils::TryParse(const std::string& str, size_t& outUint)
{
    //Fails if:
    //  1. The string is empty.
    //  2. Any characters aren't a digit.
    if (str.size() < 1 ||
        std::any_of(str.begin(), str.end(),
            [](char c) { return !std::isdigit(c); }))
    {
        return false;
    }

    outUint = std::strtoull(str.c_str(), nullptr, 10);
    return true;
}
bool Utils::TryParse(const std::string& str, int64_t& outInt)
{
    //Fails if:
    //  1. The string is empty.
    //  2. The first character is not a digit and not a negative sign.
    //  3. Subsequent characters aren't a digit.
    if (str.size() < 1 ||
        ((str[0] != '-') & !std::isdigit(str[0])) ||
        std::any_of(str.begin() + 1, str.end(),
                    [](char c) { return !std::isdigit(c); }))
    {
        return false;
    }

    outInt = std::strtoll(str.c_str(), nullptr, 10);
    return true;
}
bool Utils::TryParse(const std::string& str, float& outF)
{
    //Fails if:
    //  1. The string is empty.
    //  2. There is more than one dot in the string.
    //  2. The first character is not a digit, dot, or negative sign.
    //  3. Subsequent characters aren't a digit or dot.
    //Note that we aren't bothing to capture fancy things like scientific notation.
    if (str.size() < 1 ||
        std::count_if(str.begin(), str.end(),
                        [](char c) { return c = '.'; }) > 1 ||
        ((str[0] != '.') & (str[0] != '-') & !std::isdigit(str[0])) ||
        std::any_of(str.begin() + 1, str.end(),
                    [](char c) { return (c != '.') & !std::isdigit(c); }))
    {
        return false;
    }

    outF = std::strtof(str.c_str(), nullptr);
    return true;
}
bool Utils::TryParse(const std::string& str, bool& outB)
{
    //Force to lower-case.
    auto str2 = str;
    for (auto& c : str2)
        if (std::isupper(c))
            c = std::tolower(c);

    if (str == "true" || str == "1" || str == "yes" || str == "y")
        outB = true;
    else if (str == "false" || str == "0" || str == "no" || str == "n")
        outB = false;
    else
        return false;

    return true;
}

void Utils::GetLines(const std::string& strSrc, std::vector<std::string>& outLines,
                     const std::function<void(std::string&)>& filter,
                     const std::function<bool(const std::string&)>& predicate)
{
    std::stringstream streamSrc(strSrc);
    std::string str, str2;
    while (std::getline(streamSrc, str, '\r'))
    {
        std::stringstream strContents(str);
        while (std::getline(strContents, str2, '\n'))
        {
            filter(str2);
            if (predicate(str2))
                outLines.push_back(str2);
        }
    }
}

void Utils::SplitFirst(const std::string& src, char delim,
                       std::string& out1, std::string& out2)
{
    auto loc = std::find(src.begin(), src.end(), delim);
    out1 = std::string(src.begin(), loc);
    out2 = std::string(loc + 1, src.end());
}
void Utils::Split(const std::string& src, std::vector<std::string>& result,
                  const std::function<bool(char)>& shouldSplitAt)
{
    bool startedWord = false;
    for (size_t i = 0; i < src.size(); ++i)
    {
        if (startedWord)
        {
            if (shouldSplitAt(src[i]))
            {
                startedWord = false;
            }
            else
            {
                result[result.size() - 1] += src[i];
            }
        }
        else
        {
            if (!shouldSplitAt(src[i]))
            {
                startedWord = true;
                result.emplace_back();
                result[result.size() - 1] += src[i];
            }
        }
    }
}

bool Utils::ReadWholeFile(const std::string& path,
                          std::string& out_contents)
{
    //Get the size of the file.
    uintmax_t fileSize;
    try
    {
        fileSize = fs::file_size(path);
    }
    catch (fs::filesystem_error&)
    {
        return false;
    }

    //Open the file.
    std::ifstream file(path);
    if (!file.is_open())
        return false;
        
    //Read the contents.
    std::stringstream buffer;
    buffer << file.rdbuf();
    out_contents = buffer.str();
    return true;
}