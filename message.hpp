#pragma once 

#include <iostream>
#include <thread>

#include "./log_level.hpp"
#include "./util.hpp"

namespace log_master
{
    class Message
    {
        public:
        struct LogMsg
        {
            size_t _line;                        // 行号
            time_t _ctime;                       // 时间
            std::thread::id _tid;                // 线程ID
            std::string _name;                   // 日志器名称
            std::string _file;                   // 文件名
            std::string _payload;                // 日志消息
            log_master::Log_level::level _level; // 日志等级

            LogMsg(log_master::Log_level::level level, size_t line, std::string file, std::string name, std::string payload) : _level(level), _line(line), _name(name), _tid(std::this_thread::get_id()), _ctime(log_master::Util::Date::getTime()), _file(file), _payload(payload) {}
        };
    };
}