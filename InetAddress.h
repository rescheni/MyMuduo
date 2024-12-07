#pragma  once 

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>  // sockaddr_in 
#include <string>
namespace mymuduo {

	// 对socket 的 封装
	class InetAddress
	{
		public:
			explicit InetAddress(uint16_t port,std::string ip = "127.0.0.1");
			explicit InetAddress(const sockaddr_in & addr)
				:addr_(addr)
			{}

			std::string toIp() const;
			std::string toIpPort() const;
			uint16_t toPort() const;

		private:
			sockaddr_in addr_;
	};
}
