#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "VectorThings.h"
#include "Logging.h"

namespace fs = std::filesystem;

std::string readEntireFile(fs::path);

enum SourceFileListType {blocklist, passlist};

struct SourceFileList
{
    // default
    SourceFileListType listType { blocklist };

    // fs::path?
    std::vector<fs::path> files;
};

struct PlatformOptions
{
    std::string win;
    std::string mac;
    std::string linux;
};

// initialize w/ defaults
struct ConfigOptions
{
    // my way or the high way
    PlatformOptions compiler
    {
        // for minGW; cygwin would use C:\cygwin64\bin\g++.exe
        "g++",

        "g++",
        "g++",
    };
    SourceFileList buildFiles;
    std::vector<std::string> postBuildCommnands;
    bool isInstalling;
    std::string installName;   

};

struct command
{
    std::string command;
    std::string options;
};

const std::vector<std::string> allowedCommands
        {
            "depend",
            "no-depend",
            "manual-depends",
            "source-files",
            "include",
            "include-group",
            "exclude",
            "exclude-group",
            "library",
            "std",
            "comp",
            "bin-name",
            "install",
            "system",
            "system-now",
            "define",
            "win",
            "mac",
            "linux"
};


class ConfigFileParser
{
    public:
        void ParseConfigFile(fs::path, ConfigOptions *);


    private:
        bool lineToCommand(std::string *);

        bool systemCommandAvailable(std::string *);
        bool isCommand(std::string *);
        bool isWhitespace(std::string *);

        Logger logger{"Reeemake.ConfigFileParser"};
};