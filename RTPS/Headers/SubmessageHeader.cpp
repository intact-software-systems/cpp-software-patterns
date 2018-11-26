#include"RTPS/Headers/SubmessageHeader.h"

namespace RTPS
{

/*-----------------------------------------------------------------------------------
All Submessages start with a SubmessageHeader part followed by a concatenation of SubmessageElement parts. The
SubmessageHeader identifies the kind of Submessage and the optional elements within that Submessage. The
SubmessageHeader contains the fields listed in Table 8.15.
-------------------------------------------------------------------------------------*/
SubmessageHeader::SubmessageHeader(SubmessageKind::Type id, SubmessageLength length, octet flags)
	: submessageId_(id)
	, submessageFlags_(flags)
	, submessageLength_(length)
{

}
SubmessageHeader::SubmessageHeader(octet flags)
	: submessageId_(SubmessageKind::UNKNOWN)
	, submessageFlags_(flags)
	, submessageLength_(0)
{

}

SubmessageHeader::~SubmessageHeader()
{

}

void SubmessageHeader::Write(NetworkLib::SerializeWriter *writer) const
{
	writer->WriteInt8((octet)submessageId_);
	writer->WriteInt8(submessageFlags_);
	writer->WriteInt16(submessageLength_);
}

void SubmessageHeader::Read(NetworkLib::SerializeReader *reader)
{
	submessageId_ = (SubmessageKind::Type) reader->ReadInt8();
	submessageFlags_ = reader->ReadInt8();
	submessageLength_ = reader->ReadInt16();
}

/*-----------------------------------------------------------------------------------
- The submessageId identifies the kind of Submessage. The valid IDs are enumerated by the possible values of
SubmessageKind (see Table 8.13).

- The flags in the Submessage header contain 8 boolean values. The first flag, the EndiannessFlag, is present and located
in the same position in all Submessages and represents the endianness used to encode the information in the Submessage.
The literal E is often used to refer to the EndiannessFlag. If the EndiannessFlag is set to FALSE, the Submessage
is encoded in big-endian format, EndiannessFlag set to TRUE means little-endian. Other flags have interpretations
that depend on the type of Submessage.

- In case submessageLength > 0, it is either
	 The length from the start of the contents of the Submessage until the start of the header of the next Submessage (in
	case the Submessage is not the last Submessage in the Message).
	 Or else it is the remaining Message length (in case the Submessage is the last Submessage in the Message). An
	interpreter of the Message can distinguish between these two cases as it knows the total length of the Message.

- In case submessageLength==0, the Submessage is the last Submessage in the Message and extends up to the end of the
	Message. This makes it possible to send Submessages larger than 64k (the maximum length that can be stored in the
	submessageLength field), provided they are the last Submessage in the Message.
-----------------------------------------------------------------------------------*/
bool SubmessageHeader::IsValid() const
{
	// TODO: Implement
	return true;
}

/*-----------------------------------------------------------------------------------
	Bit number one in sugmessageFlags_

Section 8.3.3.2 in the PIM defines the EndiannessFlag as a flag present in all Submessages that indicates the endianness
used to encode the Submessage. The PSM maps the EndiannessFlag flag into the least-significant bit (LSB) of the flags.
This bit is therefore always present in all Submessages and represents the endianness used to encode the information in
the Submessage. The EndiannessFlag is represented with the literal ‘E’.

E=0 means big-endian, E=1 means little-endian.

The value of the EndiannessFlag can be obtained from the expression:

E = SubmessageHeader.flags & 0x01

Other bits in the flags have interpretations that depend on the type of Submessage.
-----------------------------------------------------------------------------------*/
bool SubmessageHeader::IsBigEndian() const
{
	char E = submessageFlags_ & SUBMESSAGE_HEADER_ENDIAN_FLAG;

	if(E == '0') return true;	// Big endian

	return false; // Little endian
}

bool SubmessageHeader::IsLittleEndian() const
{
	char E = submessageFlags_ & SUBMESSAGE_HEADER_ENDIAN_FLAG;

	if(E == '0') return false;	// Big endian

	return true; // Little endian
}

void SubmessageHeader::SetEndianFlag(octet endian)
{
	if(endian == OMG_CDR_LE)	// sets key flag by setting key-bit to 1
	{
		submessageFlags_ = submessageFlags_ | SUBMESSAGE_HEADER_ENDIAN_FLAG;		// set bit to 1
	}
	else	// sets data flag by setting bit to 0
	{
		ASSERT(endian == OMG_CDR_BE);

		submessageFlags_ = submessageFlags_ & ~SUBMESSAGE_HEADER_ENDIAN_FLAG;	// clear bit to 0
	}
}

} // namespace RTPS

