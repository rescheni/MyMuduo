#include <iostream>

#include "logger.h"
#include "Timestamp.h"



namespace mymuduo {

	// static  该文件内 
	static string LevelToString(int level)
	{
		
		switch (level) {
			case INFO:return "[INFO]";
			case ERROR:return "[ERROR]";
			case FATAL:return "[FATAL]";
			case DEBUG:return "[DEBUG]";
			default: return "[???]";
		}
	}

	// 获取 唯一的日志 实例对象
	Logger&  Logger::instance()
	{
		static Logger logger;
		return logger;
	}
	// 设置 日志 级别
	void Logger::setLogLevel(int level)
	{
		LogLevel_ = level;


	}
	// 写日志 [级别信息] time : msg
	void Logger::log(std::string msg)
	{
		// 时间 未完成
		std::cout<< "AtTime: " << Timestamp::now().toString() << ":" << LevelToString(LogLevel_)<<msg<< std::endl;
	}

}
