#ifndef RTPS_Headers_MessageHeader_h_IsIncluded
#define RTPS_Headers_MessageHeader_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*------------------------------------------------------------------
The RTPS Header must appear at the beginning of every message

The Header identifies the message as belonging to the RTPS protocol. The Header identifies the version of the protocol
and the vendor that sent the message. The Header contains the fields listed in Table 8.14.

- protocolId_ identifies the message as an RTPS message. This value is set to PROTOCOL_RTPS

- The vendorId_ identifies the vendor of the middleware that implemented the RTPS protocol and allows this vendor to add
specific extensions to the protocol. The vendorId does not refer to the vendor of the device or product that contains RTPS
middleware. The possible values for the vendorId are assigned by the OMG.

- protocolVersion_ identifies the version of the RTPS protocol. Implementations following this version of the document
implement protocol version 2.1 (major = 2, minor = 1) and have this field set to PROTOCOLVERSION_2_1.

- guidPrefix_ defines a default prefix that can be used to reconstruct the Globally Unique Identifiers (GUIDs) that
appear within the Submessages contained in the message. The guidPrefix allows Submessages to contain only the EntityId
part of the GUID and therefore saves from having to repeat the common prefix on every GUID (See Section 8.2.4.1).
------------------------------------------------------------------*/

class DLL_STATE MessageHeader : public NetworkLib::NetObjectBase
{
public:
	MessageHeader(const ProtocolId::Type &protocolId
				  , const ProtocolVersion &protocolVersion
				  , const VendorId &vendorId
				  , const GuidPrefix &guidPrefix);
	MessageHeader();
	virtual ~MessageHeader();

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const;
	virtual void Read(NetworkLib::SerializeReader *reader);

public:
	bool IsValid() const;

public:
	bool operator==(const MessageHeader &other) const
	{
		return (protocolId_ == other.protocolId_) &&
				(protocolVersion_ == other.protocolVersion_) &&
				(vendorId_ == other.vendorId_) &&
				(guidPrefix_ == other.guidPrefix_);
	}

	bool operator!=(const MessageHeader &other) const
	{
		return !(*this == other);
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int headerSize = 2 + ProtocolVersion::SIZE() + VendorId::SIZE() + GuidPrefix::SIZE();
		return headerSize;
	}

	static MessageHeader DEFAULT()
	{
		static MessageHeader messageHeader(ProtocolId::IntactProtocol,
										   ProtocolVersion::PROTOCOLVERSION(),
										   VendorId::UNKNOWN(),
										   GuidPrefix::UNKNOWN());
		return messageHeader;
	}

public:
	inline void				SetProtocolId(const ProtocolId::Type &id)		{ protocolId_ = id; }
	inline void 			SetProtocolVersion(const ProtocolVersion &ver)	{ protocolVersion_ = ver; }
	inline void				SetVendorId(const VendorId &id)					{ vendorId_ = id; }
    inline void				SetGuidPrefix(const GuidPrefix &guid)			{ guidPrefix_ = guid; }

	inline ProtocolId::Type	GetProtocolId() 		const { return protocolId_; }
	inline ProtocolVersion	GetProtocolVersion()	const { return protocolVersion_; }
	inline VendorId			GetVendorId()			const { return vendorId_; }
	inline GuidPrefix		GetGuidPrefix()			const { return guidPrefix_; }

private:
	ProtocolId::Type	protocolId_;
	ProtocolVersion 	protocolVersion_;
	VendorId			vendorId_;
	GuidPrefix			guidPrefix_;
};

} // namespace RTPS

#endif // Headers_MessageHeader_h_IsIncluded

