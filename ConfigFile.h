#pragma once
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

enum SourceFileListType {blocklist, passlist};

struct SourceFileList
{
    SourceFileListType listType;

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
        // for minGW, cygwin would use C:\cygwin64\bin\g++.exe
        "g++",

        "g++",
        "g++",
    };
    SourceFileList buildFiles;

};

class ConfigFileParser
{

};