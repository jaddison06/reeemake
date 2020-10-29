#pragma once

#include "logging.h"
#include <string>
#include <vector>

struct OptionalArg
{
    std::string shortName;
    std::string longName;
    std::string description;
    enum type {integer, decimal, string, boolean, storeTrue, storeFalse};
};

struct PositionalArg
{
    std::string name;
    std::string description;
    enum type {integer, decimal, string, boolean};
};

struct ReturnArg
{
    std::string name;
    std::string val;
};


class ArgParser
{
    public:
        ArgParser(int, std::vector<char*> *, std::vector<OptionalArg *> *, std::vector<PositionalArg *> *);
        void ParseArgs(std::vector<ReturnArg> *);
    
    private:
        int argc;
        std::vector<char*> * argv;
        std::vector<OptionalArg *> * allowedOptionals;
        std::vector<PositionalArg *> * allowedPositionals;

        Logger logger{"reeemake.args"};

};