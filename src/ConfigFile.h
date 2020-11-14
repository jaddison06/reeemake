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

/*! \file
* Header for config file parsing
*/

//! a comment in the config file
#define CONFIG_COMMENT std::string("#")

namespace fs = std::filesystem;

std::string readEntireFile(fs::path);

//! source file or group
enum SourceType {file, group};

//! code source(s) to be included/excluded
struct Source {
    //! the name of the file/group
    std::string name;
    //! to be included or excluded?
    bool include;
    //! file or group?
    SourceType type;
};

//! dependency tracking - either add or remove dependency
struct Dependency
{
    //! the source file
    fs::path source;
    //! the dependency
    fs::path depend;
    //! add or remove dependency?
    bool doesDepend;
};

/*! \brief all internal config options.
*          initialized with defaults.
*/
struct ConfigOptions
{
    // my way or the highway
    //
    // for minGW; cygwin would use C:\cygwin64\bin\g++.exe
    //! compiler to use
    std::string compiler = "g++";

    //! libraries to include
    //! \todo how to make this portable?
    std::vector<std::string> libraries;
    /*! \brief standard to use. doesn't support compiler standards
    *          eg gnu++17, only actual c++ standards.
    */
    int cxxStandard = 17;
    /*! \brief flags passed to the compiler AND linker. These aren't
    *          sanitised, we trust the user to make sure they're
    *          portable (we're probs using g++ anyway so it's fine)
    */
    std::vector<std::string> compilerFlags;

    //! type of build. See ConfigFileParser::outputOptions
    std::string build = "binary";

    //! sources
    std::vector<Source> sources;
    //! disable automatic includes?
    bool manualIncludes = false;

    //! \todo not implemented yet
    //! dependencies
    std::vector<Dependency> depends;
    //! disable automatic dependency tracking?
    bool manualDepends = false;

    /*! \brief commands to run after the build.
    *          not sanitised, we trust the user
    */
    std::vector<std::string> postBuildCommands;
    //! stuff to define in every file, before compilation
    std::vector<std::string> defines;

    //! file to output to
    std::string output;

    /*! \brief should we install?
    *          Configured at the same time as
    *          installName
    */
    bool isInstalling;
    //! Application name to install to.
    std::string installName;

};

//! internal. a line (command) in the config file
struct command
{
    //! the command
    std::string command;
    //! everything on the config line after the command, split by spaces
    std::vector<std::string> options;
};

/*! \brief template for a command. (badly named)
*          used internally to define allowed commands
*/
struct commandTemplate
{
    //! name of the command
    std::string name;
    /*! \brief how many args the command must have
    *          if  uninitialised, it's treated like infinity
    */
    std::optional<int> argCount;
};

//! main parser for config files
class ConfigFileParser
{
    public:
        void ParseConfigFile(fs::path, ConfigOptions *, int level = 1);

        /*! \brief commands currently supported
        *          public for reference only
        */
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

        /*! \brief options to output
        *          public for reference only
        */
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