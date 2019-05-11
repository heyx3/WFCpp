#include "CmdArgs.h"

#include "Utils.h"


CmdArgs::CmdArgs(int nArgs, char** argData,
                 int& outErrCode, std::string& outErrMsg)
    : DataDir(argData[0])
{
    outErrCode = 0;

    for (int i = 1; i < nArgs; ++i)
    {
        if (argData[i] == std::string("-dir"))
        {
            if (i >= nArgs - 1)
            {
                outErrMsg = "No path given after -dir argument!";
                outErrCode = 7;
                return;
            }

            i += 1;
            DataDir = argData[i];
        }
        else if (argData[i] == std::string("-progress"))
        {
            if (i >= nArgs - 1)
            {
                outErrMsg = "No number given after -progress argument!";
                outErrCode = 7;
                return;
            }

            i += 1;
            std::string progressStr = argData[i];

            //Try to parse the value.
            if (!Utils::TryParse(progressStr, ProgressInterval))
            {
                outErrCode = 7;
                outErrMsg = "-progress argument \"" + progressStr +
                                "\" isn't a valid non-negative integer";
                return;
            }
        }
        else if (argData[i] == std::string("-pgm"))
        {
            PgmMode = true;
        }
        else
        {
            outErrMsg = std::string("Unexpected argument: ") + argData[i];
            outErrCode = 7;
            return;
        }
    }
}

bool CmdArgs::ReadDataFile(const std::string& name, std::string& outContents) const
{
    return Utils::ReadWholeFile((DataDir / name).string(), outContents);
}