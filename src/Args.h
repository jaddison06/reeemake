#pragma once

#include "Logging.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

/*! \file
* Header for argument parsing
*/

/*! \brief type of an arg
*          don't use this in namespace std cause string will collide
*/
enum ArgType {integer, decimal, string, boolean, storeTrue, storeFalse};
//! format of an arg
enum ArgFmt {positional, optional};

//! an optional argument
struct OptionalArg
{
    std::string shortName;
    std::string longName;
    std::string description;
    ArgType type;
    bool duplicatesAllowed = false;
};

//! a positional argument
struct PositionalArg
{
    std::string name;
    std::string description;
    ArgType type;
};

/*! \brief part of a ParserOutputItem
*          needsParameter and receivedParameter
*          are unreliable, they get used internally but
*          please don't trust them from the caller
*/
struct ReturnArg
{
    std::string name;
    std::string val;

    bool needsParameter = false;
    bool receivedParameter = true;

    ArgType type;
    ArgFmt fmt;
};

/*! \brief returned by ArgParser::ParseArgs()
*          this is a container for ReturnArgs
*          as the user might provide an argument
*          more than once. name is the same as 
*          the name of the ReturnArgs.
*/
struct ParserOutputItem
{
    std::vector<ReturnArg> argInstances;
    std::string name;
};

std::string returnArgToString(ReturnArg);
std::string argTypeToString(ArgType);

//! ArgParser is the main class for parsing command-line args.
class ArgParser
{
    public:
        ArgParser(int, std::vector<std::string> *, std::vector<OptionalArg *> *, std::vector<PositionalArg *> *, std::string, std::string *);
        void ParseArgs(std::vector<ParserOutputItem> *);
    
    private:
        int argc;
        std::vector<std::string> * argv;
        std::vector<OptionalArg *> * allowedOptionals;
        std::vector<PositionalArg *> * allowedPositionals;

        std::string *description;
        std::string version;
        std::string helpText;
        void printHelp();

        bool isNumber(std::string);
        bool isInt(float);
        bool checkArgType(std::string, ArgType);

        void addReturnArgToOutput(std::vector<ParserOutputItem> *, ReturnArg *);

        OptionalArg helpArg
        {
            "h",
            "help",
            "Show help and exit",
            storeTrue
        };

        Logger logger;

};