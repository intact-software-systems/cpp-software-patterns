/*
 * RtpsAckNack.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsAckNack_h_IsIncluded
#define RTPS_Submessages_RtpsAckNack_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/* ------------------------------------------------------------------------
This Submessage is used to communicate the state of a Reader to a Writer.
The Submessage allows the Reader to inform the Writer about the sequence
numbers it has received and which ones it is still missing. This Submessage
can be used to do both positive and negative acknowledgments.

The Reader sends the AckNack message to the Writer to communicate its state
with respect to the sequence numbers used by the Writer.

The Writer is uniquely identified by its GUID. The Writer GUID is obtained
using the state of the Receiver:

writerGUID = { Receiver.destGuidPrefix, AckNack.writerId }

The Reader is uniquely identified by its GUID. The Reader GUID is obtained
using the state of the Receiver:

readerGUID = { Receiver.sourceGuidPrefix, AckNack.readerId }

The message serves two purposes simultaneously:
• The Submessage acknowledges all sequence numbers up to and including the
one just before the lowest sequence number in the SequenceNumberSet (that is
readerSNState.base -1).
• The Submessage negatively-acknowledges (requests) the sequence numbers that
appear explicitly in the set.

The mechanism to explicitly represent sequence numbers depends on the PSM.
Typically, a compact representation (such as a bitmap) is used.
The FinalFlag indicates whether a response by the Writer is expected by the
Reader or if the decision is left to the Writer. The use of this flag is
described in Section 8.4.
--------------------------------------------------------------------------*/
class DLL_STATE RtpsAckNack : public RtpsSubmessageBase
{
public:
    RtpsAckNack(const SubmessageHeader &header);
    RtpsAckNack();
    virtual ~RtpsAckNack();

    CLASS_TRAITS(RtpsAckNack)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        readerId_.Write(writer);
        writerId_.Write(writer);
        readerSNState_.Write(writer);
        count_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        readerId_.Read(reader);
        writerId_.Read(reader);
        readerSNState_.Read(reader);
        count_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    /* --------------------------------------------------
     The FinalFlag is represented with the literal ‘F’. F=1 means the reader does not require a
     response from the writer. F=0 means the writer must respond to the AckNack message.
     ----------------------------------------------------*/
    bool HasFinalFlag() const;
    void SetFinalFlag(bool flag);

public:
    const EntityId& 			GetReaderId()			const { return readerId_; }
    const EntityId& 			GetWriterId()			const { return writerId_; }
    const SequenceNumberSet&	GetReaderSNState()		const { return readerSNState_; }
    const Count&				GetCount()				const { return count_; }

    void SetReaderId(const EntityId &readerId)			{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  		{ writerId_ = writerId; }
    void SetReaderSNState(const SequenceNumberSet &set)	{ readerSNState_ = set; }
    void SetCount(const Count &count)					{ count_ = count; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = EntityId::SIZE() + EntityId::SIZE() + SequenceNumberSet::SIZE() + Count::SIZE();
        return headerSize;
    }

private:
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumberSet 	readerSNState_;
    Count				count_;
};

} // namespace RTPS // namespace NetworkLib

#endif // RTPS_Submessages_RtpsAckNack_h_IsIncluded
