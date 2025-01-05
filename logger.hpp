#pragma once
/*日志器模块：
    1、抽象日志器基类
    2.派生子类（同步日志器、异步日志器）*/
#define _GUN_SOURCEA
#include "./log_level.hpp"
#include "./logsink.hpp"
#include "./message.hpp"
#include "./format.hpp"
#include "./looper.hpp"

#include <atomic>
#include <mutex>
#include <cstdarg>
#include <vector>
#include <assert.h>
#include <unordered_map>

namespace log_master
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger() {}
        Logger(Log_level::level limit_level,
               Formatter::ptr formatter,
               const std::string &logger_name,
               std::vector<LogSink::ptr> logsinks) : _limit_level(limit_level), _formatter(formatter), _logger_name(logger_name), _logsinks(logsinks.begin(), logsinks.end()) {}
        //    protected:
        const std::string &name()
        {
            return _logger_name;
        }
        /*完成构造日志消息对象并进行格式化，得到格式化后的日志消息字符串，然后落地输出*/
        void Debug(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前的日志是否达到了输出等级
            if (Log_level::DEBUG < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list va;
            va_start(va, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), va);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!" << std::endl;
            }
            // 3.构造LogMsg对象
            Message::LogMsg msg(Log_level::DEBUG, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->Format(ss, msg);
            // 5.进行日志落地
            log(ss.str(), ss.str().size());
        }
        void Info(const std::string &file, const size_t line, const std::string &fmt, ...)
        { // 1.判断当前的日志是否达到了输出等级
            if (Log_level::INFO < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list va;
            va_start(va, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), va);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!" << std::endl;
            }
            // 3.构造LogMsg对象
            Message::LogMsg msg(Log_level::INFO, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->Format(ss, msg);
            // 5.进行日志落地
            log(ss.str(), ss.str().size());
        }
        void Warning(const std::string &file, const size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前的日志是否达到了输出等级
            if (Log_level::WARNING < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list va;
            va_start(va, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), va);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!" << std::endl;
            }
            // 3.构造LogMsg对象
            Message::LogMsg msg(Log_level::WARNING, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->Format(ss, msg);
            // 5.进行日志落地
            log(ss.str(), ss.str().size());
        }

        void Error(const std::string &file, const size_t line, const std::string &fmt, ...)
        { // 1.判断当前的日志是否达到了输出等级
            if (Log_level::ERROR < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list va;
            va_start(va, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), va);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!" << std::endl;
            }
            // 3.构造LogMsg对象
            Message::LogMsg msg(Log_level::ERROR, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->Format(ss, msg);
            // 5.进行日志落地
            log(ss.str(), ss.str().size());
        }
        void Fatal(const std::string &file, const size_t line, const std::string &fmt, ...)
        { // 1.判断当前的日志是否达到了输出等级
            if (Log_level::FATAL < _limit_level)
            {
                return;
            }
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list va;
            va_start(va, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), va);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!" << std::endl;
            }
            // 3.构造LogMsg对象
            Message::LogMsg msg(Log_level::FATAL, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->Format(ss, msg);
            // 5.进行日志落地
            log(ss.str(), ss.str().size());
        }

    protected:
        /*抽象接口完成实际的落地输出--不同的日志器有不同的实际落地方式*/
        virtual void log(const std::string &data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::atomic<Log_level::level> _limit_level;
        Formatter::ptr _formatter;
        std::string _logger_name;
        std::vector<LogSink::ptr> _logsinks;
    };
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(Log_level::level limit_level,
                   Formatter::ptr formatter,
                   const std::string &logger_name,
                   std::vector<LogSink::ptr> &logsinks) : Logger(limit_level, formatter, logger_name, logsinks) {}

    protected:
        void log(const std::string &data, size_t len) override
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_logsinks.empty())
            {
                return;
            }
            for (auto &e : _logsinks)
            {
                e->Log(data, len);
            }
        }
    };
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(Log_level::level limit_level,
                    Formatter::ptr formatter,
                    const std::string &logger_name,
                    std::vector<LogSink::ptr> &logsinks,
                    AsyncLooper::AsyncType looper_type) : Logger(limit_level, formatter, logger_name, logsinks),
                                                          _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), looper_type)) {}
        // 将日志写入缓冲区
        void log(const std::string &data, size_t len) override
        {

            _looper->push(data, len);
        }
        // 实际落地函数
        void realLog(Buffer &buffer)
        {

            if (_logsinks.empty())
            {
                return;
            }
            if (!_logsinks.empty())
            {
                for (auto &e : _logsinks)
                {
                    e->Log(buffer.begin(), buffer.readAbleSize());
                }
            }
        }

    private:
        AsyncLooper::ptr _looper;
    };
    /*使用建造者模式建造日志器，简化用户的操作*/
    enum LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };
    // 1、抽象一个日志器建造者类（完成日志器对象所需零件的构建&日志器的构建）
    //     1.设置日志器类型
    //     2.将不同的日志器创建放到同一个日志器建造者类中完成
    class LoggerBuilder
    {
    public:
        LoggerBuilder() : _logger_type(LoggerType::LOGGER_SYNC), _limit_level(Log_level::DEBUG), _looper_type(AsyncLooper::AsyncType::ASYNC_SAFE) {}

        void buildLoggerType(LoggerType type) { _logger_type = type; }
        void buileEnableUnSafeAsync() { _looper_type = AsyncLooper::AsyncType::ASYNC_NOSAFE; }
        void buildLoggerName(const std::string &logger_name) { _logger_name = logger_name; }
        void buildLoggerFormatter(const std::string &pattern = "")
        {
            if (pattern.empty())
            {
                _formater = std::make_shared<Formatter>();
                return;
            }
            _formater = std::make_shared<Formatter>(pattern);
        }
        void buildLoggerLevel(Log_level::level limit_level) { _limit_level = limit_level; }
        template <typename SinkType, typename... Args>
        void buildLoggerSinks(Args &&...args)
        {
            _logsinks.push_back(LogSinkFactory::Create<SinkType>(std::forward<Args>(args)...));
        }
        virtual Logger::ptr build() = 0;

    protected:
        AsyncLooper::AsyncType _looper_type;
        Log_level::level _limit_level;
        Formatter::ptr _formater;
        std::string _logger_name;
        std::vector<LogSink::ptr> _logsinks;
        LoggerType _logger_type;
    };
    // 2、派生出具体的建造者类--局部日志器的建造者 | 全局日志器的建造者（后面添加全局单例管理器，将日志器添加到全局管理器）
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); // 日志器名称不可为空
            if (_formater.get() == nullptr)
            {
                _formater = std::make_shared<Formatter>();
            }
            if (_logsinks.empty())
            {
                _logsinks.push_back(LogSinkFactory::Create<StdoutLogSink>());
            }
            if (_logger_type == LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_limit_level, _formater, _logger_name, _logsinks, _looper_type);
            }
            return std::make_shared<SyncLogger>(_limit_level, _formater, _logger_name, _logsinks);
        }
    };

    // 全局单例管理器
    class LoggerManager
    {
    public:
        static LoggerManager &getInstance()
        {
            static LoggerManager eton;
            return eton;
        }
        bool hasLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            auto it = _loggers.find(name);
            if (it == _loggers.end())
            {
                return false;
            }
            return true;
        }

        void addLogger(Logger::ptr &logger)
        {
            if (hasLogger(logger->name()))
            {
                return;
            }
            std::unique_lock<std::mutex> lock(_mutex);
            
            _loggers.insert({logger->name(), logger});
        }
        Logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            auto it = _loggers.find(name);
            if (it == _loggers.end())
            {
                return Logger::ptr();
            }
            return it->second;
        }
        Logger::ptr rootLogger()
        {
            return _root_logger;
        }

    private:
        LoggerManager()
        {
            std::unique_ptr<log_master::LoggerBuilder> builder(new log_master::LocalLoggerBuilder());
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            _loggers.insert({"root", _root_logger});
        }

    private:
        std::mutex _mutex;
        Logger::ptr _root_logger; // 默认日志器
        std::unordered_map<std::string, Logger::ptr> _loggers;
    };
    // 全局日志器的建造者
    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); // 日志器名称不可为空
            if (_formater.get() == nullptr)
            {
                _formater = std::make_shared<Formatter>();
            }
            if (_logsinks.empty())
            {
                _logsinks.push_back(LogSinkFactory::Create<StdoutLogSink>());
            }
            Logger::ptr logger;
            if (_logger_type == LOGGER_ASYNC)
            {
                logger = std::make_shared<AsyncLogger>(_limit_level, _formater, _logger_name, _logsinks, _looper_type);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_limit_level, _formater, _logger_name, _logsinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
}
