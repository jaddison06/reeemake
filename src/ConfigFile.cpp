#include "ConfigFile.h"

/*! \file
* Code for config file parsing
*/

/*! get all lines in file
* \param file The file to read
*/
std::string readEntireFile(fs::path file)
{
    std::stringstream buf;
    std::ifstream fh(file);
    buf << fh.rdbuf();
    fh.close();
    return buf.str();
}

/*! \brief split a string by a delimiter.
*          temperamental, don't trust this one
* \param string string to split
* \param delimiter delimiter to use.
* \return Vector of tokens
*/
std::vector<std::string> ConfigFileParser::splitString(std::string *string, std::string delimiter)
{
    logger.info("Splitting string \""+*string+"\" by delimiter \""+delimiter+"\"");
    std::vector<std::string> tokens;

    auto start = 0U;
    auto end = string->find(delimiter);
    while (end != std::string::npos)
    {
        tokens.push_back(string->substr(start, end - start));
        start = end + delimiter.length();
        end = string->find(delimiter, start);
    }

    tokens.push_back(string->substr(start, end));

    // needs explicit type, otherwise "" is interpreted as a char
    removeItemFromVector<std::string>("", &tokens);

    // the last token will have a newline at the end
    std::string last = tokens.back();
    last = last.substr(0, last.size() - 1);

    logger.debug("Output:");
    for (auto token : tokens) { logger.debug(token); }
    return tokens;
}

// at this point we have a map of allowedCommands,
// and argument count has already been checked
/*! Parse a command from the config file
* \param cmd The command to parse
* \param map The map of commands to numbers
* \param config The config to modify
* \param level Recursion level (internal)
*/
void ConfigFileParser::ParseCommand(command *cmd, std::unordered_map<std::string, int> *map, ConfigOptions *config, int level)
{
    logger.setID(std::to_string(level));
    logger.info("Parsing command "+cmd->command+" at level "+std::to_string(level)+" with "+std::to_string(cmd->options.size())+" options");
    for (auto option : cmd->options) { logger.debug(option); }
    if (!map->count(cmd->command))
        {
            logger.error("Command "+cmd->command+" unrecognised");
        }
        for (auto allowedCommand : allowedCommands)
        {
            if (allowedCommand.name == cmd->command)
            {
                std::optional<int> argCount = allowedCommand.argCount;
                if (argCount && cmd->options.size() != *argCount)
                {
                    logger.error("Command "+allowedCommand.name+" expected "+std::to_string(*argCount)+" args, got "+std::to_string(cmd->options.size()));
                }
            }
        }
        switch ( map->at(cmd->command) )
        {
            case 0:
            {
                // depend
                config->depends.push_back({
                    cmd->options.at(0),
                    cmd->options.at(1),
                    true
                });
                break;
            }
            case 1:
            {
                // no-depend
                config->depends.push_back({
                    cmd->options.at(0),
                    cmd->options.at(1),
                    false
                });
                break;
            }
            case 2:
            {
                // manual-depends
                std::string val = cmd->options.at(0);
                if (val == "enable")
                {
                    config->manualDepends = true;
                } else if (val == "disable")
                {
                    config->manualDepends = false;
                } else
                {
                    logger.error("Command manual-depends needs an argument of \"enable\" or \"disable\"");
                }
                break;
            }
            case 3:
            {
                // manual-includes
                std::string val = cmd->options.at(0);
                if (val == "enable")
                {
                    config->manualIncludes = true;
                } else if (val == "disable")
                {
                    config->manualIncludes = false;
                } else
                {
                    logger.error("Command manual-includes needs an argument of \"enable\" or \"disable\"");
                }
                break;
            }
            case 4:
            {
                // include
                config->sources.push_back({
                    cmd->options.at(0),
                    true,
                    file
                });
                break;
            }
            // not doing group stuff yet
            /*
            case 5:
            {
                // include-group
                config->sources.push_back({
                    cmd->options.at(0),
                    true,
                    group
                });
                break;
            }
            */
            case 6:
            {
                // exclude
                config->sources.push_back({
                    cmd->options.at(0),
                    false,
                    file
                });
                break;
            }
            // see case 5
            /*
            case 7:
            {
                // exclude-group
                config->sources.push_back({
                    cmd->options.at(0),
                    false,
                    group
                });
                break;
            }
            */
            case 8:
            {
                // library
                config->libraries.push_back(cmd->options.at(0));
                break;
            }
            case 9:
            {
                // std
                int standard;
                try
                {
                    standard = stoi(cmd->options.at(0));
                }
                catch (std::invalid_argument &ia)
                {
                    logger.error("Argument of std must be a valid int");
                }
                config->cxxStandard = standard;
                break;
            }
            case 10:
            {
                // comp
                config->compiler = cmd->options.at(0);
                break;
            }
            case 11:
            {
                // output
                config->output = cmd->options.at(0);
                break;
            }
            
            // not doing 12 yet

            case 13:
            {
                // system
                std::string systemCommand;
                for (auto segment : cmd->options)
                {
                    systemCommand += segment + " ";
                }
                config->postBuildCommands.push_back(systemCommand);
            }
            case 14:
            {
                // system-now
                std::string systemCommand;
                for (auto segment : cmd->options)
                {
                    systemCommand += segment + " ";
                }
                system(systemCommand.c_str());
            }

            // not doing 15 yet

            case 16:
            {
                // win
                #ifdef _WIN32
                osSpecificCommand(cmd, map, config, level);
                #endif
                break;
            }
            case 17:
            {
                // mac
                #ifdef __APPLE__
                osSpecificCommand(cmd, map, config, level);
                #endif
                break;
            }
            case 18:
            {
                // linux
                #ifdef __linux__
                osSpecificCommand(cmd, map, config, level);
                #endif
                break;
            }
            case 19:
            {
                // comp-flag
                config->compilerFlags.push_back(cmd->options.at(0));
                break;
            }
            case 20:
            {
                // import
                //
                // this can infinite loop so don't be a lil bitch
                ParseConfigFile(fs::path(cmd->options.at(0)), config, level + 1);
                logger.setID(std::to_string(level));
                break;
            }
            case 21:
            {
                // build
                std::string buildType = cmd->options.at(0);
                if (!itemInVector<std::string>(buildType, &outputOptions))
                {
                    logger.debug("Arg was " + buildType);
                    std::string error_msg = "Argument of command \"build\" must be one of:\n";
                    for (auto option : outputOptions)
                    {
                        error_msg += option + "\n";
                    }

                    logger.error(error_msg);
                };
                break;

            }

            default:
            {
                logger.error(cmd->command+" has not been fully implemented yet, sorry :)");
            }
            
        }
}

/*! \brief Parse a command specific to some OS.
*          Params same as ConfigFileParser::ParseCommand()
*/
void ConfigFileParser::osSpecificCommand(command *cmd, std::unordered_map<std::string, int> *map, ConfigOptions *config, int level)
{
    logger.debug("Parsing OS specific command:");
    logger.debug(cmd->command);
    for(auto opt:cmd->options){logger.debug(opt);}
    // why won't this work?
    std::string newCommand = cmd->options.at(0);
    std::vector<std::string> newOpts { cmd->options.begin() + 1, cmd->options.end() };

    logger.debug("New command: "+newCommand);
    logger.debug("Options:");
    for (auto opt:newOpts) {logger.debug(opt);}

    command newCmd
    {
        newCommand,
        newOpts
    };

    ParseCommand(&newCmd, map, config, level + 1);
}

/*! Entrypoint to parse a config file
* \param file The file to parse
* \param config The config options to modify
* \param level Recursion level (internal)
*/
void ConfigFileParser::ParseConfigFile(fs::path file, ConfigOptions *config, int level)
{
    logger.setID(std::to_string(level));
    logger.info("Parsing config file "+file.string()+" at level "+std::to_string(level));

    // we already checked if the root config file exists, but this might
    // be getting called from an import
    if (!fs::exists(file)) { logger.error("Cannot find config file "+file.string()); }

    std::string entireFile = readEntireFile(file);
    logger.debug("got file:\n"+entireFile);
    std::istringstream stream(entireFile);
    std::string line;
    std::vector<command> commands;
    while (std::getline(stream, line))
    {
        if (isCommand(&line))
        {
            size_t found = line.find(" ");
            command thisCommand;
            if (found != std::string::npos)
            {
                // is a command w/ options
                thisCommand.command = line.substr(0, found);
                std::string options = line.substr(found, line.length() - found);
                thisCommand.options = splitString(&options);
            } else
            {
                thisCommand.command = line;
                thisCommand.options = {};
            }
            commands.push_back(thisCommand);
        }
    }

    logger.debug("all commands & options:");
    for (auto cmd:commands)
    {
        logger.debug(cmd.command);
        for (auto opt:cmd.options)
        {
            logger.debug(opt);
        }
        logger.debug("\n");
    }


    // map allowedCommands so we can switch it
    //
    // we do this here so we only have to do it once
    //
    // this is vv ugly apparently there's a better solution with
    // lambdas or smth
    /*! \todo https://stackoverflow.com/questions/650162/why-the-switch-statement-cannot-be-applied-on-strings
    *         https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c/
    */
    std::unordered_map<std::string, int> map;
    for (int i=0; i<allowedCommands.size(); i++)
    {
        map.insert({allowedCommands.at(i).name, i});
    }
    for (auto cmd : commands)
    {
        ParseCommand(&cmd, &map, config, level);
        logger.setID(std::to_string(level));
    }
}

/*! Check if something is a command
* \param line The line to check
*/
bool ConfigFileParser::isCommand(std::string *line)
{
    logger.debug("Checking if "+*line+" is a command");
    bool output = true;
    if (!isWhitespace(line))
    {
        // substr will crash if the string is too short
        if (line->length() >= CONFIG_COMMENT.length())
        {
            if (line->substr(0, CONFIG_COMMENT.length()) == CONFIG_COMMENT)
            {
                logger.debug("Is a comment");
                output = false;
            }
        }
    } else
    {
        // is whitespace
        output =  false;
    }

    // we could do all the logic on the return line apart from the substr
    // which we'd have to do some spicy logic for, which could
    // get a bit unreadable

    if (output) { logger.debug("Is a command"); } else { logger.debug("Isn't a command"); }
    return output;
}

// what the fuck is this
/*! Check if a string is whitespace
* \param line The string to check
*/
bool ConfigFileParser::isWhitespace(std::string *line)
{
    logger.debug("Checking if \""+*line+"\" is whitespace");
    bool output = true;

    for (char character : *line)
    {
        if ( !isspace(character)) { output = false; }
    }

    if (output) {logger.debug("Was whitespace");} else{logger.debug("Wasn't whitespace");}
    return output;
}
