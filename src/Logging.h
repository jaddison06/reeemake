#pragma once

#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include <filesystem>

/*! \file
* Header for logging
*/

void initLogging(std::string *);

/*! The Logger is the main entrypoint
* for logging.
*/
class Logger
{
    public:
        Logger(std::string);
        Logger();
        ~Logger();

        void setSender(std::string);
        void setID(std::string);

        void debug(std::string);
        void info(std::string);
        void warning(std::string);
        
        void error(std::string);

        /*! Enable logging of debug level messages.
        * Useful when you want to quickly disable them
        * for your production build.
        */
        bool enableDebug = true;

        //! Print logs to stdout
        bool printLogs = false;
    
    private:
        void setup(std::string);

        //! Sender of logs
        std::string sender;

        //! ID
        std::string ID;

        std::string fmtLog(std::string *, std::string *);
        void writeLog(std::string, std::string *);
};