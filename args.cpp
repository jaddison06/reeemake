#include "args.h"

std::string returnArgToString(ReturnArg returnArg)
{
    return returnArg.name + ", " + returnArg.val;
}

ArgParser::ArgParser(int w, std::vector<std::string> * x, std::vector<OptionalArg *> *y, std::vector<PositionalArg *> *z, int u, std::string *v)
{
    logger.debug("ArgParser created");
    
    argc = w;
    argv = x;
    allowedOptionals = y;
    allowedPositionals = z;

    version = u;
    description = v;

    allowedOptionals->push_back(&helpArg);
    
}

void ArgParser::printHelp()
{
    logger.info("Generating help");

    helpText += argv->at(0) + " (version " + std::to_string(version) + ")\n" + *description;

    logger.debug("Help text generated:\n"+helpText);

    std::cout << helpText << "\n";

    logger.info("Exiting");
    exit(0);
}

// TODO:
// arg type checking
// check for duplicate keys
// make sure optionals w parameter get it

void ArgParser::ParseArgs(std::vector<ReturnArg> *output)
{
    logger.info("Parsing args");

    // check that hard-coded data is fine before we start
    // looking at user input
    std::vector<std::string> argNames;
    for (auto positional : *allowedPositionals)
    {
        if ( std::find(argNames.begin(), argNames.end(), positional->name) != argNames.end() )
        {
            logger.error("Argument name " + positional->name + " used more than once");
        }
    }

    for (auto optional : *allowedOptionals)
    {
        if ( std::find(argNames.begin(), argNames.end(), optional->longName) != argNames.end() )
        {
            logger.error("Argument name " + optional->longName + " used more than once");
        }
    }

    std::vector<std::string> positionals;
    ReturnArg nextArg;

    bool nextArgIsValue = false;
    bool firstArg = true;
    for (auto arg : *argv)
    {
        logger.debug("Got arg "+arg);

        // is it the first arg?
        if ( !firstArg )
        {
            // is it optional?
            if ( arg.substr(0, 1) == "-" )
            {
                
                logger.debug("is optional switch");

                bool validArg = false;
                OptionalArg argCopy;

                // short or long name?
                if ( arg.substr(0, 2) == "--" )
                {
                    logger.debug("is long name");
                    for (auto optional : *allowedOptionals)
                    {
                        if ( optional->longName == arg.substr(2, arg.length() - 2) )
                        {
                            argCopy = *optional;
                            
                            validArg = true;
                            break;
                        }
                    }
                } else {
                    logger.debug("is short name");
                    for (auto optional : *allowedOptionals)
                    {
                        if ( optional->shortName == arg.substr(1, arg.length() - 1) )
                        {
                            argCopy = *optional;
                            
                            validArg = true;
                            break;
                        }
                    }
                }

                // stuff for all optionals
                if ( validArg )
                {
                    nextArg.name = argCopy.longName;
                    logger.debug("Is a valid arg");

                    if ( argCopy.longName == "help" )
                    {
                        // exits w status 0
                        printHelp();
                    }
                    
                    // does the arg take a parameter ting?
                    if ( !( argCopy.type == storeTrue || argCopy.type == storeFalse ) )
                    {
                        logger.debug("Arg takes a parameter");
                        nextArgIsValue = true;
                    } else
                    {
                        logger.debug("Arg doesn't take a parameter");

                        // we've already got the name, so now add the val & push it
                        if ( argCopy.type == storeTrue )
                        {
                            nextArg.val = "true";
                        } else
                        {
                            nextArg.val = "false";
                        }

                        output->push_back(nextArg);
                    }
                } else
                {
                    // it's not a valid arg
                    logger.error("Unknown option "+arg+" , try --help");
                }


            } else {
                logger.debug("isn't a switch");
                if ( nextArgIsValue )
                {
                    logger.debug("is a value for an optional arg");
                    nextArgIsValue = false;
                    // name should already be there, so whack in the val & push
                    nextArg.val = arg;
                    output->push_back(nextArg);

                } else
                {
                    logger.debug("is a positional arg");

                    // just add it to positionals, we deal with them later
                    positionals.push_back(arg);
                }
            }
        } else
        {

            logger.debug("Is first arg - program name");
            firstArg = false;
        }

        // at this point all optionals should have
        // been parsed, now for positionals
        if ( !(positionals.size() == allowedPositionals->size()) )
        {
            // ewww
            logger.error("Received "+std::to_string(positionals.size())+" positional arguments, was expecting "+std::to_string(allowedPositionals->size()));
        } else
        {
            // all good, parse da positional tingz
            
            for ( int i=0; i<positionals.size(); i++ )
            {
                ReturnArg returnArg
                {
                    allowedPositionals->at(i)->name,
                    positionals.at(i)
                };
                output->push_back(returnArg);
            }
        }
    }
}