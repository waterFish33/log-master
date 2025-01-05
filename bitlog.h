#ifndef __MY_BITLOG_H__
#define __MY_BITLOG_H__

#include "./logger.hpp"

namespace log_master
{
    // 1.提供获取指定日志器的全局接口（避免用户自己操作单例对象）
    Logger::ptr getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }
    Logger::ptr rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }
// 2.使用宏函数对日志器的接口进行代理（代理模式）
#define debug(fmt, ...) Debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warning(fmt, ...) Warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
// 3.提供宏函数，直接通过默认日志器进行日志的标准输出打印（不用获取日志器）
#define DEBUG(fmt, ...) log_master::rootLogger->debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) log_master::rootLogger->info(fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) log_master::rootLogger->warning(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) log_master::rootLogger->error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) log_master::rootLogger->fatal(fmt, ##__VA_ARGS__)
}

#endif