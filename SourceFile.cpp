#include "SourceFile.h"

void SourceFileSerializationUtil::SerializeSourceFile(SourceFile *sourceFile, fs::path path)
{
    logger.info("Serializing source file "+(std::string)path);
    // if the file already exists, delete it and start afresh
    if (fs::exists(path)) { fs::remove(path); }

    std::ofstream fh(path);

    fh << (std::string)sourceFile->path+"\n";

    // shouldn't implicitly convert this but idk
    // how else to do it. please don't break
    fh << sourceFile->lastBuildTime + "\n";
    fh << "DEPENDS START\n";
    for (auto dependency : sourceFile->dependencies)
    {
        fh << (std::string)dependency + "\n";
    }
    //fh << "DEPENDS END\n";

    fh.close();

}

SourceFile SourceFileSerializationUtil::DeserializeSourceFile(fs::directory_entry *entry)
{
    logger.info("Deserializing source file "+(std::string)entry->path());
    std::ifstream fh(entry->path());
    SourceFile output;
    std::string line;
    int i=0;
    bool insideDependencies = false;
    while (std::getline(fh, line))
    {
        logger.debug("Got line "+line);
        if (line == "DEPENDS START")
        {
            insideDependencies = true;
        } else if (!insideDependencies)
        {
            if (i==0)
            {
                output.path = fs::path(line);
            } else if (i==1)
            {
                time_t lastBuildTime(stoi(line));
                output.lastBuildTime = lastBuildTime;
            }
        } else
        {
            output.dependencies.push_back(fs::path(line));
        }
    }
}