#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/NetAddress.h"
#include"NetworkLib/Serialize/SerializeBase.h"

#include"NetworkLib/Export.h"
namespace NetworkLib
{

/* -----------------------------------------------------------------------------
	class BufferedOutMessage is filled by impl of INetMessageParser
	and fills up inputqueue of impl of BufferedSocketWriter.
----------------------------------------------------------------------------- */
class DLL_STATE BufferedOutMessage
{
protected:
	typedef std::list<NetworkLib::NetAddress>	ListNetAddress;

public:
	BufferedOutMessage(const NetAddress &addr, SerializeWriter::Ptr &writer)
		: netAddress_(addr)
		, serializeWriter_(writer)
	{
		ASSERT(serializeWriter_);
		ASSERT(netAddress_.IsValid());
	}

    BufferedOutMessage(const ListNetAddress &listAddresses, SerializeWriter::Ptr &writer)
		: listNetAddresses_(listAddresses)
		, serializeWriter_(writer)
	{
		ASSERT(serializeWriter_);
	}

	~BufferedOutMessage()
	{}

    CLASS_TRAITS(BufferedOutMessage)

public:
    const NetAddress&		GetNetAddress() 		const { return netAddress_; }
	SerializeWriter::Ptr	GetSerializeWriter() 	const { return serializeWriter_; }

    bool HasMultipleAddresses()						const { return !listNetAddresses_.empty(); }

    const ListNetAddress&  GetListOfAddresses() 	const { return listNetAddresses_; }

private:
	ListNetAddress			listNetAddresses_;
	NetAddress 				netAddress_;
	SerializeWriter::Ptr 	serializeWriter_;
};

/* -----------------------------------------------------------------------------
	Base class for BufferedTcpSocketWriter and BufferedUdpSocketWriter
----------------------------------------------------------------------------- */
class DLL_STATE BufferedSocketWriter
{
public:
	virtual ~BufferedSocketWriter()
    {}

    CLASS_TRAITS(BufferedSocketWriter)

public:
	virtual bool PushMessage(BufferedOutMessage::Ptr &) = 0;

	virtual void Stop() = 0;
};

}
