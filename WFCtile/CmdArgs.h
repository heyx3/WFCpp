#pragma once

#include <string>

#include <filesystem>
namespace fs = std::filesystem;


struct CmdArgs
{
    fs::path DataDir;
    size_t ProgressInterval = 0;
    bool PgmMode = false;

    CmdArgs() { }
    CmdArgs(int nArgs, char** argData,
            int& outErrCode, std::string& outErrMsg);

    bool ReadDataFile(const std::string& name, std::string& outContents) const;
};