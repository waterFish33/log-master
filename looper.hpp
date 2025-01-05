#pragma once
/*实现异步日志器*/
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

#include "./buffer.hpp"

namespace log_master
{
    using Functor = std::function<void(Buffer &)>;
    class AsyncLooper
    {
    public:
        enum AsyncType
        {
            ASYNC_SAFE,  // 安全模式，避免无限的资源索取
            ASYNC_NOSAFE // 非安全模式，常用于测试
        };
        using ptr = std::shared_ptr<AsyncLooper>;

    public:
        AsyncLooper(const Functor &callback, AsyncLooper::AsyncType type = ASYNC_SAFE) : _stop(false), _thread(std::thread(&AsyncLooper::threadEntry, this)), _callback(callback), _looper_type(type) {}
        ~AsyncLooper() { stop(); }
        void push(const std::string &data, size_t len)
        {
            // 1.无限扩容（非安全） 2.固定大小--生产缓冲区满了就进行阻塞
            std::unique_lock<std::mutex> lock(_mutex);
            // 若缓冲区剩余空间大小大于数据长度，则可以添加数据
            if (_looper_type == ASYNC_SAFE)
            {
                _pro_cond.wait(lock, [&]()
                               { return _pro_buf.writeAbleSize() >= len; });
            }
            _pro_buf.push(data, len);
            // 唤醒消费者对缓冲区的数据进行处理
            _con_cond.notify_all();
        }
        void stop()
        {
            _stop = true;
            _con_cond.notify_all();
            _thread.join(); // 等待工作线程结束
        }

    private:
        // 线程入口函数
        void threadEntry()
        {  
            while (!_stop|| !_pro_buf.empty())
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    // 1.判断生产缓冲区是否有数据，有则交换，无则阻塞
                    // 退出前被唤醒或有数据被唤醒
                    _con_cond.wait(lock, [&]()
                                   { return _stop || !_pro_buf.empty(); });
                    _con_buf.swap(_pro_buf);
                    // 2.唤醒生产者
                    if (_looper_type == ASYNC_SAFE)
                    {
                        _pro_cond.notify_one();
                    }
                }
                // 3.唤醒后对消费者缓冲区进行处理
                _callback(_con_buf);
                // 4.初始化消费者缓冲区
                _con_buf.reset();
            }
        }

    private:
        Functor _callback; // 回调函数
    private:
        AsyncType _looper_type;
        std::atomic<bool> _stop; // 工作器停止标志
        Buffer _pro_buf;         // 生产者缓冲区
        Buffer _con_buf;         // 消费者缓冲区
        std::mutex _mutex;
        std::condition_variable _pro_cond;
        std::condition_variable _con_cond;
        std::thread _thread; // 异步工作器对应工作线程
    };

}