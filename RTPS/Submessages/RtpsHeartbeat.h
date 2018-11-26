/*
 * RtpsHeartbeat.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsHeartbeat_h_IsIncluded
#define RTPS_Submessages_RtpsHeartbeat_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*----------------------------------------------------------------------------------------
This message is sent from an RTPS Writer to an RTPS Reader to communicate the sequence numbers of changes that the
Writer has available.
----------------------------------------------------------------------------------------*/
class DLL_STATE RtpsHeartbeat : public RtpsSubmessageBase
{
public:
    RtpsHeartbeat(const SubmessageHeader &header);
    RtpsHeartbeat();
    virtual ~RtpsHeartbeat();

    CLASS_TRAITS(RtpsHeartbeat)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        readerId_.Write(writer);
        writerId_.Write(writer);
        firstSN_.Write(writer);
        lastSN_.Write(writer);
        count_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        readerId_.Read(reader);
        writerId_.Read(reader);
        firstSN_.Read(reader);
        lastSN_.Read(reader);
        count_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    /* --------------------------------------------------
    HeartBeat Submessage introduces the FinalFlag and the LivelinessFlag ("Content" on page 46).
    The PSM maps the FinalFlag flag into the 2nd least-significant bit (LSB) of the flags and the
    LivelinessFlag into the 3rd least-significant bit (LSB) of the flags.
    The FinalFlag is represented with the literal "F".

    F=1 means the Writer does not require a response from the Reader.
    F=0 means the Reader must respond to the HeartBeat message.

    The value of the FinalFlag can be obtained from the expression:

    F = SubmessageHeader.flags & 0x02

    The LivelinessFlag is represented with the literal "L". L=1 means the DDS DataReader associated with the RTPS Reader
    should refresh the "manual" liveliness of the DDS DataWriter associated with the RTPS Writer of the message.
    The value of the LivelinessFlag can be obtained from the expression:

    L = SubmessageHeader.flags & 0x04
    ----------------------------------------------------*/
    bool HasFinalFlag() const;
    bool HasLivelinessFlag() const;

    void SetFinalFlag(bool flag);
    void SetLivelinessFlag(bool flag);

public:
    const EntityId& 			GetReaderId()			const { return readerId_; }
    const EntityId& 			GetWriterId()			const { return writerId_; }
    const SequenceNumber&		GetFirstSN()			const { return firstSN_; }
    const SequenceNumber&		GetLastSN()				const { return lastSN_; }
    const Count&				GetCount()				const { return count_; }

    void SetReaderId(const EntityId &readerId)			{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  		{ writerId_ = writerId; }
    void SetFirstSN(const SequenceNumber &seq)			{ firstSN_ = seq; }
    void SetLastSN(const SequenceNumber &seq)			{ lastSN_ = seq; }
    void SetCount(const Count &count)					{ count_ = count; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = EntityId::SIZE()
                                + EntityId::SIZE()
                                + SequenceNumber::SIZE()
                                + SequenceNumber::SIZE()
                                + Count::SIZE();
        return headerSize;
    }

private:
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber		firstSN_;
    SequenceNumber 		lastSN_;
    Count				count_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsHeartbeat_h_IsIncluded
