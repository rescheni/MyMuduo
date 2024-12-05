#pragma once

#include <cstdint>
#include <iostream>


namespace mymuduo {

	class Timestamp
	{
		public:
			Timestamp();
			explicit Timestamp(std::int64_t microSecondsSinceEpoch);
			static Timestamp now();
			std::string toString() const;
		private:
			std::int64_t microSecondsSinceEpoch_;
	};


}
