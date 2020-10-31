#pragma once

#include <filesystem>
#include <vector>
#include "logging.h"
#include "args.h"

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

        std::vector<ParserOutputItem> parsedArgs;

        Logger logger;
        
};