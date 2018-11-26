/*
 * RtpsHeartbeatFrag.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsHeartbeatFrag_h_IsIncluded
#define RTPS_Submessages_RtpsHeartbeatFrag_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*----------------------------------------------------------------------------------------
When fragmenting data and until all fragments are available, the HeartbeatFrag Submessage is sent from an RTPS
Writer to an RTPS Reader to communicate which fragments the Writer has available. This enables reliable
communication at the fragment level.

Once all fragments are available, a regular Heartbeat message is used.
----------------------------------------------------------------------------------------*/

class DLL_STATE RtpsHeartbeatFrag : public RtpsSubmessageBase
{
public:
    RtpsHeartbeatFrag(const SubmessageHeader &header);
    RtpsHeartbeatFrag();
    virtual ~RtpsHeartbeatFrag();

    CLASS_TRAITS(RtpsHeartbeatFrag)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        readerId_.Write(writer);
        writerId_.Write(writer);
        writerSN_.Write(writer);
        lastFragmentNum_.Write(writer);
        count_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        readerId_.Read(reader);
        writerId_.Read(reader);
        writerSN_.Read(reader);
        lastFragmentNum_.Read(reader);
        count_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    const EntityId& 			GetReaderId()			const { return readerId_; }
    const EntityId& 			GetWriterId()			const { return writerId_; }
    const SequenceNumber&		GetWriterSN()			const { return writerSN_; }
    const FragmentNumber&		GetLastFragmentNum()	const { return lastFragmentNum_; }
    const Count&				GetCount()				const { return count_; }

    void SetReaderId(const EntityId &readerId)			{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  		{ writerId_ = writerId; }
    void SetWriterSN(const SequenceNumber &seq)			{ writerSN_ = seq; }
    void SetLastFragmentNum(const FragmentNumber &seq)	{ lastFragmentNum_ = seq; }
    void SetCount(const Count &count)					{ count_ = count; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = EntityId::SIZE()
                                        + EntityId::SIZE()
                                        + SequenceNumber::SIZE()
                                        + FragmentNumber::SIZE()
                                        + Count::SIZE();
        return headerSize;
    }

private:
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber		writerSN_;
    FragmentNumber 		lastFragmentNum_;
    Count				count_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsHeartbeatFrag_h_IsIncluded
