#include "VNetSocket.h"

#include <sstream>

VIPEndpoint::VIPEndpoint(const char* ipport)
{
	std::stringstream s(ipport);
	int a, b, c, d, port; //to store the 4 ints
	char ch; //to temporarily store the '.'
	s >> a >> ch >> b >> ch >> c >> ch >> d >> ch >> port;
	*this = VIPEndpoint(a, b, c, d, port);
}

VIPEndpoint::VIPEndpoint(const char* ip, uint16_t port)
{
	mAddr.sin_family = AF_INET;
	mAddr.sin_port = htons(port);
	//mAddr.sin_addr.s_addr = htonl(ip);
	int ret = inet_pton(AF_INET, ip, &(mAddr.sin_addr));
}

void VIPEndpoint::ToString(char* out, size_t outSize) const
{
	char ipStr[128] = {};
	inet_ntop(AF_INET, &(mAddr.sin_addr), ipStr, sizeof(ipStr));
	sprintf_s(out, outSize, "%s:%d", ipStr, GetPort());

	//sprintf_s(out, outSize, "%d.%d.%d.%d:%d",
	//	(uint32_t)mAddr.sin_addr.S_un.S_un_b.s_b1,
	//	(uint32_t)mAddr.sin_addr.S_un.S_un_b.s_b2,
	//	(uint32_t)mAddr.sin_addr.S_un.S_un_b.s_b3,
	//	(uint32_t)mAddr.sin_addr.S_un.S_un_b.s_b4,
	//	(uint32_t)mAddr.sin_port
	//);
}

