#include "Reeemake.h"

/*! \file
* Reeemake entrypoint
*/

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


    Reeemake reeemake;
    reeemake.build(argc, argv);

}