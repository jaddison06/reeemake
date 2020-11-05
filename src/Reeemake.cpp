#include "Reeemake.h"

// file paths should not be having a trailing / please

// general TODO: add lots of logging

// TODO: optimise this nugget
std::vector<fs::path> Reeemake::getFilesInDir(fs::path dir)
{
    bool annoyingDir = isAnnoyingDir(dir.string());
    if (!annoyingDir)
    {
        logger.info("Getting all files in dir "+dir.string());
    }
    std::vector<fs::path> output;
    for (auto entry : fs::directory_iterator(dir))
    {
        if ( !annoyingDir )
        {
            logger.debug("Got dir entry "+entry.path().string());
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

// TODO: how the hell does this work on ./extension/.git ??
// blind luck
// something in the implementation probably
bool Reeemake::isAnnoyingDir(std::string dirName)
{
    //logger.debug("Checking if "+dirName+" is an annoying dir");
    const std::vector<std::string> annoyingDirs {
        "./.git",
        "./.reeemake"
    };

    bool output = false;
    for (auto item : annoyingDirs)
    {
        if (dirName.substr(0, item.length()) == item) { output = true; }
    }
    //if (output) { logger.debug("yes"); } else { logger.debug("no"); }
    return output;
}

std::string Reeemake::time_t_to_string(time_t *time)
{
    return std::asctime(std::localtime(time));
}

// spicy itemInVector
bool Reeemake::fileDataExists(fs::path *path, std::vector<SourceFile> *fileData, int *fileDataIndex)
{
    //logger.debug("Checking if file data exists for "+path->string());
    for (int i=0; i<fileData->size(); i++)
    {
        auto file = fileData->at(i);
        //logger.debug("Got item "+std::to_string(i)+" from fileData, path "+file.path.string());
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

bool Reeemake::needToBuild(fs::path *path, std::vector<SourceFile> *fileData, std::optional<SourceFile> sourceFile)
{
    logger.info("Checking whether to build "+path->string());
    int fileDataIndex;

    if ( fileDataExists(path, fileData, &fileDataIndex) || sourceFile )
    {
        logger.debug("Found file data or SourceFile");
        
        if (!sourceFile)
        {
            sourceFile = fileData->at(fileDataIndex);
        }
        // sourceFile is guaranteed to be
        // an initialized std::optional of
        // the correct source file
        //
        // access its contents with the * operator


        if ( hasBeenModified(&(*sourceFile)) )
        {
            // file has changed since it was last built, build it now
            logger.debug("File change detected, adding it to the build list");
            return true;
        } else
        {
            logger.debug("No file change, checking dependencies");
            int modifiedDependencyCount = 0;
            for (auto dependency : (*sourceFile).dependencies)
            {
                logger.debug("Got dependency "+dependency.path.string());
                if (needToBuild(&dependency.path, fileData, dependency))
                {
                    modifiedDependencyCount++;
                    logger.debug("Dependency "+dependency.path.string()+" has changed");
                } else
                {
                    logger.debug("No change to dependency");
                }
            }
            if (modifiedDependencyCount > 0)
            {
                logger.debug(std::to_string(modifiedDependencyCount)+" modified dependencies found, adding this source file to the build list");
                return true;
            }
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
    logger.debug("Getting dependencies for file "+sourceFile->string());
    // atm this just does headers,
    // will spice it up in v1.1
    std::vector<fs::path> dependencies;
    for (auto file : *allFilesInDir)
    {
        //if (!isAnnoyingDir(file.string())) { logger.debug("Got possible dependency "+file.string()); }

        // they have the same stem, dependency is a header file, and they _aren't_the_same_file_
        if ( (file.stem() == sourceFile->stem()) && (file.extension() == ".h" || file.extension() == ".hpp") && (file != *sourceFile) )
        {
            logger.debug("Was a dependency");
            dependencies.push_back(file);
        }
    }

    return dependencies;
}


bool Reeemake::hasBeenModified(SourceFile *sourceFile)
{
    auto lastWriteTimeRaw = fs::last_write_time(sourceFile->path);

    // IMPORTANT:
    // this isn't portable until c++20,
    // until then it won't work on g++>=9 or MSVC
    //
    // see https://en.cppreference.com/w/cpp/filesystem/file_time_type
    //
    // if using g++>=9, don't <3
    time_t lastWriteTime = decltype(lastWriteTimeRaw)::clock::to_time_t(lastWriteTimeRaw);
    
    time_t lastBuildTime = sourceFile->lastBuildTime;

    logger.debug("Last write time "+time_t_to_string(&lastWriteTime)+"\nLast build time "+time_t_to_string(&lastBuildTime));

    return ( difftime(lastWriteTime, lastBuildTime) > 0 );
}

SourceFile Reeemake::genSourceFile(fs::path *file, std::vector<fs::path> *allFilesInDir)
{
    logger.debug("Generating a SourceFile for "+file->string());

    SourceFile newSourceFile
    {
        *file,
        time(0),
        {}
    };

    for (auto dependency : getDependencies(file, allFilesInDir))
    {
        logger.debug("Got dependency "+dependency.string());
        SourceFile dependencyData = genSourceFile(&dependency, allFilesInDir);
        newSourceFile.dependencies.push_back(dependencyData);
    }

    return newSourceFile;
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
            configToUse = configFiles.at(0).string();
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
        logger.warning(configPath.string()+" didn't exist, created it");

    }

    std::vector<fs::path> allFilesInDir;
    std::vector<fs::path> cxxSourceFiles;

    allFilesInDir = getFilesInDir(".");
    std::vector<int> entriesToRemove;

    // spicy removeItemFromVector
    for (int i=0; i<allFilesInDir.size(); i++)
    {
        auto entry = allFilesInDir.at(i);
        if (fs::is_directory(entry))
        {
            bool annoyingDir = isAnnoyingDir(entry.string());
            if ( !annoyingDir)
            {
                logger.debug("Found directory "+entry.string()+", searching it");
            }
            entriesToRemove.push_back(i);
            std::vector<fs::path> newFiles = getFilesInDir(allFilesInDir.at(i));
            allFilesInDir.insert(allFilesInDir.end(), newFiles.begin(), newFiles.end());
        }
    }

    for (auto file: allFilesInDir) {
        if (file.extension() == ".cpp")
        {
            logger.debug("Found c++ source file "+file.string());
            cxxSourceFiles.push_back(file);
        }
    }

    // spicy dependency tracking
    std::vector<fs::path> filesToBuild;


    SourceFileSerializationUtil serializationUtil;
    std::vector<SourceFile> fileData;
    fs::path fileDataPath(configPath.string()+"/fileData");
    if (!fs::exists(fileDataPath)) { fs::create_directories(fileDataPath); }
    for (auto sourceFile : fs::directory_iterator(fileDataPath))
    {
        std::string fileString = readEntireFile(sourceFile.path());
        fileData.push_back(serializationUtil.DeserializeSourceFile(&fileString));
    }

    ConfigOptions config;

    if (!(configToUse == "default"))
    {
        ConfigFileParser configParser;

        // i reckon this will segfault it won't be allowed to
        // write to config
        configParser.ParseConfigFile(fs::path(configToUse), &config);
    }

    // TODO: depends from config

    if (config.manualIncludes) { cxxSourceFiles.clear(); }

    for (auto sourceFile : config.sources)
    {
        if (sourceFile.type == file)
        {
            const std::vector<fs::path> sourceFilesCopy = cxxSourceFiles;
            if (sourceFile.include)
            {
                if (!itemInVector<fs::path>(fs::path(sourceFile.name), &sourceFilesCopy))
                {
                    cxxSourceFiles.push_back(fs::path(sourceFile.name));
                }
            } else
            {
                if (itemInVector<fs::path>(fs::path(sourceFile.name), &sourceFilesCopy))
                {
                    removeItemFromVector(fs::path(sourceFile.name), &cxxSourceFiles);
                }
            }
        } else if (sourceFile.type == group)
        {
            logger.error("Source file groups aren't supported yet, sorry :)");
        }
    }



    for (auto sourceFile : cxxSourceFiles)
    {
        if (needToBuild(&sourceFile, &fileData)) { filesToBuild.push_back(sourceFile); }
    }
    

    // build
    // TODO: actually make this cross-platform

    // compile to objects
    
    fs::path objDir(configPath.string()+"/build");
    fs::create_directories(objDir);

    logger.info("Compiling objects");

    for (auto file : filesToBuild)
    {
        std::string command = config.compiler+" -c "+file.string()+" -I . -Wall -std=c++"+std::to_string(config.cxxStandard)+" -o "+objDir.string()+"/"+file.stem().string()+".o";
        for (auto flag : config.compilerFlags)
        {
            command += " " + flag;
        }
        verboseSystem(command);
    }

    if (!filesToBuild.size() == 0)
    {
        logger.info("Building");
        std::string buildCommand = config.compiler + " ";
        for (auto file : cxxSourceFiles)
        {
            buildCommand += objDir.string() + "/" + file.stem().string() + ".o ";
        }
        buildCommand += "-o ./" + config.output;
        for (auto lib : config.libraries)
        {
            buildCommand += " -l" + lib;
        }
        for (auto flag : config.compilerFlags)
        {
            buildCommand += flag;
        }

        verboseSystem(buildCommand);

        }
    
    // post-build stuff


    for (auto cmd : config.postBuildCommands)
    {
        system(cmd.c_str());
    }


    logger.info("Updating fileData");
    for (auto file : cxxSourceFiles)
    {
        logger.debug("Updating fileData for file "+file.string());
        
        
        SourceFile newFileData = genSourceFile(&file, &allFilesInDir);

        fs::path sourceFilePath(fileDataPath.string() + "/"+file.string()+".dat");

        logger.info("Serializing source file "+sourceFilePath.string());
        // if the file already exists, delete it and start afresh
        if (fs::exists(sourceFilePath))
        {
            fs::remove(sourceFilePath);
            logger.debug("The .dat file already existed, removed it");
        }

        logger.debug("Opening file handle");
        std::ofstream fh(sourceFilePath);
        fh << serializationUtil.SerializeSourceFile(&newFileData);
        fh.close();
    }

}

