/*
 * RtpsNackFrag.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsNackFrag_h_IsIncluded
#define RTPS_Submessages_RtpsNackFrag_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*----------------------------------------------------------------------------------------
The NackFrag Submessage is used to communicate the state of a Reader to a Writer. When a data change is sent as a
series of fragments, the NackFrag Submessage allows the Reader to inform the Writer about specific fragment numbers
it is still missing.

This Submessage can only contain negative acknowledgements. Note this differs from an AckNack Submessage, which
includes both positive and negative acknowledgements. The advantages of this approach include:

 It removes the windowing limitation introduced by the AckNack Submessage.
Given the size of a SequenceNumberSet is limited to 256, an AckNack Submessage is limited to NACKing only those
samples whose sequence number does not not exceed that of the first missing sample by more than 256. Any samples
below the first missing samples are acknowledged.
NackFrag Submessages on the other hand can be used to NACK any fragment numbers, even fragments more than
256 apart from those NACKed in an earlier AckNack Submessage. This becomes important when handling samples
containing a large number of fragments.

 Fragments can be negatively acknowledged in any order.
----------------------------------------------------------------------------------------*/
class DLL_STATE RtpsNackFrag : public RtpsSubmessageBase
{
public:
    RtpsNackFrag(const SubmessageHeader &header);
    RtpsNackFrag();
    virtual ~RtpsNackFrag();

    CLASS_TRAITS(RtpsNackFrag)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        readerId_.Write(writer);
        writerId_.Write(writer);
        writerSN_.Write(writer);
        fragmentNumberState_.Write(writer);
        count_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        readerId_.Read(reader);
        writerId_.Read(reader);
        writerSN_.Read(reader);
        fragmentNumberState_.Read(reader);
        count_.Read(reader);
    }

public:
    virtual SubmessageKind::Type GetSubmessageKind() const;
    virtual int64                GetSubmessageSize() const;
    virtual bool				 IsValid() const;

public:
    const EntityId& 			GetReaderId()					const { return readerId_; }
    const EntityId& 			GetWriterId()					const { return writerId_; }
    const SequenceNumber&		GetWriterSN()					const { return writerSN_; }
    const FragmentNumberSet&	GetFragmentNumberState()		const { return fragmentNumberState_; }
    const Count&				GetCount()						const { return count_; }

    void SetReaderId(const EntityId &readerId)					{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  				{ writerId_ = writerId; }
    void SetWriterSN(const SequenceNumber &seq)					{ writerSN_ = seq; }
    void SetFragmentNumberState(const FragmentNumberSet &seq)	{ fragmentNumberState_ = seq; }
    void SetCount(const Count &count)							{ count_ = count; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = EntityId::SIZE() + EntityId::SIZE() + SequenceNumber::SIZE() + FragmentNumberSet::SIZE() + Count::SIZE();
        return headerSize;
    }

private:
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber 		writerSN_;
    FragmentNumberSet	fragmentNumberState_;
    Count				count_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsNackFrag_h_IsIncluded
