#ifndef RTPS_Headers_DataEncapsulation_h_IsIncluded
#define RTPS_Headers_DataEncapsulation_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Export.h"
namespace RTPS
{
/* ---------------------------------------------------------------------------------
All data encapsulation schemes must start with an encapsulation scheme identifier.

	octet[2] Identifier

The identifier occupies the first two octets of the serialized data-stream, as shown below:
0...2...........8...............16..............24..............32
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Encapsulation Identifier		|				|				|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
				-----------stream------------->>>>
The remaining part of the serialized data stream either contains the actual data
or additional encapsulation specific information.
--------------------------------------------------------------------------------- */
class DLL_STATE DataEncapsulation : public NetworkLib::NetObjectBase
{
public:
	DataEncapsulation(char endian = OMG_CDR_BE);
	virtual ~DataEncapsulation();

	virtual void Write(NetworkLib::SerializeWriter *writer) const;
	virtual void Read(NetworkLib::SerializeReader *reader);

public:
	inline bool IsValid() const { return true; }

public:
	inline char GetEndian()	const { return endian_;}

public:
	static unsigned int SIZE()
	{
		static unsigned int headerSize = 1;
		return headerSize;
	}

private:
	// TODO: Match DataEncapsulation as defined in RTPS
	octet endian_;
};

} // namespace RTPS

#endif

