#pragma once

#include <filesystem>
#include <time.h>
#include <sstream>
#include "Logging.h"

/*! \file
* Header for SourceFiles
*/

namespace fs = std::filesystem;

//! a source file and all the data we need on it
struct SourceFile
{
    //! path to the file
    fs::path path;
    //! when it was last built (according to the current build profile)
    std::time_t lastBuildTime;
    //! its dependencies. recursive until they're not.
    std::vector<SourceFile> dependencies;
};

//! Utility class to serialize and deserialize SourceFiles
class SourceFileSerializationUtil
{
    public:
        std::string SerializeSourceFile(SourceFile *);
        SourceFile DeserializeSourceFile(std::string *, int = 1);

    private:
        Logger logger{"SourceFile"};
};