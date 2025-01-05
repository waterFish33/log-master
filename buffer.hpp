#pragma once

#include <vector>
#include <string>
#include <cassert>
namespace log_master
{
    #define DEFAULT_BUFFER_SIZE 10*1024*1024
    #define THRESHOLD_BUFFER_SIZE 80*1024*1024
    #define INCREMENT_BUFFER_SIZE 10*1024*1024
    class Buffer
    {
    public:
        Buffer():_buffer(DEFAULT_BUFFER_SIZE),_writer_idx(0),_reader_idx(0){}
        // 向缓冲区写入数据
        void push(const std::string &data, size_t len){
            //缓冲区剩余空间不够处理：
                //1.扩容 
                // if(len>writeAblesize()){return ;}
                //2.阻塞/返回false
                expension(len);
            //将数据拷贝进缓冲区
            std::copy(data.begin(),data.end(),&_buffer[_writer_idx]);
            //将写入指针向后偏移
            moveWriter(len);
        }
       
        // 返回可读数据的起始地址
        const char* begin(){
            return &_buffer[_reader_idx];
        }
        // 返回可读数据的长度
        size_t readAbleSize(){
            return _writer_idx-_reader_idx;
        }
        // 返回可写数据的长度
        size_t writeAbleSize(){
            return _buffer.size()-_writer_idx;
        }
        // 对读指针进行向后偏移操作
        void moveReader(size_t len){
            assert(len<=readAbleSize());
            _reader_idx+=len;
        }
        // 重置读写位置，初始化缓冲区
        void reset(){
            _writer_idx=0;
            _reader_idx=0;
        }
        // 对Buffer实现交换操作
        void swap(Buffer &buffer){
            _buffer.swap(buffer._buffer);
           std::swap(_reader_idx,buffer._reader_idx);
           std::swap(_writer_idx,buffer._writer_idx);
        }
        // 判断缓冲区是否为空
        bool empty(){
            return _writer_idx==_reader_idx;
        }

    private:
        // 对写指针进行向后偏移操作
        void moveWriter(size_t len){
            assert(_writer_idx + len <= _buffer.size());
            _writer_idx+=len;
        }
        //扩容操作(阈值范围内翻倍增长，阈值范围外线性增长)
        void expension(size_t len)
        {
            if (len <= writeAbleSize())
            {
                return;
            }
            size_t new_size=_buffer.size();
            while (len > new_size-_writer_idx)
            {
                if (_buffer.size() < THRESHOLD_BUFFER_SIZE)
                {
                    new_size =new_size * 2;
                }
                else
                {
                    new_size = new_size + INCREMENT_BUFFER_SIZE;
                }
            }

            _buffer.resize(new_size);
        }

    private:
        std::vector<char> _buffer;
        size_t _reader_idx; // 当前可读数据的指针--下标
        size_t _writer_idx; // 当前可写数据的指针
    };
}