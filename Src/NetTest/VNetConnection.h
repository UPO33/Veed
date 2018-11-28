#pragma once

#include "VNetSocket.h"
#include "../Core/VNetBitStream.h"
#include "../Core/VLogger.h"
#include "../Core/VArray.h"

//////////////////////////////////////////////////////////////////////////
struct VNetId
{
	uint32_t mId = 0;

	bool IsNull() const { return mId == 0; }
};

inline VBitStream& operator << (VBitStream& stream, VNetId& id)
{
	stream.RWNumerBytePacked(id.mId);
	return stream;
}

//per application static object, for things like class, asset
struct VNetStaticEntity
{
};
//
struct VNetAsset : VNetStaticEntity
{
};
//an instance of this holds the list of static object 
//items must be registered in the same order and number in both sides
struct VNetAppCache
{
	//read class id from the stream and searchs it
	VNetClassInfo* ReadClass(VBitStream& stream) const;
	VNetAsset* ReadAsset(VBitStream& stream) const;

	void RegClass(VNetClassInfo* pClass);
	void RegAsset(VNetAsset* pAsset);

	VArray<VNetClassInfo*> mClasses;
	VArray<VNetAsset*> mAssets;
};

struct VNetIdCache
{
	uint32_t mIdCounter = 0;

	//create a new unique id. server only.
	VNetId CreateNewId(VNetObject* pObject)
	{
		mIdCounter++;
	}
	//free a previously created id
	void FreeId(VNetId& id)
	{

	}

	//
	VNetObject* FindObject(VNetId id) const;
	//
	void CacheIds(VNetId* ids, void** pointers, size_t count);
	//
	bool IsServer() const {}
};

typedef uint32_t VNetClassId;

struct VNetClassInfo
{
	struct VProp
	{
		void OnReplication = void(VNetObject::*)(VNetPlayer* pPeer, VBitStream& stream);
	};

	uint32_t mPropertyCount;
	uint32_t mClassId;
	VProp mProperties;

};

//net object that handles replication
struct VNetObject
{
	VNetId mId;
	uint32_t mChangeFlags;

	VNetClassInfo* GetClass() const {}

	//return true if this object has something to replicate to @pPlayer
	virtual bool IsReplicationRelevant(VNetPlayer* pPlayer) const
	{
		return mChangeFlags;
	}
	virtual void OnReplicate(VNetPlayer* pTarget, VBitStream& stream)
	{
		stream.RWNumber(mChangeFlags, GetClass()->mPropertyCount);
		for (size_t iProperty = 0; iProperty < GetClass()->mPropertyCount; iProperty++)
		{
			GetClass()->mProperties[i].OnReplication(pTarget, stream);
		}
	}

	//returns true if its dedicated server or listen server
	bool IsServer() const { }
	bool IsDedicatedServer() const {}
	bool IsListenServer() const {}
	bool IsClient() const {}
	void ClientDestroy() {}
};
//////////////////////////////////////////////////////////////////////////
struct VNetObject_TestPlant : VNetObject
{
	enum
	{
		ECF_GrowAge,
		ECF_CuurentPlace,
		ECF_CellIndex,
	};

	//time at which it spawned
	uint64_t mSpawnTime;
	//age of the plant 0 == seed, 1 fully grown
	uint8_t mGrowAge;
	//if null its placed on the soil. else it could be on player , store box, etc, ...
	VNetObject* mCurrentPlace;
	//cell index this is plant on
	int mCellIndex;

	void SetGrowAge(uint8_t value)
	{
		if (mGrowAge != value)
		{
			mGrowAge = value;
			mChangeFlags |= 1 << ECF_GrowAge;
		}
	}
	void SetCuurentPlace(VNetObject* value)
	{
		if (mCurrentPlace != value)
		{
			mCurrentPlace = value;
			mChangeFlags |= 1 << ECF_CuurentPlace;
		}
	}
	void OnRe_GrowAge(VNetPlayer* pPeer, VBitStream& stream)
	{
		stream << mGrowAge;
	}
};

struct VPacketHeader
{
	uint32_t mProto : 3;
	uint32_t mSeq : 9;
	uint32_t mAck : 9;
	uint32_t mUnrelliableSize : 10;
	uint32_t mReliableUnorderedSize : 10;
	uint32_t mReliableSize : 10;
};
struct VPacketExtracted
{
	uint8_t  mChannelsBuffer[3][0xFFf];
	uint32_t mChannelsSize[3];

	VPacketExtracted(VBitStream& stream)
	{

	}
};

struct VNetPacket
{
	uint32_t mSizeInByte;
	uint8_t  mData[0xFFff];
};


inline bool sequence_greater_than(uint16_t s1, uint16_t s2)
{
	return ((s1 > s2) && (s1 - s2 <= 32768)) ||
		((s1 < s2) && (s2 - s1  > 32768));
}
inline bool VSeqNumberGratherThan(uint16_t s1, uint16_t s2, uint16_t max = 0xFFff)
{
	const uint32_t d = max / 2 + 1;
	return ((s1 > s2) && (s1 - s2 <= d)) || ((s1 < s2) && (s2 - s1 > d));
}

//////////////////////////////////////////////////////////////////////////
struct VNetConnection
{
	static const unsigned MAX_CONNECTION = 100 * 1000;
	VSocket mSocket = VINVALID_SOCKET;
	VIPEndpoint mEndpoint;
	//valid if this is server
	VNetConnection* mConnections[MAX_CONNECTION];
	int mConnectionCount = 0;
	char mRecvBuffer[0xFFff];
	//Each time we send a packet we increase the local sequence number
	uint16_t mLocalSeqNumber = 0;
	//most recently received packet
	uint16_t mRemoteSeqNumber = 0;
	
	bool mIsInitilized = false;
	bool mInitilizedAsServer = false;

	VNetWorld* mWorld = nullptr;
	VNetIdCache* mCache = nullptr;
	VNetAppCache* mAppCache = nullptr;

	//replication data of an object
	struct VRepObject
	{
		uint64_t mCaptureTime;
		VNetObject* mObject;
		VBitWriter mData;
		VArray<VNetObject*> mRefrencedobjects;
		//whether its the first spawn or not
		bool bIsInitial;
	};
	//replication of a world for a client
	struct VRepCapture
	{
		uint64_t mCaptureTime;	//time at which we captured the properties
		VNetConnection* mTargetClient; //client qw want to send data to
		VArray<VRepObject> mObjects;
	};
	struct VDestructionPack
	{
		//list of destroyed objects
		VArray<VNetObject*> mObjects;
		VBitWriter mSerializedData;
	};
	
	
	void ServerMakeDestructionIdPack()
	{

	}
	void ServerReplicateObject()
	{
		//for each client
		for (size_t iClient = 0; iClient < mConnectionCount; iClient++)
		{
			VNetConnection* pClient = mConnections[iClient];
			if (pClient)
			{
				VRepCapture* pCapture = new VRepCapture;
				pCapture->mCaptureTime = CurrentRealTime();
				pCapture->mTargetClient = pClient;
				
				//for each object
				for (int iObject = 0; iObject < mWorld->mObjects.Length(); iObject++)
				{
					VNetObject* pObject = mWorld->mObjects[iObject];
					if (pObject)
					{
						if (pObject->IsReplicationRelevant(pClient))
						{
							VRepObject* pRepObject = new VRepObject();
							pObject->OnReplicate(pClient, pRepObject->mData);
							pCapture->mObjects.Add(pRepObject);
						}
					}
				}
										  
				pClient->QueRep(pCapture);
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	int ClientHandleDestructionPack(VBitStream& packet)
	{
		uint32_t count = 0;
		packet.RWNumerBytePacked(count);
		if (packet.IsValid())
			return -1;

		int numHandled = 0;

		for (uint32_t i = 0; i < count; i++)
		{
			VNetId netId;
			packet << netId;

			if (netId.IsNull() || !packet.IsValid())
				return -1;

			if (VNetObject* pObject = mCache->FindObject(netId))
			{
				pObject->ClientDestroy();
				numHandled++;
			}
			else
			{
				VLOG_ERR("object not found for VNetId %", netId);
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	int ClientHandleReplicationPack(VBitStream& packet)
	{
		uint32_t count = 0;
		packet.RWNumerBytePacked(count);
		if (!packet.IsValid())
			return -1;

		int numHandled = 0;

		for (uint32_t i = 0; i < count; i++)
		{
			VNetId objectId;
			packet << objectId;

			if (!packet.IsValid() || objectId.IsNull())
				return -1;
			
			if (VNetObject* pObject = mCache->FindObject(objectId))
			{
				pObject->OnReplicate(nullptr, packet);
				numHandled++;
			}
			else
			{
				VLOG_ERR("object not found for NetId %. we can't continue the parsing and drop the packet.", objectId);
				return -1;
			}
		}

		return numHandled;
	}
	//////////////////////////////////////////////////////////////////////////
	int ClientHandleSpawnPack(VBitStream& packet)
	{
		int numCreated = 0;
		while (packet.IsValid() && packet.Avail())
		{
			VNetId id;
			packet << id;

			
			if (mCache->FindObject(id))
			{
				VLOG_ERR("netid already exist");
				return numCreated;
			}

			VNetClassInfo* pClass = mAppCache->ReadClass(packet);
			if (pClass)
			{
				VNetObject* pObj = pClass->NewObject();
				pObj->mId = id;
				pObj->OnReplicate(packet);
			}


			numCreated++;
		}
		return numCreated;
	}
	void ServerGenerateSpawnPack(VArray<VRepObject*> out)
	{
		//for(int i = 0; i < mWorld->mPendingReplicateSpawn.Length(); i++)
		//{
		//	VNetObject* pObject = mWorld->mPendingReplicateSpawn[i];
		//	if (pObject && pObject->mId.IsNull())
		//	{
		//		VRepObject* pNewRep = new VRepObject();
		//		pNewRep->mObject = pObject;
		//		pNewRep->mCaptureTime = this->CurrentRealTime();
		//		pObject->IsReplicationRelevant()
		//	}
		//}

	}
	/*
	What TCP does is maintain a sliding window where the ack sent is the sequence number of the next packet
	it expects to receive, in order. If TCP does not receive an ack for a given packet, it stops and resends
	a packet with that sequence number again. This is exactly the behavior we want to avoid!
	*/

	VNetConnection* FindConnection(const VIPEndpoint& addr) const
	{
		for (int i = 0; i < mConnectionCount; i++)
		{
			if (mConnections[i] && mConnections[i]->mEndpoint == addr)
				return mConnections[i];
		}
		return nullptr;
	}
	bool CreateServer(const VIPEndpoint& bindTo)
	{
		if (mIsInitilized)return false;

		mSocket = VSocket_CreateUDP();
		if (mSocket != VINVALID_SOCKET)
			return false;

		VSocket_SetNonBlocking(mSocket);
		if (VSocket_Bind(mSocket, bindTo))
		{
			VSocket_Close(mSocket);
			return false;
		}

		mEndpoint = bindTo;
		mIsInitilized = mInitilizedAsServer = true;
		return true;
	}
	bool ClientConnect(const VIPEndpoint& serverAddr)
	{
		if (mIsInitilized)return false;

		mSocket = VSocket_CreateUDP();
		if (mSocket == VINVALID_SOCKET)
			return false;
		VSocket_SetNonBlocking(mSocket);
		mIsInitilized = true;
		mInitilizedAsServer = false;
		return true;
	}
	void Tick()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void ClientChannelGotData(VBitStream& channelData)
	{
		if (ClientHandleDestructionPack(channelData) == -1)
			return;

		if (ClientHandleSpawnPack(channelData) == -1)
			return;

		if (ClientHandleReplicationPack(channelData) == -1)
			return;
		
	}





	void ServerHandleChanelRead(VBitStream& channelData)
	{
		channelData.
	}
	void ServerHandleReadUDP(VBitStream& packet)
	{
		VPacketHeader header;
		header << packet;

		uint8_t unreliableBuffer[0xFFf];

		packet.RWBits(unreliableBuffer, 0, header.mUnrelliableSize);
		
		
	}
	void TickServer()
	{
		if (!mIsInitilized) return;

		//get incoming packets
		while (true)
		{
			VIPEndpoint sender;
			int byteRecv = VSocket_RecvFrom(mSocket, mRecvBuffer, sizeof(mRecvBuffer), sender);
			if (byteRecv <= 0)
				break;

			VNetConnection* pSenderConnection = FindConnection(sender);
			if (pSenderConnection)
			{
				VBitReader packetReader(mRecvBuffer, byteRecv);
			}
		}

		//send outgoing packets
		for (int i = 0; i < mConnectionCount; i++)
		{
			if (mConnections[i] == nullptr)
				continue;


		}
	}
	void TickClient()
	{
		if (!mIsInitilized) return;

		//get incoming packets
		while (true)
		{
			VIPEndpoint sender;
			int byteRecv = VSocket_RecvFrom(mSocket, mRecvBuffer, sizeof(mRecvBuffer), sender);
			if (byteRecv <= 0)
				break;

			if (sender != mEndpoint)
			{
				VLOG_MSG("we got packet from % which is not the server.", sender);
			}
		}

		//send outgoing packets
	}

	//0 unreliable
	//1 reliable unordered
	//2 reliable
	void QueuePacket(int layer, const void* pData, size_t size)
	{

	}
	//server only
	void RegisterNetObject(VNetObject* pObject)
	{

	}
	uint64_t CurrentRealTime() const {}
};

struct VNetWorld
{
	//all object in the world
	VArray<VNetObject*> mObjects;
	//object that has been spawned but not replicated to clients yet
	VArray<VNetObject*> mPendingReplicateSpawn;
	VNetPlayer* mPlayer;


};

struct VNetPlayer
{
	VNetConnection* mConnection;

	//position
	//rotation
	//velocity
	
	void Server_PickItem();
};

struct VWorldObject : VNetObject
{

};