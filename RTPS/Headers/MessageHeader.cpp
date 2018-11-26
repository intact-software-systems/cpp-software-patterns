#include"RTPS/Headers/MessageHeader.h"

namespace RTPS
{

MessageHeader::MessageHeader(const ProtocolId::Type &protocolId
							 , const ProtocolVersion &protocolVersion
							 , const VendorId &vendorId
							 , const GuidPrefix &guidPrefix)
	: protocolId_(protocolId)
	, protocolVersion_(protocolVersion)
	, vendorId_(vendorId)
	, guidPrefix_(guidPrefix)
{

}

MessageHeader::MessageHeader()
{

}
MessageHeader::~MessageHeader()
{

}

void MessageHeader::Write(NetworkLib::SerializeWriter *writer) const
{
	writer->WriteInt16((short)protocolId_);
	protocolVersion_.Write(writer);
	vendorId_.Write(writer);
	guidPrefix_.Write(writer);
}

void MessageHeader::Read(NetworkLib::SerializeReader *reader)
{
	protocolId_ = (ProtocolId::Type)reader->ReadInt16();
	protocolVersion_.Read(reader);
	vendorId_.Read(reader);
	guidPrefix_.Read(reader);
}

/*------------------------------------------------------------------------
A Header is invalid when any of the following are true:
 The Message has less than the required number of octets to contain a full Header. The number required is defined by
the PSM.
 Its protocol value does not match the value of PROTOCOL_RTPS.
 The major protocol version is larger than the major protocol version supported by the implementation.
-------------------------------------------------------------------------*/
bool MessageHeader::IsValid() const
{
	// TODO: Implement
	return true;
}

} // namespace RTPS // namespace MessageHeader::RTPS
