#ifndef RTPS_Headers_SubMessageHeader_h_IsIncluded
#define RTPS_Headers_SubMessageHeader_h_IsIncluded

/*--------------------------------------------------------------------------------------
- The overall structure of an RTPS Message consists of a fixed-size leading RTPS Header followed by a variable number
of RTPS Submessage parts. Each Submessage in turn consists of a SubmessageHeader and a variable number of
SubmessageElements. This is illustrated in Figure 8.8.

- All Submessages start with a SubmessageHeader part followed by a concatenation of SubmessageElement parts. The
SubmessageHeader identifies the kind of Submessage and the optional elements within that Submessage. The
SubmessageHeader contains the fields listed in Table 8.15.

- Each RTPS message contains a variable number of RTPS Submessages. Each RTPS Submessage in turn is built from a set
of predefined atomic building blocks called SubmessageElements. RTPS 2.1 defines the following Submessage
elements: GuidPrefix, EntityId, SequenceNumber, SequenceNumberSet, FragmentNumber,
FragmentNumberSet, VendorId, ProtocolVersion, LocatorList, Timestamp, Count,
SerializedData, and ParameterList.
---------------------------------------------------------------------------------------*/

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/SubmessageKind.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Export.h"
namespace RTPS
{

/* --------------------------------------------------------------------------
Each RTPS Message consists of a variable number of RTPS Submessage parts. All RTPS Submessages feature the same
identical structure shown in Figure 8.10.

All Submessages start with a SubmessageHeader part followed by a concatenation of SubmessageElement parts. The
SubmessageHeader identifies the kind of Submessage and the optional elements within that Submessage. The
SubmessageHeader contains the fields listed in Table 8.15.

Variables:

The submessageId identifies the kind of Submessage. The valid ID’s are enumerated by the possible values of
SubmessageKind (see Table 8.13).

The flags in the Submessage header contain 8 boolean values. The first flag, the EndiannessFlag, is present and located
in the same position in all Submessages and represents the endianness used to encode the information in the Submessage.
The literal ‘E’ is often used to refer to the EndiannessFlag.
If the EndiannessFlag is set to FALSE, the Submessage is encoded in big-endian format, EndiannessFlag set to TRUE
means little-endian.
Other flags have interpretations that depend on the type of Submessage.

Indicates the length of the Submessage (not including the Submessage header).
In case submessageLength > 0, it is either
• The length from the start of the contents of the Submessage until the start of the header of the next Submessage (in
case the Submessage is not the last Submessage in the Message).
• Or else it is the remaining Message length (in case the Submessage is the last Submessage in the Message). An
interpreter of the Message can distinguish between these two cases as it knows the total length of the Message.

In case submessageLength==0, the Submessage is the last Submessage in the Message and extends up to the end of the
Message. This makes it possible to send Submessages larger than 64k (the maximum length that can be stored in the
submessageLength field), provided they are the last Submessage in the Message.
---------------------------------------------------------------------------*/
class DLL_STATE SubmessageHeader : public NetworkLib::NetObjectBase
{
public:
	SubmessageHeader(SubmessageKind::Type id, SubmessageLength length, octet flags);
	SubmessageHeader(octet flags = OMG_CDR_BE);
	virtual ~SubmessageHeader();

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const;
	virtual void Read(NetworkLib::SerializeReader *reader);

public:
	bool IsValid() const;

public:
	bool IsBigEndian() const;
	bool IsLittleEndian() const;

	void SetEndianFlag(octet endian);

public:
	static unsigned int SIZE()
	{
		static unsigned int headerSize = 4;
		return headerSize;
	}

public:
	bool operator==(const SubmessageHeader &other) const
	{
		return (submessageId_ == other.submessageId_) && (submessageFlags_ == other.submessageFlags_) && (submessageLength_ == other.submessageLength_);
	}

	bool operator!=(const SubmessageHeader &other) const
	{
		return !(*this == other);
	}

public:
	inline void					SetSubmessageKind(const SubmessageKind::Type &id)	{ submessageId_ = id; }
	inline void					SetSubmessageFlags(const octet &flags)				{ submessageFlags_ = flags; }
	inline void					SetSubmessageLength(const SubmessageLength &length)	{ submessageLength_ = length; }

	inline SubmessageKind::Type GetSubmessageKind()		const { return submessageId_; }
	inline octet				GetSubmessageFlags()	const { return submessageFlags_; }
	inline SubmessageLength		GetSubmessageLength()	const { return submessageLength_; }

private:
	SubmessageKind::Type	submessageId_;
	octet					submessageFlags_;
	SubmessageLength		submessageLength_;
};

} // namespace RTPS // RTPS

#endif // RTPS_Headers_SubMessageHeader_h_IsIncluded

