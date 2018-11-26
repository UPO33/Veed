#include "MainApp.h"
#include "../ImGui/imgui.h"
#include "../Core/VLogger.h"

enum ETestId
{
	ETI_TestFlood = 0xf
};
struct VTestPackBase
{
	uint32_t mId;
};
struct VTestFloodPack : VTestPackBase
{
	uint32_t mIndex;
	//rest is data
};

VNetTestApp::VNetTestApp()
{
	VInitializeSockets();
}

VNetTestApp::~VNetTestApp()
{
	VShutdownSockets();
}

void VNetTestApp::Tick()
{
	for (VServerApp& server : mServers)
	{
		server.Tick();
	}
	for (VClientApp& client : mClients)
	{
		client.Tick();
	}
}

void VNetTestApp::Draw()
{
	if (ImGui::Begin("NetTestApp"))
	{
		if (ImGui::Button("CreateServer"))
		{
			mServers.push_back(VServerApp());
		}
		if (ImGui::Button("ClientApp"))
		{
			mClients.push_back(VClientApp());
		}
	}
	ImGui::End();


	for (VServerApp& server : mServers)
	{
		server.Draw();
	}
	for (VClientApp& client : mClients)
	{
		client.Draw();
	}
}

VServerApp::VServerApp()
{
	static int SCounter = 0;
	sprintf_s(mName, "Server%d", SCounter++);
}

void VServerApp::DoFlooding()
{
	uint32_t curClock = std::clock();
	uint32_t elapsed = curClock - mLastFloodTime;
	if (elapsed > mGuiFloodTickMs)
	{
		mLastFloodTime = curClock;
		
		char* pBuffer = new char[mGuiFloodPackSize];
		memset(pBuffer, 0, mGuiFloodPackSize);

		VTestFloodPack* pFloodPkt = (VTestFloodPack*)pBuffer;
		pFloodPkt->mId = ETI_TestFlood;
		pFloodPkt->mIndex = mFloodPktCounter++;

		for (VClientPeer& peer : mClients)
		{
			VSocket_SendTo(mSocket, pBuffer, mGuiFloodPackSize, peer.mAddr);
		}

		delete pBuffer;
	}

	
}

void VServerApp::CreateListen(uint16_t port)
{
	mSocket = VSocket_CreateUDP();
	VSocket_Bind(mSocket, VIPEndpoint(0, 0, 0, 0, port));
	VSocket_SetNonBlocking(mSocket);
}

void VServerApp::Tick()
{
	if (mSocket != VINVALID_SOCKET)
	{
		char buffer[0xFFff];
		VIPEndpoint sender;

		int numRecv = VSocket_RecvFrom(mSocket, buffer, sizeof(buffer), sender);
		if (numRecv > 0)
		{
			
			uint32_t curClock = std::clock();
			if (mLastRecvClock == 0) mLastRecvClock = curClock;
			uint32_t clockOffst = curClock - mLastRecvClock;
			mLastRecvClock = curClock;

			VLOG_SUC("% bytes received from %. clockOffset %", numRecv, sender, clockOffst);
			

			VTestPackBase* pPack = (VTestPackBase*)buffer;
			if (pPack->mId == ETI_TestFlood)
			{

			}
		}


		if (mIsFlooding)
			DoFlooding();
	}
}

void VServerApp::Draw()
{
	if (ImGui::Begin(mName))
	{
		if(mSocket == VINVALID_SOCKET)
		{
			ImGui::InputInt("Port", &mGuiPort);
			if (ImGui::Button("CreateaAndListen"))
			{
				CreateListen(mGuiPort);
			}
		}
		else
		{
			ImGui::DragInt("FloodPktSize", &mGuiFloodPackSize);
			ImGui::DragInt("FloodTickMs", &mGuiFloodTickMs);
			if (ImGui::Button(mIsFlooding ? "StopFlood" : "StartFlood"))
			{
				mIsFlooding = !mIsFlooding;
			}
		}
	}
	ImGui::End();
}

VClientApp::VClientApp()
{
	static int SCounter = 0;
	sprintf_s(mName, "Client%d", SCounter++);
	sprintf_s(mGuiIPPort, "127.0.0.1:3434");
	mTargetAddr = VIPEndpoint(mGuiIPPort);
}

void VClientApp::TestFlood()
{
	char* pBuffer = new char[mGuiTestFloodPacketSize];
	memset(pBuffer, 0, mGuiTestFloodPacketSize);
	VTestFloodPack* pPkt = (VTestFloodPack*)pBuffer;
	pPkt->mId = ETI_TestFlood;
	
	for (int i = 0; i < mGuiTestFloodPacketCount; i++)
	{
		pPkt->mIndex = i;
		if (!VSocket_SendTo(mSocket, pBuffer, mGuiTestFloodPacketSize, mTargetAddr))
		{
			VLOG_ERR("failed to send");
		}
	}
	
}

void VClientApp::Tick()
{
	//if (mSocket != VINVALID_SOCKET)
	//{
	//	char buffer[0xFFff];
	//	VIPEndpoint sender;
	//	char senderStr[256];
	//	int numRecv = VSocket_RecvFrom(mSocket, buffer, sizeof(buffer), sender);
	//	sender.ToString(senderStr, sizeof(senderStr));
	//	printf("%d bytes received from %s\n", numRecv, senderStr);
	//
	//}
}

void VClientApp::Draw()
{
	if (ImGui::Begin(mName))
	{
		if (mSocket == VINVALID_SOCKET )
		{
			if (ImGui::InputText("IPPort", mGuiIPPort, sizeof(mGuiIPPort)))
			{
				mTargetAddr = VIPEndpoint(mGuiIPPort);
			}
			if (ImGui::Button("Create"))
			{
				mSocket = VSocket_CreateUDP();
			}

		}
		else
		{
			if (ImGui::Button("SendHello"))
			{
				VSocket_SendTo(mSocket, "Hello", 6, mTargetAddr);
			}
			ImGui::DragInt("FloodPktSize", &mGuiTestFloodPacketSize);
			ImGui::DragInt("FloodPktCount", &mGuiTestFloodPacketCount);
			if (ImGui::Button("TestFlood"))
			{
				TestFlood();
			}
		}
	}
	ImGui::End();
}
