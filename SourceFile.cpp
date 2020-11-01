#include "SourceFile.h"

std::string SourceFileSerializationUtil::SerializeSourceFile(SourceFile *sourceFile)
{
    logger.debug("Generating serialized text");

    std::string output;

    logger.debug("Adding path");
    output += (std::string)sourceFile->path+"\n";

    // fsr this segfaults if you do it inline,
    // you have to get the val first and then stream it in
    logger.debug("Getting lastBuildTime");
    int lastBuildTime = sourceFile->lastBuildTime;
    logger.debug("Adding lastBuildTime");
    output += std::to_string(lastBuildTime) + "\n";

    logger.debug("Adding dependencies");
    output += "DEPENDS START\n";
    for (auto dependency : sourceFile->dependencies)
    {
        logger.debug("Adding dependency "+(std::string)dependency.path);
        output += SerializeSourceFile(&dependency);
    }

    output += "DEPENDS END\n";

    return output;

}

SourceFile SourceFileSerializationUtil::DeserializeSourceFile(std::string *file, int level)
{
    logger.info("Deserializing source file:\n"+*file+"\n(at level "+std::to_string(level)+")");
    std::istringstream stream(*file);
    SourceFile output;
    std::string line;
    int i=0;
    bool insideDependencies = false;
    bool insideADependency = false;
    std::string currentDependency;
    while (std::getline(stream, line))
    {
        logger.debug("Got line "+line);
        
        /*
        if (line == "DEPENDS START")
        {
            logger.debug("At start of dependencies");
            insideDependencies = true;
        } else if (!insideDependencies)
        {
            logger.debug("Outside of dependencies");
            if (i==0)
            {
                logger.debug("Line is path");
                output.path = fs::path(line);
            } else if (i==1)
            {
                logger.debug("Line is lastBuildTime");
                time_t lastBuildTime(stoi(line));
                output.lastBuildTime = lastBuildTime;
            }
            i++;
        } else
        {
            logger.debug("Currently inside dependencies");
            if (line == "DEPENDS END")
            {
                if (insideADependency)
                {
                    logger.debug("At end of current dependency");
                    logger.debug("Previous dependency should be intact:\n"+currentDependency+"\nDeserializing it");
                    output.dependencies.push_back(DeserializeSourceFile(&currentDependency, level+1));
                    insideADependency = false;
                } else
                {
                    logger.debug("At end of dependencies");
                    insideDependencies = false;
                }
            } else {
                if (!insideADependency)
                {
                    // we just got to the end of a dependency,
                    // start a new one
                    logger.debug("At start of next dependency");
                    currentDependency = "";
                    insideADependency = true;
                } else {
                    logger.debug("Inside a dependency");
                    currentDependency += line + "\n";
                }
                
            }
        }
        */

       // updated logic since SourceFile.dependencies is now std::vector<SourceFile>
       // so we get nesting
       if (!insideDependencies)
       {
           logger.debug("Not inside dependencies");
           if (line == "DEPENDS START")
            {
                logger.debug("At start of dependencies");
                insideDependencies = true;
            } else
            {
                logger.debug("Outside of dependencies");
                if (i==0)
                {
                    logger.debug("Line is path");
                    output.path = fs::path(line);
                } else if (i==1)
                {
                    logger.debug("Line is lastBuildTime");
                    time_t lastBuildTime(stoi(line));
                    output.lastBuildTime = lastBuildTime;
                }
                i++;
            }
        } else
        {
            logger.debug("Currently inside dependencies");
            if (!insideADependency)
            {
                logger.debug("Not inside a dependency");
                if (line == "DEPENDS END")
                {
                    logger.debug("At end of dependencies");
                    insideDependencies = false;
                } else
                {
                    logger.debug("At start of next dependency");
                    currentDependency = line + "\n";
                    insideADependency = true;
                }
            } else
            {
                logger.debug("Inside a dependency");
                currentDependency += line;
                if (line == "DEPENDS END")
                {
                    logger.debug("At end of dependency, whole thing:\n"+currentDependency+"\nDeserializing it");
                    output.dependencies.push_back(DeserializeSourceFile(&currentDependency, level+1));
                    insideADependency = false;
                } else
                {
                    logger.debug("Dependency line");
                    currentDependency += "\n";
                }
            }
        }
    }
    logger.info("Finished deserializing (level "+std::to_string(level)+")");

    return output;
}