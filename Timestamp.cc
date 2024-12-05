#include "Timestamp.h"
#include <cstdio>
#include <ctime>



namespace mymuduo {

	Timestamp::Timestamp()
		:microSecondsSinceEpoch_(0)
	{}
	Timestamp::Timestamp(std::int64_t microSecondsSinceEpoch)	
		:microSecondsSinceEpoch_(microSecondsSinceEpoch)
	{}
	Timestamp Timestamp::now()
	{
		// 获取 当前时间
		return Timestamp(time(NULL));
	}
	std::string Timestamp::toString() const
	{
		char buffer[128];
		time_t ti = time(nullptr);
		tm * tm_time = localtime(&microSecondsSinceEpoch_);
		snprintf(buffer,128, "%4d/%02d/%02d %02d:%02d:%02d",
				tm_time->tm_year + 1900,
				tm_time->tm_mon + 1,
				tm_time->tm_mday,
				tm_time->tm_hour,
				tm_time->tm_min,
				tm_time->tm_sec);

		return buffer;
	}
}

#include <iostream>

int main()
{
	std::cout << mymuduo::Timestamp::now().toString() << std::endl;
	return 0;
}
