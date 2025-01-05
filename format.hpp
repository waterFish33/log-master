#pragma once
/*⽇志格式化（format）类主要负责格式化日志消息*/
/*  MsgFormatltem:表示要从LogMsg中取出有效日志数据
    LevelFormatItem:表示要从LogMsg中取出日志等级
    NameFormatItem:表示要从LogMsg中取出日志器名称
    ThreadFormatItem:表示要从LogMsg中取出线程ID
    TimeFormatItem:表示要从LogMsg中取出时间戳并按照指定格式进行格式化
    CFileFormatItem:表示要从LogMsg中取出源码所在文件名
    CLineFormatItem:表示要从LogMsg中取出源码所在行号
    TabFormatItem:表示⼀个制表符缩进
    NLineFormatItem:表示⼀个换行
    OtherFormatItem:表示非格式化的原始字符串*/
#include <iostream>
#include <time.h>
#include <cassert>
#include <vector>
#include <sstream>

#include "./message.hpp"

namespace log_master
{
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const log_master::Message::LogMsg &Msg) = 0;
    };

    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << Msg._payload;
        }
    };
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << log_master::Log_level::ToString(Msg._level);
        }
    };
    class ThreadFormatItem : public FormatItem
    {
    public:
       void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << Msg._tid;
        }
    };
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &tf = "[%H:%M:%S]") : _tf(tf) {}
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            struct tm t;
            localtime_r(&Msg._ctime, &t);
            char tmp[32];
            strftime(tmp, 32, _tf.c_str(), &t);
            out << tmp;
        }

    private:
        std::string _tf;
    };
    class NameFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << Msg._name;
        }
    };
    class CFileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << Msg._file;
        }
    };
    class CLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << Msg._line;
        }
    };
    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << "\t";
        }
    };
    class NLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << "\n";
        }
    };
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str) : _str(str) {}
        void format(std::ostream &out, const log_master::Message::LogMsg &Msg) override
        {
            out << _str;
        }

    private:
        std::string _str;
    };

    /*  %d 日期
        %T 缩进
        %t 线程id
        %p 日志级别
        %c 日志器名称
        %f 文件名
        %l 行号
        %m 日志消息
        %n 换行*/
    class Formatter
    {
    public:
        using ptr=std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}] [%t] [%p] [%c] [%f:%l]%T%m%n") : _pattern(pattern)
        {
            assert(ParsePattern());
        }
        // 对Msg进行格式化
        void Format(std::ostream &out, log_master::Message::LogMsg &Msg)
        {

            for (auto &e : _items)
            {
                 e->format(out, Msg);
            }
        }
        std::string Format(log_master::Message::LogMsg &Msg)
        {
            std::stringstream ss;
            Format(ss, Msg);
            return ss.str();
        }
        // 对格式化规则字符串进行解析
        private:
        bool ParsePattern()
        {
            size_t pos = 0;
            size_t psize = _pattern.size();
            std::string key, val;

            while (pos < psize)
            {
                // 处理非格式化字符
                if (_pattern[pos] != '%')
                {
                    val += _pattern[pos];
                    pos++;
                    continue;
                }
                // 两个%%相当于一个非格式化字符%
                if (pos + 1 < psize && _pattern[pos + 1] == '%')
                {
                    val += '%';
                    pos += 2;
                    continue;
                }
                // 处理格式化字符串前，先处理非格式化字符串
                if (val.empty() == false)
                {
                    _items.push_back(CreateItem(key, val));
                    val.clear();
                }

                // 处理格式化字符串%d{%H:%M:%S}%T%S%Y
                pos++;
                if(pos==psize){
                    std::cout<<"格式化字符串错误"<<std::endl;
                }
                if (pos < psize){
                 key = _pattern[pos];

                }
                pos++;

                // 子格式
                if (pos < psize && _pattern[pos] == '{')
                {
                    pos++;
                    while (pos < psize && _pattern[pos] != '}')
                    {
                        val += _pattern[pos];
                        pos++;
                    }
                    if (pos == psize)
                    {
                        std::cout<<"子规则{}匹配错误"<<std::endl;
                        return false;
                    }
                    pos++;
                }
                    _items.push_back(CreateItem(key,val));
                    key.clear();
                    val.clear();
                
            }
            return true;
        }

    private:
        // 根据不同的格式化字符创建不同的格式化子对象
        std::shared_ptr<FormatItem> CreateItem(const std::string &key, const std::string &val)
        {
            if (key == "d")
            {
               return std::make_shared<TimeFormatItem>(val);
            }
            if (key == "T")
            {
              return  std::make_shared<TabFormatItem>();
            }
            if (key == "t")
            {
               return std::make_shared<ThreadFormatItem>();
            }
            if (key == "p")
            {
               return std::make_shared<LevelFormatItem>();
            }
            if (key == "c")
            {
               return std::make_shared<NameFormatItem>();
            }
            if (key == "f")
            {
               return std::make_shared<CFileFormatItem>();
            }
            if (key == "l")
            {
               return std::make_shared<CLineFormatItem>();
            }
            if (key == "m")
            {
               return std::make_shared<MsgFormatItem>();
            }
            if (key == "n")
            {
                return std::make_shared<NLineFormatItem>();
            }
            if(!key.empty()){
                std::cout<<"没有对应的格式化字符%"<<key<<std::endl;
                abort();
            }

            return std::make_shared<OtherFormatItem>(val);
        }

    private:
        std::string _pattern;
        std::vector<FormatItem::ptr> _items;
    };
}