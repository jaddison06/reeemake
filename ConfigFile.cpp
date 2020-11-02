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
    while (std::getline(stream, line))
    {
        if (isCommand(&line))
        {

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