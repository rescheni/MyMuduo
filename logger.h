#pragma once

#include <string>

#include "noncopyable.h"

// 定义日志的 级别  
// INFO  普通信息
// ERROR 错误信息
// FATAL 致命错误
// DEBUG 调试信息



// 用户使用 宏
// LOG_INFO("%s %d",arg1,arg2);
#define LOG_INFO(logmsgFormat,...)			\
	do										\
	{										\
		Logger &logger = Logger::instance();\
		logger.setLogLevel(INFO);			\
		char buffer[1024] = {0};			\
		snprintf(buffer,1024,logmsgFormat,##__VA_ARGS__);\
		logger.log(buffer);					\
	}while (0)

#define LOG_ERROR(logmsgFormat,...)			\
	do										\
	{										\
		Logger &logger = Logger::instance();\
		logger.setLogLevel(ERROR);			\
		char buffer[1024] = {0};			\
		snprintf(buffer,1024,logmsgFormat,##__VA_ARGS__);\
		logger.log(buffer);					\
	}while (0)


#define LOG_FATAL(logmsgFormat,...)			\
	do										\
	{										\
		Logger &logger = Logger::instance();\
		logger.setLogLevel(FATAL);			\
		char buffer[1024] = {0};			\
		snprintf(buffer,1024,logmsgFormat,##__VA_ARGS__);\
		logger.log(buffer);					\
	}while (0)


#ifdef DEBUG
#define LOG_DEBUG(logmsgFormat,...)			\
	do										\
{										\
	Logger &logger = Logger::instance();\
	logger.setLogLevel(DEBUG);			\
	char buffer[1024] = {0};			\
	snprintf(buffer,1024,logmsgFormat,##__VA_ARGS__);\
	logger.log(buffer);					\
}while (0)
#else 
	#define LOG_DEBUG(logmsgFormat,...)
#endif // DEBUG



namespace mymuduo {

	enum LogLevel
	{
		INFO ,
		ERROR,
		FATAL,
		DEBUG
	};



	// 输出 一个  日志 类
	class Logger:noncopyable
	{
		public:
			// 获取 唯一的日志 实例对象
			static Logger&  instance();
			// 设置 日志 级别
			void setLogLevel(int level);
			// 写日志
			void log(std::string msg);


		private:
			int LogLevel_; // 和系统的 变量不产生冲突
			Logger(){}
	};


}


