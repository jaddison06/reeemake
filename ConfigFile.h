#pragma once
#include <string>

enum SourceFileListType {blocklist, passlist};

struct PlatformOptions
{
    std::string win;
    std::string mac;
    std::string linux;
    std::string cygwin;
};

// initialize w/ defaults
struct ConfigOptions
{
    PlatformOptions compiler
    {};
};

class ConfigFileParser
{

};