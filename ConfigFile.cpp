#include "ConfigFile.h"

// get all lines in file
std::string readEntireFile(fs::path file)
{
    std::stringstream buf;
    std::ifstream fh(file);
    buf << fh.rdbuf();
    fh.close();
    return buf.str();
}

std::vector<std::string> ConfigFileParser::splitString(std::string * string, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream stream;
    while (std::getline(stream, token, delimiter)) { tokens.push_back(token); };
    if (tokens.size() == 0) { tokens.push_back(*string); }

    return tokens;
}

// at this point we have a map of allowedCommands,
// and argument count has already been checked
void ConfigFileParser::ParseCommand(command *cmd, std::unordered_map<std::string, int> *map, ConfigOptions *config)
{

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
        // TODO: add breaks
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
                // bin-name
                config->binName = cmd->options.at(0);
                break;
            }
            
            // not doing 12-15 yet

            case 16:
            {
                // win
                #ifdef _WIN32
                ParseCommand(cmd, map, config);
                #endif
                break;
            }
            case 17:
            {
                // mac
                #ifdef __APPLE__
                ParseCommand(cmd, map, config);
                #endif
                break;
            }
            case 18:
            {
                // linux
                #ifdef __linux__
                ParseCommand(cmd, map, config);
                #endif
                break;
            }
            case 19:
            {
                // comp-flag
                config->compilerFlags.push_back(cmd->options.at(0));
                break;
            }

            default:
            {
                logger.error(cmd->command+" has not been fully implemented yet, sorry :)");
            }
            
        }
}

void ConfigFileParser::ParseConfigFile(fs::path file, ConfigOptions *config)
{
    std::istringstream stream(file);
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


    // map allowedCommands so we can switch it
    //
    // we do this here so we only have to do it once
    //
    // this is vv ugly apparently there's a better solution with
    // lambdas or smth
    // TODO:
    // https://stackoverflow.com/questions/650162/why-the-switch-statement-cannot-be-applied-on-strings
    // https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c/
    std::unordered_map<std::string, int> map;
    for (int i=0; i<allowedCommands.size(); i++)
    {
        map.insert({allowedCommands.at(i).name, i});
    }
    for (auto cmd : commands)
    {
        ParseCommand(&cmd, &map, config);
    }
}

bool ConfigFileParser::isCommand(std::string *line)
{
    if (!isWhitespace(line))
    {
        // substr will crash if the string is too short
        if (line->length() >= 2)
        {
            if (line->substr(0, 2) == "//")
            {
                // is a comment
                return false;
            }
        }
    } else
    {
        // is whitespace
        return false;
    }

    // we could do all the logic on the return line apart from the substr
    // which we'd have to do some spicy logic for, which could
    // get a bit unreadable.
    return true;
}

bool ConfigFileParser::isWhitespace(std::string *line)
{
    std::vector<std::string> whitespaceCharacters
    {
        " ",
        "   "
    };
    for (char character : *line)
    {
        if ( !itemInVector(std::to_string(character), &whitespaceCharacters) )
        {
            return true;
        }
    }

    return false;
}