#include "Reeemake.h"

// file paths should not be having a trailing / please

// general TODO: add lots of logging

// TODO: optimise this nugget
std::vector<fs::path> Reeemake::getFilesInDir(fs::path dir)
{
    bool annoyingDir = isAnnoyingDir(dir);
    if (!annoyingDir)
    {
        logger.info("Getting all files in dir "+(std::string)dir);
    }
    std::vector<fs::path> output;
    for (auto entry : fs::directory_iterator(dir))
    {
        if ( !annoyingDir )
        {
            logger.debug("Got dir entry "+(std::string)(fs::path)entry);
        }
        output.push_back(entry);
    }
    return output;
}

Reeemake::Reeemake(char *argv[])
{
    logger.setSender((std::string)argv[0] + ".Reeemake");
    logger.info("Reeemake initialized");
}

void Reeemake::verboseSystem(std::string cmd)
{
    std::cout << cmd << std::endl;
    logger.info("Running system command "+cmd);
    system(cmd.c_str());
}

bool Reeemake::isAnnoyingDir(std::string dirName)
{
    std::vector<std::string> annoyingDirs {
        "./.git",
        "./.reeemake"
    };

    for (auto dir : annoyingDirs)
    {
        if (dirName.substr(0, dir.length()) == dir) { return true; }
    }

    return false;
}

std::string Reeemake::time_t_to_string(time_t *time)
{
    return std::asctime(std::localtime(time));
}

bool Reeemake::fileDataExists(fs::path *path, std::vector<SourceFile> *fileData, int *fileDataIndex)
{
    //logger.debug("Checking if file data exists for "+(std::string)*path);
    for (int i=0; i<fileData->size(); i++)
    {
        auto file = fileData->at(i);
        //logger.debug("Got item "+std::to_string(i)+" from fileData, path "+(std::string)file.path);
        if (file.path == *path)
        {
            //logger.debug("It's A Match!");
            *fileDataIndex = i;
            return true;
        } else
        {
            //logger.debug("no match ):");
        }
    }
    return false;
}

void Reeemake::parseArgs(int argc, char *argv[])
{
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
        string,
    };

    OptionalArg test1
    {
        "b",
        "bonk",
        "bonk dat ting",
        integer,
        true
    };

    PositionalArg test2
    {
        "conk",
        "conk dat ting",
        decimal
    };


    std::vector<OptionalArg *> allowedOptionals {&conf};
    std::vector<PositionalArg *> allowedPositionals {};

    std::string description = DESCRIPTION;
    ArgParser argParser(argc, &argvVector, &allowedOptionals, &allowedPositionals, VERSION, &description);
    
    std::vector<ParserOutputItem> parsedArgs;
    argParser.ParseArgs(&parsedArgs);

    /*
    for (auto item : parsedArgs)
    {
        std::cout << item.name << "\n";
        for (auto arg : item.argInstances)
        {
            std::cout << returnArgToString(arg) << "\n";
        }
        std::cout << "\n\n";
    }
    */

}

bool Reeemake::needToBuild(fs::path *sourceFile, std::vector<SourceFile> *fileData)
{
    logger.info("Checking whether to build "+(std::string)*sourceFile);
    int fileDataIndex;

    if ( fileDataExists(sourceFile, fileData, &fileDataIndex) )
    {
        logger.debug("Found file data");
        auto lastWriteTimeRaw = fs::last_write_time(*sourceFile);

        // IMPORTANT:
        // this isn't portable until c++20,
        // until then it won't work on g++>=9 or MSVC
        //
        // see https://en.cppreference.com/w/cpp/filesystem/file_time_type
        time_t lastWriteTime = decltype(lastWriteTimeRaw)::clock::to_time_t(lastWriteTimeRaw);
        time_t lastBuildTime = fileData->at(fileDataIndex).lastBuildTime;

        logger.debug("Last write time "+time_t_to_string(&lastWriteTime)+"\nLast build time "+time_t_to_string(&lastBuildTime));

        if (difftime(lastWriteTime, lastBuildTime) > 0)
        {
            // file has changed since it was last built, build it now
            logger.debug("File change detected, adding it to the build list");
            return true;
        } else
        {
            logger.debug("File hasn't been modified");
            return false;
        }
    } else
    {
        // this source file is new to us
        logger.debug("This source file is new, adding it to the build list");
        return true;
    }
}

std::vector<fs::path> Reeemake::getDependencies(fs::path *sourceFile, std::vector<fs::path> *allFilesInDir)
{

}

bool Reeemake::hasBeenModified(SourceFile *sourceFile)
{

}

void Reeemake::build(int argc, char *argv[])
{
    parseArgs(argc, argv);

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
                // very incorrect type but um ok?
                // pretty sure i already tested this and it
                // works fine...
                configFiles.push_back(entry);
            }
        }
    }

    std::string configToUse = "default";

    // figure out what we're gonna do

    // were we given the conf arg?
    bool confArgSupplied = false;
    ReturnArg confArg;
    for ( auto arg : parsedArgs )
    {
        if ( arg.name == "conf" && arg.argInstances.size() > 0 )
        {
            confArgSupplied = true;
            confArg = arg.argInstances.at(0);
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

    logger.info("Using config "+configToUse);

    // pre-build stuff


    fs::path configPath("./.reeemake/" + configToUse);
    if ( !(fs::exists(configPath) && fs::is_directory(configPath)) )
    {
        fs::create_directories(configPath);
        logger.warning((std::string)configPath+" didn't exist, created it");

    }

    std::vector<fs::path> allFilesInDir;
    std::vector<fs::path> cxxSourceFiles;

    allFilesInDir = getFilesInDir(".");
    std::vector<int> entriesToRemove;
    for (int i=0; i<allFilesInDir.size(); i++)
    {
        auto entry = allFilesInDir.at(i);
        if (fs::is_directory(entry))
        {
            bool annoyingDir = isAnnoyingDir((std::string)entry);
            if ( !annoyingDir)
            {
                logger.debug("Found directory "+(std::string)entry+", searching it");
            }
            entriesToRemove.push_back(i);
            std::vector<fs::path> newFiles = getFilesInDir(allFilesInDir.at(i));
            allFilesInDir.insert(allFilesInDir.end(), newFiles.begin(), newFiles.end());
        }
    }

    for (auto file: allFilesInDir) {
        if (file.extension() == ".cpp")
        {
            logger.debug("Found c++ source file "+(std::string)file);
            cxxSourceFiles.push_back(file);
        }
    }

    // spicy dependency tracking
    std::vector<fs::path> filesToBuild;


    SourceFileSerializationUtil serializationUtil;
    std::vector<SourceFile> fileData;
    fs::path fileDataPath((std::string)configPath+"/fileData");
    if (!fs::exists(fileDataPath)) { fs::create_directories(fileDataPath); }
    for (auto sourceFile : fs::directory_iterator(fileDataPath))
    {
        fileData.push_back(serializationUtil.DeserializeSourceFile(&sourceFile));
    }

    for (auto sourceFile : cxxSourceFiles)
    {
        if (needToBuild(&sourceFile, &fileData)) { filesToBuild.push_back(sourceFile); }
    }
    

    // actual build
    // TODO: actually make this cross-platform

    std::string COMPILER = "g++-8";
    std::string BIN_NAME = "reeemake";
    //DEBUG TODO: get rid of -Wno-return-type once the spicy functions exist
    std::vector<std::string> COMPILER_FLAGS {"-Wno-sign-compare", "-Wno-return-type"};
    std::vector<std::string> LIBRARIES {"stdc++fs"};

    // compile to objects
    
    fs::path objDir((std::string)configPath+"/build");
    fs::create_directories(objDir);

    logger.info("Compiling objects");

    for (auto file : filesToBuild)
    {
        std::string command = COMPILER+" -c "+(std::string)file+" -I . -Wall -std=c++17 -o "+(std::string)objDir+"/"+(std::string)file.stem()+".o";
        for (auto flag : COMPILER_FLAGS)
        {
            command += " " + flag;
        }
        verboseSystem(command);
    }

    if (!filesToBuild.size() == 0)
    {
        logger.info("Building");
        std::string buildCommand = COMPILER + " ";
        for (auto file : cxxSourceFiles)
        {
            buildCommand += (std::string)objDir + "/" + (std::string)file.stem() + ".o ";
        }
        buildCommand += "-o ./" + BIN_NAME;
        for (auto lib : LIBRARIES)
        {
            buildCommand += " -l" + lib;
        }
        verboseSystem(buildCommand);

        // cleanup
        logger.info("Updating fileData");
        for (auto file : cxxSourceFiles)
        {
            logger.debug("Updating fileData for file "+(std::string)file);
            
            SourceFile newFileData
            {
                file,
                time(0),
                getDependencies(&file, &allFilesInDir)
            };

            
            serializationUtil.SerializeSourceFile(&newFileData, fs::path((std::string)fileDataPath + "/"+(std::string)file+".dat"));
        }
    }

}
