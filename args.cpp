#include "args.h"

ArgParser::ArgParser(int w, char *x[], std::vector<OptionalArg *> *y, std::vector<PositionalArg *> *z)
{
    argc = w;
    allowedOptionals = y;
    allowedPositionals = z;


    
}

void ArgParser::ParseArgs(std::vector<std::string> *output)
{

}