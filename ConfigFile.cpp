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
                thisCommand.options = line.substr(found, line.length() - found);
            } else
            {
                thisCommand.command = line;
                thisCommand.options = "";
            }
            commands.push_back(thisCommand);
        }
    }

    // map allowedCommands so we can switch it
    //
    // this is vv ugly apparently there's a better solution with
    // lambdas or smth
    // TODO:
    // https://stackoverflow.com/questions/650162/why-the-switch-statement-cannot-be-applied-on-strings
    // https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c/

    std::unordered_map<std::string, int> map;
    for (int i=0; i<allowedCommands.size(); i++)
    {
        map.insert({allowedCommands.at(i), i});
    }

    for (auto cmd : commands)
    {
        if (!map.count(cmd.command))
        {
            logger.error("Command "+cmd.command+" unrecognised");
        }
        switch ( map.at(cmd.command) )
        {
            case 0:
            {
                // depend
                size_t space = cmd.options.find(" ");
                std::string first = cmd.options.substr(0, space);
                std::string second = cmd.options.substr(space, cmd.options.length() - space);
                
            }

            default:
            {
                logger.error(cmd.command+" has not been fully implemented yet, sorry :)");
            }
            
        }
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