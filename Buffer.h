#pragma once

#include <algorithm>
#include <cstddef>
#include<vector>
#include <string>

namespace mymuduo
{
///  Code
/// +-------------------+ ---------------+------------------+
///|prependablebytes   | readableBytes  |   writeableBytes |
///|                  |                 |                  |
///+------------------+----------------+------------------+
///0  		<=   readerIndex_  <= writerIndex_  <=  	size
/// endcode
    class Buffer
    {

        public:
            static const size_t kCheapPrepend = 8;
            static const size_t kInitialSize = 1024; 


        public:
            explicit Buffer(size_t initialSize = kInitialSize)
                : buffer_(kCheapPrepend + initialSize)
                , readerIndex_(kCheapPrepend)
                , writerIndex_(kCheapPrepend)
            {}

            // ~Buffer();
            size_t readableBytes() const
            {
                return writerIndex_ - readerIndex_ ;
            }

            size_t writeableBytes() const
            {
                return buffer_.size() - writerIndex_;
            }
			size_t prependableBytes() const
			{
				return  readerIndex_;
			}
			const char* peek() const 
			{
				return  begin() + readerIndex_;
			}


			ssize_t readFd(int fd,int * saveErrno);

			ssize_t writeFd(int fd,int *saveErrno);

			
			//	onMessage string < Buffer
			void retrieve(size_t len)
			{
				if(len < readableBytes())
				{
					readerIndex_ += len;  // 只读取了刻度缓冲区的一部分len 
										  // 还剩下 reeaderIndex_ += len -> writerIndex_
				}
				else	// len == readableBytes() 
				{
					retrieveAll();
				}
			}

			void retrieveAll()
			{
				readerIndex_ = writerIndex_ = kCheapPrepend;
			}

			// 将 onMessage 函数 上报的 buffer 数据 转换成 string 类型的数据 返回
			std::string retrieveAllasString()
			{
				return retrieveAsString(readableBytes());	//  应用 可读取的长度
			}

			std::string retrieveAsString(size_t len)
			{
				std::string result(peek(),len);
				retrieve(len);
				return result;
			}	


			// buffer_.size() - writeenableBytes
			//	确保 可写
			void ensureWriteableBytes(size_t len)
			{
				if(writeableBytes() < len)
				{
					makeSpace(len);		// 扩容函数
				}
			}

			// 将 [data,data+len] 内存上的数据  添加到writeabl buffer
			void append(const char * /* restrict  */data, size_t len)
			{
				ensureWriteableBytes(len);// 保证空间足够
				std::copy(data,data+len,beginWrite());
				writerIndex_ += len;
			}
			char * beginWrite()
			{
				return begin() + writerIndex_;
			}
			
			const char * beginWrite() const
			{
				return  begin() + writerIndex_;
			}


		private:
			// 	it.operator *  
			// 	// 返回的 是vector容器 内部 元素数组的地址
			char * begin()
			{
				return &*buffer_.begin();
			}

			const char * begin() const
			{
				return &*buffer_.begin();
			}


			void makeSpace(size_t len)	// !!!!
			{
				if(writeableBytes() + prependableBytes() < len + kCheapPrepend)
				{
					buffer_.resize(writerIndex_ + len);
				}
				else {
					size_t readable = readableBytes();
					std::copy(begin() + readerIndex_,begin() + writerIndex_,begin()+kCheapPrepend);
					readerIndex_ = kCheapPrepend;
					writerIndex_ = readerIndex_ + readable;
				}
			}
        private:
            std::vector<char> buffer_;
            size_t readerIndex_;
            size_t writerIndex_;
    };
   
    
    
} // namespace mymuduo
