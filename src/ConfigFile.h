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

// a comment in the config file
#define CONFIG_COMMENT std::string("#")

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

// initialize w/ defaults
struct ConfigOptions
{
    // my way or the highway
    //
    // for minGW; cygwin would use C:\cygwin64\bin\g++.exe
    std::string compiler = "g++";
    std::vector<std::string> libraries;
    int cxxStandard = 17;
    std::vector<std::string> compilerFlags;

    std::string build = "binary";

    std::vector<Source> sources;
    bool manualIncludes = false;

    // not implemented yet
    std::vector<Dependency> depends;
    bool manualDepends = false;

    std::vector<std::string> postBuildCommands;
    std::vector<std::string> defines;
    std::string output;

    bool isInstalling;
    std::string installName;


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

class ConfigFileParser
{
    public:
        void ParseConfigFile(fs::path, ConfigOptions *, int level = 1);

        const std::vector<commandTemplate> allowedCommands
        {
            // 0
            {
                "depend",
                2
            },
            // 1
            {
                "no-depend",
                2
            },
            // 2
            {
                "manual-depends",
                1
            },
            // 3
            {
                "manual-includes",
                1
            },
            // 4
            {
                "include",
                1
            },
            // 5
            {
                "include-group",
                1
            },
            // 6
            {
                "exclude",
                1
            },
            // 7
            {
                "exclude-group",
                1
            },
            // 8
            {
                "library",
                1
            },
            // 9
            {
                "std",
                1
            },
            // 10
            {
                "comp",
                1
            },
            // 11
            {
                "output",
                1
            },
            // 12
            {
                "install",
                1
            },
            // 13
            {
                "system"
            },
            // 14
            {
                "system-now"
            },
            // 15
            {
                "define",
                2
            },
            // 16
            {
                "win"
            },
            // 17
            {
                "mac"
            },
            // 18
            {
                "linux"
            },
            // 19
            {
                "comp-flag",
                1
            },
            // 20
            {
                "import",
                1
            },
            // 21
            {
                "build",
                1
            }
        };

        const std::vector<std::string> outputOptions
        {
            "binary",
            "ninja",
            "makefile",
            "cmake",
            "script",
            "xcode",
            "msvc"
        };

    private:
        bool lineToCommand(std::string *);

        bool systemCommandAvailable(std::string *);
        bool isCommand(std::string *);
        bool isWhitespace(std::string *);

        void ParseCommand(command *, std::unordered_map<std::string, int> *, ConfigOptions *, int = 1);
        void osSpecificCommand(command *, std::unordered_map<std::string, int> *, ConfigOptions *, int);

        std::vector<std::string> splitString(std::string *, std::string = " ");

        Logger logger{"ConfigFileParser"};
};