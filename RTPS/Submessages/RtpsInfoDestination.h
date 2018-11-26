/*
 * RtpsInfoDestination.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsInfoDestination_h_IsIncluded
#define RTPS_Submessages_RtpsInfoDestination_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*----------------------------------------------------------------------------------------
This message is sent from an RTPS Writer to an RTPS Reader to modify the GuidPrefix used to interpret the Reader
entityIds appearing in the Submessages that follow it.
----------------------------------------------------------------------------------------*/

class DLL_STATE RtpsInfoDestination : public RtpsSubmessageBase
{
public:
    RtpsInfoDestination(const SubmessageHeader &header);
    RtpsInfoDestination();
    virtual ~RtpsInfoDestination();

    CLASS_TRAITS(RtpsInfoDestination)

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        guidPrefix_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        guidPrefix_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    const GuidPrefix& 			GetGuidPrefix()			const { return guidPrefix_; }

    void SetGuidPrefix(const GuidPrefix &prefix)			{ guidPrefix_ = prefix; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = GuidPrefix::SIZE();
        return headerSize;
    }

private:
    GuidPrefix			guidPrefix_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsInfoDestination_h_IsIncluded
