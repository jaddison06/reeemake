#pragma once

#include <filesystem>
#include <fstream>
#include <time.h>
#include "Logging.h"

namespace fs = std::filesystem;

struct SourceFile
{
    fs::path path;
    std::time_t lastBuildTime;
    std::vector<fs::path> dependencies;
};

class SourceFileSerializationUtil
{
    public:
        void SerializeSourceFile(SourceFile *, fs::path);
        SourceFile DeserializeSourceFile(fs::directory_entry *);

    private:
        Logger logger{"Reeemake/SourceFile"};
};