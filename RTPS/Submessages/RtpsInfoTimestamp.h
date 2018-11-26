/*
 * RtpsInfoTimestamp.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsInfoTimestamp_h_IsIncluded
#define RTPS_Submessages_RtpsInfoTimestamp_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*----------------------------------------------------------------------------------------
This Submessage is used to send a timestamp which applies to the Submessages that follow within the same message.
----------------------------------------------------------------------------------------*/
class DLL_STATE RtpsInfoTimestamp : public RtpsSubmessageBase
{
public:
    RtpsInfoTimestamp(const SubmessageHeader &header);
    RtpsInfoTimestamp();
    virtual ~RtpsInfoTimestamp();

    CLASS_TRAITS(RtpsInfoTimestamp)

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        timestamp_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        timestamp_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    /* --------------------------------------------------
    The InfoTimestamp Submessage introduces the InvalidateFlag (“Content” on
    page 46). The PSM maps the InvalidateFlag flag into the 2nd least-significant bit (LSB) of the flags.
    The InvalidateFlag is represented with the literal ‘I’.

    I=0 means the InfoTimestamp also includes a timestamp.
    I=1 means subsequent Submessages should not be considered to have a valid timestamp.

    The value of the InvalidateFlag can be obtained from the expression:

    I = SubmessageHeader.flags & 0x02
    ----------------------------------------------------*/
    bool HasInvalidateFlag() const;
    void SetInvalidateFlag(bool flag);

public:
    const Timestamp& 			GetTimestamp()				const { return timestamp_; }

    void SetTimestamp(const Timestamp &t)					{ timestamp_ = t; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = Timestamp::SIZE();
        return headerSize;
    }

private:
    Timestamp			timestamp_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsInfoTimestamp_h_IsIncludeds
