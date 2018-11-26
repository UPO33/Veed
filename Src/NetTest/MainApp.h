#pragma once

#include "VNetSocket.h"
#include <vector>

struct VServerConnection
{
	std::vector<VClientPeer> mClients;
};
struct VClientPeer
{
	VIPEndpoint mAddr;
};

struct VServerApp
{
	VServerApp();

	uint32_t mLastRecvClock = 0;
	char mName[128] = {};
	VSocket mSocket = VINVALID_SOCKET;
	int mGuiPort = 3434;

	int mGuiFloodPackSize = 1024;
	int mGuiFloodTickMs = 10;
	int mFloodPktCounter = 0;
	bool mIsFlooding = false;
	uint32_t mLastFloodTime = 0;
	std::vector<VClientPeer> mClients;
	
	void DoFlooding();
	
	void CreateListen(uint16_t port);

	virtual void Tick();
	virtual void Draw();
};

struct VClientApp
{
	VClientApp();
	
	char mName[128] = {};
	VSocket mSocket = VINVALID_SOCKET;
	char mGuiIPPort[128] = {};
	VIPEndpoint mTargetAddr;

	int mGuiTestFloodPacketSize = 1024;
	int mGuiTestFloodPacketCount = 32;

	void TestFlood();

	virtual void Tick();
	virtual void Draw();
};
struct VNetTestApp
{
	VNetTestApp();
	~VNetTestApp();
	
	std::vector<VServerApp> mServers;
	std::vector<VClientApp> mClients;

	virtual void Tick();
	virtual void Draw();
	
};