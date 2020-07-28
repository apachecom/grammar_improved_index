//
// Created by alejandro on 27-11-19.
//

#ifndef RRPAIR_CLOGGER_H
#define RRPAIR_CLOGGER_H

#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <vector>
#include <time.h>
#include <unordered_map>

#define DEBUG_LOG_M 1


using namespace std;
#define LOGGER CLogger::GetLogger()
/**
*   Singleton Logger Class.
*/


class CLogger
{
public:

    typedef std::unordered_map<std::string,uint64_t > Model;

    /**
    *   Funtion to create the instance of logger class.
    *   @return singleton object of Clogger class..
    */
    static CLogger* GetLogger();

    /**
   *   Logs a message
   *   @param sMessage message to be logged.
   */
    void Log(const std::string& sMessage);
    /**
    *   << overloaded function to Logs a message
    *   @param sMessage message to be logged.
    */
    CLogger& operator<<(const string& sMessage);
    /**
    *   Funtion to create the instance of logger class.
    *   @return singleton object of Clogger class..
    */
//    static void write_stats();

    /**model for stats*/
    static Model model;

private:
    /**
    *    Default constructor for the Logger class.
    */
    CLogger();
    /**
    *   copy constructor for the Logger class.
    */
    CLogger(const CLogger&){};             // copy constructor is private
    /**
    *   assignment operator for the Logger class.
    */
    CLogger& operator=(const CLogger&){ return *this; };  // assignment operator is private

    /**
    *   Singleton logger class object pointer.
    **/
    static CLogger* m_pThis;

    /**
   *   Log file name.
   **/
    static const std::string m_sFileName;


    /**
    *   Log file stream object.
    **/
    static ofstream m_Logfile;
};


#endif //RRPAIR_CLOGGER_H
