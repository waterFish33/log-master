#pragma once
/*⽇志落地类主要负责落地日志消息到目的地
    1.抽象落地基类
    2.派生子类(根据不同的落地方向进行派生)
    3.使用工厂模式进行创建与表示的分离
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <cassert>

#include "./util.hpp"
namespace log_master
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink() {}
        ~LogSink() {}
        virtual void Log(const std::string &data, size_t len) = 0;
    };

    // 标准输出:StdoutSink
    class StdoutLogSink : public LogSink
    {
    public:
        // 将日志写入到标准输出
        void Log(const std::string &data, size_t len) override
        {
            std::cout.write(data.c_str(), len);
        }
    };

    // 固定文件:FileSink
    class FileLogSink : public LogSink
    {
    public:
        // 构造时传入文件名，并打开文件，把文件句柄管理起来
        FileLogSink(const std::string &filepath) : _filepath(filepath)
        {
            std::string path = log_master::Util::File::Path(_filepath);
            log_master::Util::File::CreateDirectory(path);
            _ofs.open(_filepath, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        void Log(const std::string &data, size_t len) override
        {
            _ofs.write(data.c_str(), len);
            assert(_ofs.good());
        }

    private:
        std::string _filepath;
        std::ofstream _ofs;
    };

    // 滚动文件:RollSink(以大小滚动)
    class RollByFileLogSink : public LogSink
    {

    public:
        // 构造时传入文件名，并打开文件，把文件句柄管理起来
        RollByFileLogSink(const std::string &basename, size_t max_fsize) : _basename(basename), _max_fsize(max_fsize)
        {
            // 创新文件所在目录
            log_master::Util::File::CreateDirectory(log_master::Util::File::Path(_basename));
            std::string pathname = CreateNFileName();
            // 打开文件
            _ofs.open(pathname, std::ios::app | std::ios::binary);
            assert(_ofs.is_open());
        }
        // 写入前判断文件大小，超过最大值后切换文件
        void Log(const std::string &data, size_t len) override
        {
            if (_cur_fsize >= _max_fsize)
            {
                _ofs.close(); // 关闭原来打开的文件

                std::string pathname = CreateNFileName();

                _ofs.open(pathname, std::ios::app | std::ios::binary);
                assert(_ofs.is_open());
                _cur_fsize = 0;
            }
            _ofs.write(data.c_str(), len);
            _cur_fsize += len;
            assert(_ofs.good());
        }

    private:
        std::string CreateNFileName()
        {
            time_t tm = log_master::Util::Date::getTime();
            struct tm t;
            localtime_r(&tm, &t);
            std::stringstream filename;

            filename << _basename;
            filename << t.tm_year + 1900;
            filename << t.tm_mon + 1;
            filename << t.tm_mday;
            filename << t.tm_hour;
            filename << t.tm_min;
            filename << t.tm_sec;
            filename << "-";
            filename << _name_count++;
            filename << ".log";
            return filename.str();
        }

    private:
        size_t _name_count = 0;
        std::string _basename; //_filename+拓展文件名(时间)=实际文件名
        size_t _max_fsize;     // 记录文件最大大小，超过后开新文件
        size_t _cur_fsize;     // 记录文件当前大小
        std::ofstream _ofs;
    };

    // 滚动文件:RollSink(以时间段滚动)
    class RollByTimeLogSink : public LogSink
    {
    public:
        enum Gap_Size
        {
            GAP_SECOND=0,
            GAP_MINUTE,
            GAP_HOUR,
            GAP_DAY
        };
        // 构造时传入文件名，并打开文件，把文件句柄管理起来
        RollByTimeLogSink(const std::string &basename,Gap_Size gap_type) : _basename(basename)
        {
            switch (gap_type)
            {
            case Gap_Size::GAP_SECOND:
                _gap_size = 1;
            case Gap_Size::GAP_MINUTE:
                _gap_size = 60;
                break;
            case Gap_Size::GAP_HOUR:
                _gap_size = 3600;
                break;
            case Gap_Size::GAP_DAY:
                _gap_size = 24 * 3600;
                break;
            }
            // 初始化当前时间段
            time_t cur_time = log_master::Util::Date::getTime();
            _cur_gap = cur_time / _gap_size;
            // 创新文件所在目录
            log_master::Util::File::CreateDirectory(log_master::Util::File::Path(_basename));
            std::string pathname = CreateNFileName();
            // 打开文件
            _ofs.open(pathname, std::ios::app | std::ios::binary);
            assert(_ofs.is_open());
        }
        // 写入前判断文件大小，超过最大值后切换文件
        void Log(const std::string &data, size_t len) override
        {
            time_t cur_time = log_master::Util::Date::getTime();
            if (_cur_gap != cur_time / _gap_size)
            {
                _ofs.close(); // 关闭原来打开的文件
                std::string pathname = CreateNFileName();
                _ofs.open(pathname, std::ios::app | std::ios::binary);
                assert(_ofs.is_open());
            }
            _ofs.write(data.c_str(), len);
            assert(_ofs.good());
        }

    private:
        std::string CreateNFileName()
        {
            time_t tm = log_master::Util::Date::getTime();
            struct tm t;
            localtime_r(&tm, &t);
            std::stringstream filename;

            filename << _basename;
            filename << t.tm_year + 1900;
            filename << t.tm_mon + 1;
            filename << t.tm_mday;
            filename << t.tm_hour;
            filename << t.tm_min;
            filename << t.tm_sec;
            filename << ".log";
            return filename.str();
        }

    private:
        std::string _basename; //_filename+拓展文件名(时间)=实际文件名
        size_t _gap_size;      // 时间段大小
        size_t _cur_gap;       // 第几个时间段
        std::ofstream _ofs;
    };

    class LogSinkFactory
    {
    public:
        template <typename Sinktype, typename... Args>
        static LogSink::ptr Create(Args &&...args)
        {
            return std::make_shared<Sinktype>(std::forward<Args>(args)...);
        }
    };
}
