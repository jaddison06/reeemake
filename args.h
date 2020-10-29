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


class ArgParser
{
    public:
        ArgParser(int, char *[], std::vector<OptionalArg *> *, std::vector<PositionalArg *> *);
        void ParseArgs(std::vector<std::string> *);
    
    private:
        int argc;
        std::vector<std::string> argv;
        std::vector<OptionalArg *> * allowedOptionals;
        std::vector<PositionalArg *> * allowedPositionals;

        

};