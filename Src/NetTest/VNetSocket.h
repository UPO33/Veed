#pragma once

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif


#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment( lib, "ws2_32.lib" )

typedef SOCKET VSocket;
#define VINVALID_SOCKET  INVALID_SOCKET

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <arpa/inet.h>

typedef int VSocket;
#define VINVALID_SOCKET -1

#endif

#include <stdint.h>
#include <stdio.h>
#include <iostream>

struct VIPEndpoint
{
	VIPEndpoint()
	{
		mAddr.sin_family = AF_INET;
		mAddr.sin_port = INADDR_ANY;
		mAddr.sin_addr.s_addr = 0;
	}
	VIPEndpoint(sockaddr_in addr) : mAddr(addr)
	{}
	VIPEndpoint(uint32_t a, uint32_t  b, uint32_t  c, uint32_t  d, uint16_t port) 
		: VIPEndpoint((a << 24) | (b << 16) | (c << 8) | d, port)
	{
	}
	VIPEndpoint(uint32_t ip, uint16_t port)
	{
		mAddr.sin_family = AF_INET;
		mAddr.sin_port = htons(port);
		mAddr.sin_addr.s_addr = htonl(ip);
	}
	VIPEndpoint(const char* ipport);
	VIPEndpoint(const char* ip, uint16_t port);
	
	uint16_t GetPort() const { return ntohs(mAddr.sin_port); }
	uint32_t GetIP() const { return ntohl(mAddr.sin_addr.s_addr); }

	void ToString(char* out, size_t outSize) const;

	//check if port and ip is equal
	inline bool operator == (const VIPEndpoint& other) const
	{
		return mAddr.sin_port == other.mAddr.sin_port && mAddr.sin_addr.s_addr == other.mAddr.sin_addr.s_addr;
	}
	inline bool operator != (const VIPEndpoint& other) const
	{
		return !this->operator==(other);
	}
	sockaddr_in mAddr;

};

inline void VGetLogString(const VIPEndpoint& addr, char* out, size_t size)
{
	addr.ToString(out, size);
}
static_assert(sizeof(VIPEndpoint) == sizeof(sockaddr_in), "");



//initialize socket system
inline bool VInitializeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
#else
	return true;
#endif
}
//shutdown socket system
inline void VShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
}
//
inline void VSocket_Close(VSocket socket)
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	close(socket);
#elif PLATFORM == PLATFORM_WINDOWS
	closesocket(socket);
#endif
}
//
inline void VSocketPrintLastError()
{
#if PLATFORM == PLATFORM_WINDOWS
	int err = WSAGetLastError();
	char buffer[4096];
	buffer[0] = 0;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, sizeof(buffer), NULL);
	printf("%s\n", buffer);
#else
#endif
}

inline VSocket VCreateTCPSocket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}
inline bool VSocket_Bind(VSocket s, VIPEndpoint addr)
{
	if (bind(s, (const sockaddr*)&addr.mAddr, sizeof(addr.mAddr)) < 0)
	{
		VSocketPrintLastError();
		return false;
	}
	return true;
}
inline bool VSocket_Listen(VSocket s, int backlog)
{
	if (listen(s, backlog) != 0)
	{
		VSocketPrintLastError();
		return false;
	}
	return true;
}

inline int VSocket_Send(VSocket s, const void* pData, int dataSize)
{
	if (dataSize == 0) return 0;

	int ret = send(s, (char*)pData, dataSize, 0);
	if (ret == -1)
	{
		VSocketPrintLastError();
	}
	return ret;
}
inline int VSocket_Recv(VSocket s, void* pData, int dataSize)
{
	if (dataSize == 0) return 0;
	int ret = recv(s, (char*)pData, dataSize, 0);
	if (ret == -1)
	{
		VSocketPrintLastError();
	}
	return ret;
}
inline VSocket VSocket_Accept(VSocket s, VIPEndpoint& addr)
{
	int addrLen = sizeof(addr.mAddr);

	return accept(s, (sockaddr*)&addr.mAddr, &addrLen);
}
inline VSocket VSocket_Connect(VSocket s, const VIPEndpoint& addr)
{
	int ret = connect(s, (const sockaddr*)&addr.mAddr, sizeof(addr.mAddr));
	if (ret != 0)
	{
		VSocketPrintLastError();
	}
	return ret;
}

inline void VSocket_SetBuffSize(VSocket s, int sndSize, int rcvSize)
{
	setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&sndSize, sizeof(int));
	setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcvSize, sizeof(int));
}
inline void VSocket_PrintBuffSize(VSocket s)
{
	int sb = 0;
	int rb = 0;
	int sbSize = sizeof(sb);
	int rbSize = sizeof(rb);
	getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&sb, &sbSize);
	getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&rb, &rbSize);
	printf("SndBuffSize %d  RcvBuffSize %d\n", sb, rb);

}
inline void VSocket_Timeout(VSocket s, uint32_t recvTimeoutMS, uint32_t sendTimeoutMS)
{
#if PLATFORM == PLATFORM_WINDOWS
	if (0 != setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeoutMS, sizeof(recvTimeoutMS)))
	{
		VSocketPrintLastError();
	}
	if (0 != setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendTimeoutMS, sizeof(sendTimeoutMS)))
	{
		VSocketPrintLastError();
	}
#else
	struct timeval timeout;

	timeout.tv_sec = recvTimeoutMS;
	timeout.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	timeout.tv_sec = sendTimeoutMS;
	timeout.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
}

inline VSocket VSocket_CreateUDP()
{
	auto handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (handle <= 0)
	{
		VSocketPrintLastError();
		return VINVALID_SOCKET;
	}
}

inline bool VSocket_SetNonBlocking(VSocket s)
{
#if PLATFORM == PLATFORM_MAC || 	PLATFORM == PLATFORM_UNIX

	int nonBlocking = 1;
	if (fcntl(s, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
	{
		VSocketPrintLastError();
		return false;
	}

#elif PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	if (ioctlsocket(s, FIONBIO, &nonBlocking) != 0)
	{
		VSocketPrintLastError();
		return false;
	}
	return true;
#endif
}

/*
UDP is a connectionless protocol, so each time you send a packet you must specify the destination address.
This means you can use one UDP socket to send packets to any number of different IP addresses, 
there is no single computer at the other end of your UDP socket that you are connected to.
*/
inline bool VSocket_SendTo(VSocket s, const void* buffer, int bufferLen, VIPEndpoint to)
{
	/*
	The return value from 'sendto' only indicates if the packet was successfully sent from the local computer.
	It does not tell you whether or not the packet was received by the destination computer.
	UDP has no way of knowing whether or not the the packet arrived at its destination!
	*/
	int sentSize = sendto(s, (const char*)buffer, bufferLen, 0, (sockaddr*)&to.mAddr, sizeof(to.mAddr));
	if (sentSize != bufferLen)
	{
		VSocketPrintLastError();
		return false;
	}
	return true;
}

inline int VSocket_RecvFrom(VSocket s, void* buffer, int bufferLen, VIPEndpoint& from)
{
	/*
	Any packets in the queue larger than your receive buffer will be silently discarded. 
	So if you have a 256 byte buffer to receive packets like the code above, and somebody sends you 
	a 300 byte packet, the 300 byte packet will be dropped. You will not receive just 
	the first 256 bytes of the 300 byte packet.
	*/
	int fromLength = sizeof(from.mAddr);
	int bytesReceived = recvfrom(s, (char*)buffer, bufferLen, 0, (sockaddr*)&from.mAddr, &fromLength);
	if (bytesReceived <= 0)
	{
		//VSocketPrintLastError();
	}
	return bytesReceived;
}