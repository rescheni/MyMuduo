#include "Buffer.h"
#include <cstddef>
#include <bits/types/struct_iovec.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/uio.h>

namespace mymuduo
{
	// 从 fd  读取数据  
	// Poller 工作在 LT 模式  -->  数据 不会丢失
	//  buffer  缓冲区 是有大小的 但是 fd 读数据 并不知道tcp 的最终大小
	
	ssize_t Buffer::readFd(int fd,int * saveErrno)
	{
		char extrabuf[65536] = {0};
		struct iovec vec[2];
		const size_t writeable = writeableBytes(); // 底层 的 缓冲区 剩余的空间大小
		vec[0].iov_base = begin() + writerIndex_;
		vec[0].iov_len  = writeable;

		vec[1].iov_base = extrabuf;
		vec[1].iov_len = sizeof extrabuf;

		const int iovcent = (writeable < sizeof(extrabuf)) ? 2 : 1;
		const ssize_t n = readv(fd, vec,iovcent);
		if(n  < 0)
		{
			* saveErrno = errno;
		}
		else if(n <= writeable)		// buffer  的 可写 缓冲区 已经够数据
		{
			writerIndex_+=n;

		}
		else		// extrabuf 里面 也写入了数据 
		{
			writerIndex_ = buffer_.size();
			append(extrabuf, n - writeable);
		}

		return n;
	}
} // namespace mymuduo
