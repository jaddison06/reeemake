#include "Logging.h"

// very very simple logging

// global tingz
std::ofstream logfile;
std::string fname;

// get a tm struct
tm *getTime()
{
    time_t rawTime = time(0);
    return localtime(&rawTime);
}

// take a tm and return a beautified string
std::string formatTime(tm *someTime)
{
    return std::to_string(someTime->tm_year + 1900) + "-" + std::to_string(someTime->tm_mon) + "-" + std::to_string(someTime->tm_mday) + " " + std::to_string(someTime->tm_hour) + ":" + std::to_string(someTime->tm_min) + ":" + std::to_string(someTime->tm_sec);
}


// IMPORTANT:
// call this before you try to instantiate ANY loggers
// or you'll get a really hard to debug segfault
//
// it won't show up at build time
//
// only runtime
void initLogging(std::string *logFolder)
{
    tm *startTime = getTime();

    // create the logs folder, if it already exists this will hopefully just fail silently (?)
    std::filesystem::create_directories(*logFolder);

    fname = *logFolder + formatTime(startTime);

    logfile.open(fname, std::ios::app);
}

// called by both versions of the constructor
// put Logger construction stuff in here
void Logger::setup(std::string someApplication)
{
    
    setSender(someApplication);
}

// constructors
Logger::Logger(std::string someApplication)
{
    setup(someApplication);
}

Logger::Logger()
{
    setup("");
}

// set the sender of the logs
//
// you can also do this via the constructor, I recommend
// inherited classes use setSender but standalones can be implicit
void Logger::setSender(std::string someApplication)
{
    sender = someApplication;
}

// set a unique ID if you're gonna have more than one 
// of the same class
//
// this one can't be done from the constructor. this is probably a design choice
void Logger::setID(std::string someID)
{
    ID = someID;
}

// called by someone, hopefully, when they're finished with the logger
void Logger::destroy()
{
    //logfile.close();
}

// wrappers for various log levels
void Logger::debug(std::string msg)
{
    if (enableDebug)
    {
        writeLog("DEBUG", &msg);
    }
}

void Logger::info(std::string msg)
{
    writeLog("INFO", &msg);
}

void Logger::warning(std::string msg)
{
    writeLog("WARNING", &msg);
}

void Logger::error(std::string msg)
{
    msg = "Error - " + msg;
    warning(msg);
    std::cout << msg << "\n";

    exit(69);
}

// format a log entry
std::string Logger::fmtLog(std::string *level, std::string *msg)
{
    tm *logTime = getTime();

    // make id look slightly prettier
    std::string fmtID = "";
    if (ID != "")
    {
        fmtID = " " + ID;
    }

    return formatTime(logTime) + "   " + *level + " (" + sender + fmtID + ") " + *msg + "\n\n";
}

// write a log entry
//
// although this will probs have a performance impact
// i flush the logfile here to make sure that
// shit doesn't just hang around in the buffer when my code
// inevitably crashes at runtime
void Logger::writeLog(std::string level, std::string *msg)
{    
    std::string output = fmtLog(&level, msg);
    logfile << output;

    logfile.flush();

    if (printLogs)
    {
        std::cout << output << std::endl;
    }
}