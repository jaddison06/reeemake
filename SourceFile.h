#pragma once

#include <filesystem>
#include <time.h>
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
        void SerializeSourceFile(SourceFile *, fs::path *);
        SourceFile DeserializeSourceFile(fs::directory_entry *);

    private:
        Logger logger{"Reeemake/SourceFile"};
};