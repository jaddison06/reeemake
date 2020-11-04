#pragma once

#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include <filesystem>

void initLogging(std::string *);

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

        bool enableDebug = true;
        bool printLogs = false;
    
    private:
        void setup(std::string);

        std::string sender;
        std::string ID;

        std::string fmtLog(std::string *, std::string *);
        void writeLog(std::string, std::string *);
};