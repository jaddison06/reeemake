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
//! format of an arg (positional/optional)
enum ArgFmt {positional, optional};

//! an optional argument
struct OptionalArg
{
    //! name to use with a -
    std::string shortName;
    /*! \brief name to use with a --
    *          also used as the name
    *          of the ParserOutputItem
    */
    std::string longName;
    //! description for the help page
    std::string description;
    //! type
    ArgType type;
    //! are we allowed duplicates?
    bool duplicatesAllowed = false;
};

//! a positional argument
struct PositionalArg
{
    /*! \brief name of the arg
    *          also used as the name
    *          of the ParserOutputItem
    */
    std::string name;
    //! description for the help page
    std::string description;
    //! type
    ArgType type;
};

/*! \brief part of a ParserOutputItem
*          needsParameter and receivedParameter
*          are unreliable, they get used internally but
*          please don't trust them from the caller
*/
struct ReturnArg
{
    //! name of the arg
    std::string name;
    /*! value, as a string
    * numerical args will be a string
    * that is convertible
    * 
    * boolean/storeTrue/storeFalse args
    * will be either "true" or "false"
    */
    std::string val;

    //! \deprecated
    //! does it need a parameter?
    bool needsParameter = false;
    //! \deprecated
    //! has it received its parameter?
    bool receivedParameter = true;

    //! type of the arg
    ArgType type;
    //! format of the arg
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
    //! everywhere the arg was given
    std::vector<ReturnArg> argInstances;
    //! the name of the arg
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