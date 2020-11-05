#pragma once

#include <filesystem>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include <optional>
#include "Logging.h"
#include "Args.h"
#include "SourceFile.h"
#include "ConfigFile.h"
#include "VectorThings.h"

#define VERSION "0.25"
#define DESCRIPTION "Super-Fast, Cross-Platform Build System For C++! Simple And Exciting!"

namespace fs = std::filesystem;

class Reeemake
{
    public:
        void build(int, char *[]);
    private:
        std::vector<fs::path> getFilesInDir(fs::path);
        void parseArgs(int, char *[]);

        void verboseSystem(std::string);
        bool isAnnoyingDir(std::string dirName);
        bool fileDataExists(fs::path *, std::vector<SourceFile> *, int *);
        std::string time_t_to_string(time_t *);
        
        bool needToBuild(fs::path *, std::vector<SourceFile> *, std::optional<SourceFile> = std::nullopt);
        std::vector<fs::path> getDependencies(fs::path *, std::vector<fs::path> *);
        bool hasBeenModified(SourceFile *);
        SourceFile genSourceFile(fs::path *, std::vector<fs::path> *);

        std::vector<ParserOutputItem> parsedArgs;

        Logger logger{"Reeemake"};
              
};