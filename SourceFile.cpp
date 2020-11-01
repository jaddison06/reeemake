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
    output += lastBuildTime + "\n";

    logger.debug("Adding dependencies");
    output += "DEPENDS START\n";
    for (auto dependency : sourceFile->dependencies)
    {
        logger.debug("Adding dependency "+(std::string)dependency.path);
        output += SerializeSourceFile(&dependency) + "\n";
    }

    output += "DEPENDS END\n";

}

SourceFile SourceFileSerializationUtil::DeserializeSourceFile(std::string *file)
{
    logger.info("Deserializing source file "+*file);
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
        
        if (line == "DEPENDS START")
        {
            // line is start of dependencies
            insideDependencies = true;
        } else if (!insideDependencies)
        {
            if (i==0)
            {
                // line is path
                output.path = fs::path(line);
            } else if (i==1)
            {
                // line is lastBuildTime
                time_t lastBuildTime(stoi(line));
                output.lastBuildTime = lastBuildTime;
            }
            i++;
        } else
        {
            if (line == "DEPENDS END")
            {
                // line is end of dependencies
                insideDependencies = false;
            } else {
                if (!insideADependency)
                {
                    // we just got to the end of a dependency,
                    // write it and start a new one
                    output.dependencies.push_back(DeserializeSourceFile(&currentDependency));
                    currentDependency = "";
                    insideADependency = true;
                } else {
                    // we're inside a dependency
                    currentDependency += line + "\n";
                    if (line == "END DEPENDS")
                    {
                        // end of the dependency
                        insideADependency = false;
                    }
                }
                
            }
        }
    }

    return output;
}