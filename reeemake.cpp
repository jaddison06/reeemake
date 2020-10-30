#include "reeemake.h"


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

    Logger logger((std::string)argv[0] + ".main");
    if (!folderExisted)
    {
        logger.warning("./.reeemake folder didn't exist, created it.");
    }

    logger.debug("Initialized");

    // parse da proverbial args

    std::vector<std::string> argvVector;
    for (int i=0; i<argc; i++) {
        argvVector.push_back((std::string)argv[i]);
    }

    
    // config file to use
    OptionalArg conf
    {
        "c",
        "conf",
        "Configuration file to read from",
        string
    };


    std::vector<OptionalArg *> allowedOptionals {&conf};
    std::vector<PositionalArg *> allowedPositionals {};

    std::string description = DESCRIPTION;
    ArgParser argParser(argc, &argvVector, &allowedOptionals, &allowedPositionals, VERSION, &description);
    
    std::vector<ReturnArg> parsedArgs;
    argParser.ParseArgs(&parsedArgs);

    /*
    for (auto arg : parsedArgs)
    {
        std::cout << returnArgToString(arg) << "\n\n";
    }
    */

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

    std::string configToUse = "";

    // figure out what we're gonna do

    // were we given the conf arg?
    bool confArgSupplied = false;
    ReturnArg confArg;
    for ( auto arg : parsedArgs )
    {
        if ( arg.name == "conf" )
        {
            confArgSupplied = true;
            confArg = arg;
            break;
        }
    }

    if ( confArgSupplied )
    {
        logger.info("Config arg was supplied, using it");
        configToUse = confArg.val;
    } else
    {
        logger.info("Config arg wasn't supplied");
        if ( configFiles.size() == 0 )
        {
            logger.info("No conf files in dir, using default config");
        } else if ( configFiles.size() == 1 )
        {
            configToUse = configFiles.at(0);
            logger.info("Only 1 conf file in the dir ("+configToUse+"), using it");
        } else
        {
            logger.error("More than one config file present - please specify one with the --conf option");
        }
    }





}