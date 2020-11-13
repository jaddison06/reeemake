#include "Args.h"

/*! \file
* Code for argument parsing
*/

/*! convert a ReturnArg to a string
* \param returnArg the ReturnArg to convert
*/
std::string returnArgToString(ReturnArg returnArg)
{
    return returnArg.name + ", " + returnArg.val + "\n" +
    std::to_string(returnArg.needsParameter) + " " +
    std::to_string(returnArg.receivedParameter) + "\n" +
    argTypeToString(returnArg.type) + "\n";
}

/*! convert an ArgType to a string
* \param argType the ArgType to convert
*/
std::string argTypeToString(ArgType argType)
{
    switch(argType)
    {
        case integer: { return "int"; }
        case string: { return "string"; }
        case boolean: { return "boolean"; }
        case decimal: { return "float"; }
        default: { return ""; }
    }
}

/*! check if a string is a number
* \param arg The string to check
*/
bool ArgParser::isNumber(std::string arg)
{
    logger.debug("Checking if "+arg+" is a number");
    char* p;
    strtol(arg.c_str(), &p, 10);
    bool output = (*p == 0);
    logger.debug("Result: "+std::to_string(output));
    return output;
}

/*! check if a float is a valid int
* \param num The number to check
*/
bool ArgParser::isInt(float num)
{
    logger.debug("Checking if "+std::to_string(num)+" is an int");
    bool output = ( (int)num == num );
    logger.debug("Result: "+std::to_string(output));
    return output;
}

/*! check if a string arg is of the correct type
* \param val The string to check
* \param type The type that the arg should be
* \return Whether the arg is the correct type
*/
bool ArgParser::checkArgType(std::string val, ArgType type)
{
    bool valid = true;
    switch (type)
    {
        case integer:
        {
            // check if it's a number
            if ( !isNumber(val) )
            {
                valid = false;
            } else
            {
                float num = std::stof(val);
                if ( !isInt(num) )
                {
                    valid = false;
                }
            }
            break;
        }
        case decimal:
        {
            if ( !isNumber(val) )
            {
                valid = false;
            }
            break;
        }
        case boolean:
        {
            if ( !(val == "true" || val == "false") )
            {
                valid = false;
            }
            break;
        }

        // useless but it pleases the compiler
        default: {}
    }

    if (valid)
    {
        logger.debug("Type is good");
    } else
    {
        logger.debug("Type not good");
    }
    
    return valid;
}

/*! Add a ReturnArg to an output vector, 
* checking if the ParserOutputItem already exists
* \param output The vector
* \param arg The ReturnArg to add
*/
void ArgParser::addReturnArgToOutput(std::vector<ParserOutputItem> *output, ReturnArg *arg)
{
    logger.info("Adding return arg to output:\n"+returnArgToString(*arg));
    
    // does the output already have a ParserOutputItem for this?
    ParserOutputItem outputItem;
    bool itemExists = false;
    int elementToRemove;
    // spicy removeItemFromVector
    for (int i=0; i<output->size(); i++)
    {
        auto item = output->at(i);
        if (item.name == arg->name)
        {
            itemExists = true;

            // create a copy of the element and
            // prepare to remove the original
            outputItem = item;
            elementToRemove = i;
        }
    }

    


    if (itemExists)
    {
        logger.debug("Output already has a ParserOutputItem for this");
        // we've got a copy of the element, remove it as
        // the actual one is read-only fsr
        output->erase(output->begin() + elementToRemove);
    } else
    {
        logger.debug("Output doesn't have a ParserOutputItem for this, creating one");
        ParserOutputItem newItem;
        newItem.name = arg->name;
        outputItem = newItem;
    }

    outputItem.argInstances.push_back(*arg);

    // now return the copy of the item to output
    output->push_back(outputItem);
    
}

/*! constructor
* \param argc The argc passed to your program
* \param argv Argv passed to your program as vector of strings
* \param allowedOptionals Pointer to a vector of optionals you allow
* \param allowedPositionals Pointer to a vector of positionals you allow
* \param version The version number of your program, as a string
* \param description The description of your program
*/
ArgParser::ArgParser(int argc, std::vector<std::string> * argv, std::vector<OptionalArg *> *allowedOptionals, std::vector<PositionalArg *> *allowedPositionals, std::string version, std::string *description)
{
    this->argc = argc;
    this->argv = argv;
    this->allowedOptionals = allowedOptionals;
    this->allowedPositionals = allowedPositionals;

    this->version = version;
    this->description = description;

    allowedOptionals->push_back(&helpArg);

    //logger.setSender(argv->at(0)+".ArgParser");
    logger.setSender("ArgParser");
    logger.info(*description + " (version "+version+")");

    logger.debug("ArgParser created");
    
}

//! Print help message and exit
void ArgParser::printHelp()
{
    int spacesInTab = 5;
    logger.info("Generating help");

    helpText += argv->at(0) + " (version " + version + ")\n" + *description+"\n\n";

    // add positional args, if they exist
    if ( allowedPositionals->size() > 0 )
    {
        helpText += "Positional args:\n\n";
        for (auto positional : *allowedPositionals)
        {
            for (int i=0; i<spacesInTab; i++) { helpText += " "; }

            helpText +=positional->name + " : " + positional->description;
            if ( !(argTypeToString(positional->type) == "") )
            {
                helpText += " ("+argTypeToString(positional->type)+")";
            }
            helpText += "\n\n";
        }
    }

    // same for optionals
    if ( allowedOptionals->size() > 0 )
    {
        helpText += "Optional args:\n\n";
        for (auto optional : *allowedOptionals)
        {
            for (int i=0; i<spacesInTab; i++) { helpText += " "; }
            helpText += "-"+optional->shortName + ", --"+optional->longName+" : " + optional->description;
            if ( !(argTypeToString(optional->type) == "") )
            {
                helpText += " ("+argTypeToString(optional->type)+")";
            }
            helpText += "\n\n";
        }
    }

    logger.debug("Help text generated:\n"+helpText);

    std::cout << helpText << "\n";

    logger.info("Exiting");
    exit(0);
}

/*! Entrypoint
* \param output The vector to output to
*/
void ArgParser::ParseArgs(std::vector<ParserOutputItem> *output)
{
    logger.info("Parsing args");

    // check that hard-coded data is fine before we start
    // looking at user input

    logger.debug("Checking for duplicate arg names");

    std::vector<std::string> argNames;

    for (auto optional : *allowedOptionals)
    {
        logger.debug("Checking optional "+optional->longName);
        if ( std::find(argNames.begin(), argNames.end(), optional->longName) != argNames.end() )
        {
            logger.error("Argument name " + optional->longName + " used more than once");
        } else
        {
            logger.debug("Optional "+optional->longName+" OK");
            argNames.push_back(optional->longName);
        }
    }

    // in positionals, also check that it's not storeTrue/False
    for (auto positional : *allowedPositionals)
    {
        logger.debug("Checking positional " + positional->name);

        if (positional->type == storeTrue || positional->type == storeFalse)
        {
            logger.error("Positional arg cannot be of type storeTrue or storeFalse");
        }

        if ( std::find(argNames.begin(), argNames.end(), positional->name) != argNames.end() )
        {
            logger.error("Argument name " + positional->name + " used more than once");
        } else
        {
            logger.debug("Positional "+positional->name+" OK");
            argNames.push_back(positional->name);
        }
    }


    // now parse all args
    std::vector<std::string> positionals;
    ReturnArg nextArg;

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

                if (nextArg.needsParameter)
                {
                    // oops, there should've been a value before this
                    logger.error("Optional arg "+nextArg.name+" needed a parameter");
                }

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
                        }
                    }
                }

                // stuff for all optionals
                if ( validArg )
                {
                    logger.debug("Is a valid arg");
                    
                    nextArg.name = argCopy.longName;
                    nextArg.fmt = optional;

                    if ( argCopy.longName == "help" )
                    {
                        // exits w status 0
                        printHelp();
                    }
                    
                    // does the arg take a parameter ting?
                    if ( !( argCopy.type == storeTrue || argCopy.type == storeFalse ) )
                    {
                        logger.debug("Arg takes a parameter");
                        nextArg.needsParameter = true;
                        nextArg.receivedParameter = false;
                        nextArg.type = argCopy.type;
                    } else
                    {
                        logger.debug("Arg doesn't take a parameter");

                        nextArg.needsParameter = false;

                        // we've already got the name, so now add the val & push it
                        if ( argCopy.type == storeTrue )
                        {
                            nextArg.val = "true";
                        } else
                        {
                            nextArg.val = "false";
                        }

                        addReturnArgToOutput(output, &nextArg);
                    }
                } else
                {
                    // it's not a valid arg
                    logger.error("Unknown option "+arg+" , try --help");
                }


            } else {
                logger.debug("isn't a switch");
                if ( nextArg.needsParameter )
                {
                    logger.debug("is a value for an optional arg");
                    nextArg.val = arg;

                    logger.debug("Checking type");
                    if (!checkArgType(nextArg.val, nextArg.type))
                    {
                        logger.error("Arg "+nextArg.name+" must be the correct type, use --help for more.");
                    }

                    nextArg.needsParameter = false;
                    // name should already be there, so whack in the val & push
                    nextArg.val = arg;
                    nextArg.receivedParameter = true;
                    addReturnArgToOutput(output, &nextArg);

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
    }

    // check if any illegal duplicates exist
    logger.debug("Checking for illegal duplicates");
    for (auto item : *output)
    {
        // only check optionals
        if (item.argInstances.at(0).fmt == optional)
        {
            // is it allowed duplicates?
            OptionalArg *original;
            for (auto optional : *allowedOptionals)
            {
                if (optional->longName == item.argInstances.at(0).name) { original = optional; }
            }

            if (!original->duplicatesAllowed)
            {
                logger.debug(original->longName + " isn't allowed duplicates, checking");
                if (item.argInstances.size() > 1)
                {
                    logger.error("Argument "+original->longName+" cannot be used more than once");
                } else
                {
                    logger.debug("Argument "+original->longName+" didn't have any illegal duplicates");
                }
            }
        }
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
            // check type
            if (!checkArgType(positionals.at(i), allowedPositionals->at(i)->type))
            {
                logger.error("Arg "+allowedPositionals->at(i)->name+" must be the correct type, use --help for more.");
            }

            ReturnArg returnArg
            {
                allowedPositionals->at(i)->name,
                positionals.at(i),
                false, false,
                allowedPositionals->at(i)->type,
                positional
            };
            addReturnArgToOutput(output, &returnArg);
        }
    }
    

    // if the last arg is missing a parameter, we shouldddd catch it here
    if (nextArg.needsParameter && !nextArg.receivedParameter)
    {
        logger.error("Optional arg "+nextArg.name+" needed a parameter");
    }

}