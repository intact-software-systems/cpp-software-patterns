/*
 * RtpsPad.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsPad_h_IsIncluded
#define RTPS_Submessages_RtpsPad_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*----------------------------------------------------------------------------------------
The purpose of this Submessage is to allow the introduction of any padding necessary to meet any desired
memory-alignment requirements. It has no other meaning.
----------------------------------------------------------------------------------------*/
class DLL_STATE RtpsPad : public RtpsSubmessageBase
{
public:
    RtpsPad(const SubmessageHeader &header);
    RtpsPad(unsigned int submessageSize);
    virtual ~RtpsPad();

    CLASS_TRAITS(RtpsPad)

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        ASSERT(writer);
        // Do nothing?
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        ASSERT(reader);
        // Do nothing?
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

private:
    unsigned int submessageSize_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsPad_h_IsIncluded
