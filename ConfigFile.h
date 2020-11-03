#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <optional>
#include "VectorThings.h"
#include "Logging.h"

namespace fs = std::filesystem;

std::string readEntireFile(fs::path);

// source file or group
//
// include - either include this or exclude it
enum SourceType {file, group};
struct Source {
    std::string name;
    bool include;
    SourceType type;
};

// dependency tracking - either add or remove dependency
struct Dependency
{
    fs::path source;
    fs::path depend;
    bool doesDepend;
};

// store different values for one option
struct PlatformOptions
{
    std::string win;
    std::string mac;
    std::string linux;
};


// initialize w/ defaults
struct ConfigOptions
{
    // my way or the highway
    PlatformOptions compiler
    {
        // for minGW; cygwin would use C:\cygwin64\bin\g++.exe
        "g++",

        "g++",
        "g++",
    };
    std::vector<Source> sources;
    std::vector<Dependency> dependOptions;
    bool manualDepends = false;
    bool manualIncludes = false;
    std::vector<std::string> postBuildCommands;
    std::vector<std::string> defines;
    std::string binName;
    bool isInstalling;
    std::string installName;
    std::vector<std::string> libraries;
    int cxxStandard = 17;

};

struct command
{
    std::string command;
    std::vector<std::string> options;
};

struct commandTemplate
{
    std::string name;
    
    // if this is uninitialised, treat it like infinity
    std::optional<int> argCount;
};

const std::vector<commandTemplate> allowedCommands
        {
            {
                "depend",
                2
            },
            {
                "no-depend",
                2
            },
            {
                "manual-depends",
                1
            },
            {
                "manual-includes",
                1
            },
            {
                "include",
                1
            },
            {
                "include-group",
                1
            },
            {
                "exclude",
                1
            },
            {
                "exclude-group",
                1
            },
            {
                "library",
                1
            },
            {
                "std",
                1
            },
            {
                "comp",
                1
            },
            {
                "bin-name",
                1
            },
            {
                "install",
                1
            },
            {
                "system"
            },
            {
                "system-now"
            },
            {
                "define",
                2
            },
            {
                "win"
            },
            {
                "mac"
            },
            {
                "linux"
            }
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

        void ParseCommand(command *, std::unordered_map<std::string, int> *, ConfigOptions *);

        std::vector<std::string> splitString(std::string *, char = *" ");

        Logger logger{"Reeemake.ConfigFileParser"};
};