#include "args.h"

#include <iostream>

ArgParser::ArgParser(int w, std::vector<char*> * x, std::vector<OptionalArg *> *y, std::vector<PositionalArg *> *z)
{
    logger.debug("ArgParser created");
    
    argc = w;
    argv = x;
    allowedOptionals = y;
    allowedPositionals = z;
    
}

void ArgParser::ParseArgs(std::vector<ReturnArg> *output)
{
    logger.info("Parsing args");
    for (auto arg : *argv) {
        logger.debug("Got arg "+(std::string)arg);
    }
}