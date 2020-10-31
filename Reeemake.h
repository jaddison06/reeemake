#pragma once

#include <filesystem>
#include <vector>
#include <ctime>
#include "Logging.h"
#include "Args.h"
#include "SourceFile.h"
#include "ConfigFile.h"

#define VERSION "0.2"
#define DESCRIPTION "Super-Fast, Cross-Platform Build System For C++! Simple And Exciting!"

namespace fs = std::filesystem;

class Reeemake
{
    public:
        Reeemake(char *[]);
        void build(int, char *[]);
    private:
        std::vector<fs::path> getFilesInDir(fs::path);
        void parseArgs(int, char *[]);

        void verboseSystem(std::string);
        bool isAnnoyingDir(std::string dirName);
        bool fileDataExists(fs::path *, std::vector<SourceFile> *, int *);
        std::string time_t_to_string(time_t *);
        
        bool needToBuild(fs::path *, std::vector<SourceFile> *);
        std::vector<fs::path> getDependencies(fs::path *, std::vector<fs::path> *);
        bool hasBeenModified(SourceFile *);

        std::vector<ParserOutputItem> parsedArgs;

        Logger logger;
        
};