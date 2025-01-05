#pragma once
/*
    枚举日志等级
    提供把日志等级转换为相应字符串
*/
#include <iostream>
namespace log_master
{
    class Log_level
    {

    public:
        enum level
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            FATAL,
            OFF
        };
        static std::string ToString(Log_level::level level)
        {
            switch (level)
            {
            case level::DEBUG:
                return "DEBUG";
            case level::INFO:
                return "INFO";
            case level::WARNING:
                return "WARNING";
            case level::ERROR:
                return "ERROR";
            case level::FATAL:
                return "FATAL";
            case level::OFF:
                return "OFF";
            }
            return "UNKNOW";
        }
    };
}