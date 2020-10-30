#pragma once

#include "logging.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

enum ArgType {integer, decimal, string, boolean, storeTrue, storeFalse};

struct OptionalArg
{
    std::string shortName;
    std::string longName;
    std::string description;
    ArgType type;
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
};

std::string returnArgToString(ReturnArg);

class ArgParser
{
    public:
        ArgParser(int, std::vector<std::string> *, std::vector<OptionalArg *> *, std::vector<PositionalArg *> *, int, std::string *);
        void ParseArgs(std::vector<ReturnArg> *);
    
    private:
        int argc;
        std::vector<std::string> * argv;
        std::vector<OptionalArg *> * allowedOptionals;
        std::vector<PositionalArg *> * allowedPositionals;

        std::string *description;
        int version;
        std::string helpText;
        void printHelp();

        OptionalArg helpArg
        {
            "h",
            "help",
            "Show help and exit",
            storeTrue
        };

        Logger logger{"reeemake.args"};

};