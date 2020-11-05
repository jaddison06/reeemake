#pragma once

#include <filesystem>
#include <time.h>
#include <sstream>
#include "Logging.h"

namespace fs = std::filesystem;

struct SourceFile
{
    fs::path path;
    std::time_t lastBuildTime;
    std::vector<SourceFile> dependencies;
};

class SourceFileSerializationUtil
{
    public:
        std::string SerializeSourceFile(SourceFile *);
        SourceFile DeserializeSourceFile(std::string *, int = 1);

    private:
        Logger logger{"SourceFile"};
};