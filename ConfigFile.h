#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

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
    std::vector<std::string> options;
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
};