#ifndef NetworkLib_BufferedSocketReader_IsIncluded
#define NetworkLib_BufferedSocketReader_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/NetAddress.h"
#include"NetworkLib/Serialize/SerializeBase.h"

#include"NetworkLib/Export.h"
namespace NetworkLib 
{
/* -----------------------------------------------------------------------------
	class BufferedInMessage is filled by impl of BufferedSocketReader 
	and accessed through PopMessage
----------------------------------------------------------------------------- */
class DLL_STATE BufferedInMessage
{
public:
	BufferedInMessage(const NetAddress &addr, SerializeReader::Ptr &reader)
		: netAddress_(addr)
		, serializeReader_(reader)
	{
		ASSERT(serializeReader_);
		ASSERT(netAddress_.IsValid());
	}
	~BufferedInMessage()
    {}

    CLASS_TRAITS(BufferedInMessage)

public:
	const NetAddress&		GetNetAddress() 		const { return netAddress_; }
	SerializeReader::Ptr	GetSerializeReader() 	const { return serializeReader_; }

private:
	NetAddress 				netAddress_;
	SerializeReader::Ptr 	serializeReader_;
};

/* -----------------------------------------------------------------------------
	Base class for BufferedTcpSocketReader and BufferedUdpSocketReader
----------------------------------------------------------------------------- */
class DLL_STATE BufferedSocketReader 
{
public:
    virtual ~BufferedSocketReader()	{}

    CLASS_TRAITS(BufferedSocketReader)

public:
	virtual BufferedInMessage::Ptr PopMessage() = 0;

	virtual void Stop() = 0;
};

} // namespace NetworkLib

#endif // NetworkLib_BufferedSocketReader_IsIncluded


