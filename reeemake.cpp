#include "reeemake.h"

// TODO: filename compatibility on Windows?
int main(int argc, char *argv[])
{
    // init some tings
    std::string dataFolder = "./.reeemake/";

    bool folderExisted = true;
    if ( !fs::exists(fs::path(dataFolder)) )
    {
        fs::create_directories(fs::path(dataFolder));
        folderExisted = false;
    }

    std::string logFolder = dataFolder + "logs/";
    initLogging(&logFolder);

    Logger logger("reeemake.main");
    if (!folderExisted)
    {
        logger.warning("./.reeemake folder didn't exist, created it.");
    }

    logger.debug("Initialized");

    // parse da proverbial args

    std::vector<char*> argvVector;
    for (int i=0; i<argc; i++) {
        argvVector.push_back(argv[i]);
    }

    std::vector<OptionalArg *> allowedOptionals;
    std::vector<PositionalArg *> allowedPositionals;

    ArgParser argParser(argc, &argvVector, &allowedOptionals, &allowedPositionals);
    
    std::vector<ReturnArg> parsedArgs;
    argParser.ParseArgs(&parsedArgs);


    // look for reeemake config files

    std::vector<fs::path> configFiles;

    // iterate over all directory entries
    for (auto &entry : fs::directory_iterator("."))
    {
        // only go for non-directories (could be a symlink)
        if ( !fs::is_directory(entry) )
        {
            // check for a .reee extension
            if ( fs::path(entry).extension().string() == ".reee" )
            {
                configFiles.push_back(entry);
            }
        }
    }



}