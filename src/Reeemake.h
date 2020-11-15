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

/*! \file
* Header for main Reeemake stuff
*/

//! Reeemake version
#define VERSION "0.26"

//! \todo make this not dumbass
//! Reeemake description
#define DESCRIPTION "Super-Fast, Cross-Platform Build System For C++! Simple And Exciting!"

namespace fs = std::filesystem;

//! Main Reeemake class
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

        bool containsDirs(std::vector<fs::path>);
        std::vector<fs::path> getFilesInDirRecursive(fs::path);

        std::vector<ParserOutputItem> parsedArgs;

        Logger logger{"Reeemake"};
              
};