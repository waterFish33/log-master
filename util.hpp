#pragma once
/*实现实用工具类
1.获取系统时间
2.判断文件是否存在
3.获取文件所在路径
4.创建目录
*/
#include <iostream>
#include <ctime>
#include <sys/stat.h>

namespace log_master
{
    namespace Util
    {
        class Date
        {
        public:
            static size_t getTime()
            {
                return (size_t)std::time(nullptr);
            }
        };
        class File
        {
        public:
            // 成功返回true，失败返回false
            static bool IsExist(const std::string &filepath)
            {
                struct stat st;
                if (stat(filepath.c_str(), &st) < 0)
                {
                    return false;
                }
                return true;
            }
            //
            static std::string Path(const std::string &pathname)
            {
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                {
                    return ".";
                }
                return pathname.substr(0, pos + 1);
            }
            // 需要一个相对路径
            static void CreateDirectory(const std::string &pathname)
            {
                // mkdir(pathname.c_str(),0777);
                //./aaa/aaa/aaa
                size_t idx = 0, pos = 0;
                while (pos < pathname.size())
                {
                    idx = pathname.find_first_of("/\\", pos);
                    if (idx == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0777);
                        break;
                    }
                    std::string parent_dir = pathname.substr(0, idx);
                    if (IsExist(parent_dir))
                    {
                        pos = idx + 1;
                        continue;
                    }

                    mkdir(parent_dir.c_str(), 0777);
                    pos = idx + 1;
                }
            }
        };
    }
}