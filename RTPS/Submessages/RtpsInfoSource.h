/*
 * RtpsInfoSource.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsInfoSource_h_IsIncluded
#define RTPS_Submessages_RtpsInfoSource_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*----------------------------------------------------------------------------------------
This message modifies the logical source of the Submessages that follow.
----------------------------------------------------------------------------------------*/
class DLL_STATE RtpsInfoSource : public RtpsSubmessageBase
{
public:
    RtpsInfoSource(const SubmessageHeader &header);
    RtpsInfoSource();
    virtual ~RtpsInfoSource();

    CLASS_TRAITS(RtpsInfoSource)

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32(unused_);
        protocolVersion_.Write(writer);
        vendorId_.Write(writer);
        guidPrefix_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        unused_ = reader->ReadInt32();
        protocolVersion_.Read(reader);
        vendorId_.Read(reader);
        guidPrefix_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    const ProtocolVersion&		GetProtocolVersion()		const { return protocolVersion_; }
    const VendorId&				GetVendorId()				const { return vendorId_; }
    const GuidPrefix&			GetGuidPrefix()				const { return guidPrefix_; }

    void SetProtocolVersion(const ProtocolVersion &ver)		{ protocolVersion_ = ver; }
    void SetVendorId(const VendorId &id)					{ vendorId_ = id; }
    void SetGuidPrefix(const GuidPrefix &guid)				{ guidPrefix_ = guid; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = 4 + ProtocolVersion::SIZE() + VendorId::SIZE() + GuidPrefix::SIZE();
        return headerSize;
    }

private:
    long				unused_;
    ProtocolVersion 	protocolVersion_;
    VendorId			vendorId_;
    GuidPrefix			guidPrefix_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsInfoSource_h_IsIncluded
