#pragma once

#include "logging.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

// don't use this in namespace std cause string will collide
enum ArgType {integer, decimal, string, boolean, storeTrue, storeFalse};
enum ArgFmt {positional, optional};

struct OptionalArg
{
    std::string shortName;
    std::string longName;
    std::string description;
    ArgType type;
    bool duplicatesAllowed;
};

struct PositionalArg
{
    std::string name;
    std::string description;
    ArgType type;
};

struct ReturnArg
{
    std::string name;
    std::string val;
    bool needsParameter = false;
    bool receivedParameter = true;
    ArgType type;
    ArgFmt fmt;
};

struct ParserOutputItem
{
    std::vector<ReturnArg> argInstances;
    std::string name;
};

std::string returnArgToString(ReturnArg);
std::string argTypeToString(ArgType);

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