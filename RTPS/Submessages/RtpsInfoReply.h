/*
 * RtpsInfoReply.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsInfoReply_h_IsIncluded
#define RTPS_Submessages_RtpsInfoReply_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*----------------------------------------------------------------------------------------
This message is sent from an RTPS Reader to an RTPS Writer. It contains explicit information on where to send a reply
to the Submessages that follow it within the same message.
----------------------------------------------------------------------------------------*/

class DLL_STATE RtpsInfoReply : public RtpsSubmessageBase
{
public:
    RtpsInfoReply(const SubmessageHeader &header);
    RtpsInfoReply(const Locator &locator);
    RtpsInfoReply();
    virtual ~RtpsInfoReply();

    CLASS_TRAITS(RtpsInfoReply)

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        unicastLocatorList_.Write(writer);
        multicastLocatorList_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        unicastLocatorList_.Read(reader);
        multicastLocatorList_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
   /* -------------------------------------------------------------
    In addition to the EndiannessFlag, The InfoReply Submessage introduces the MulticastFlag (“Content” on page 46).
    The PSM maps the MulticastFlag flag into the 2nd least-significant bit (LSB) of the flags.
    The MulticastFlag is represented with the literal ‘M’. M=1 means the InfoReply also includes a multicastLocatorList.
    The value of the MulticastFlag can be obtained from the expression:

    M = SubmessageHeader.flags & 0x02
    -------------------------------------------------------------*/
    bool HasMulticastFlag() const;
    void SetMulticastFlag(bool flag);

public:
    const LocatorList&          GetUnicastLocatorList()     const { return unicastLocatorList_; }
    const LocatorList&          GetMulticastLocatorList()   const { return multicastLocatorList_; }

    void SetUnicastLocatorList(const LocatorList &l)    { unicastLocatorList_ = l; }
    void SetMulticastLocatorList(const LocatorList &l)  { multicastLocatorList_ = l; }

private:
    LocatorList         unicastLocatorList_;
    LocatorList         multicastLocatorList_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsInfoReply_h_IsIncluded
