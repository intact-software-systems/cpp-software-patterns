/*
 * RtpsGap.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */
#ifndef RTPS_Submessages_RtpsGap_h_IsIncluded
#define RTPS_Submessages_RtpsGap_h_IsIncluded

#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/*-------------------------------------------------------------------------------------------------
    This Submessage is sent from an RTPS Writer to an RTPS Reader and indicates to the RTPS Reader that a range of
sequence numbers is no longer relevant. The set may be a contiguous range of sequence numbers or a specific set of
sequence numbers.
-------------------------------------------------------------------------------------------------*/

class DLL_STATE RtpsGap : public RtpsSubmessageBase
{
public:
    RtpsGap(const SubmessageHeader &header);
    RtpsGap();
    virtual ~RtpsGap();

    CLASS_TRAITS(RtpsGap)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        readerId_.Write(writer);
        writerId_.Write(writer);
        gapStart_.Write(writer);
        gapList_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        readerId_.Read(reader);
        writerId_.Read(reader);
        gapStart_.Read(reader);
        gapList_.Read(reader);
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    const EntityId& 			GetReaderId()			const { return readerId_; }
    const EntityId& 			GetWriterId()			const { return writerId_; }
    const SequenceNumber&		GetGapStart()			const { return gapStart_; }
    const SequenceNumberSet&	GetGapList()			const { return gapList_; }

    void SetReaderId(const EntityId &readerId)			{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  		{ writerId_ = writerId; }
    void SetGapStart(const SequenceNumber &seq)			{ gapStart_ = seq; }
    void SetGapList(const SequenceNumberSet &set)		{ gapList_ = set; }

public:
    static unsigned int SIZE()
    {
        static unsigned int headerSize = EntityId::SIZE() + EntityId::SIZE() + SequenceNumber::SIZE() + SequenceNumberSet::SIZE();
        return headerSize;
    }

private:
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber		gapStart_;
    SequenceNumberSet 	gapList_;
};

} // namespace RTPS // namespace NetworkLib

#endif // RTPS_Submessages_RtpsGap_h_IsIncluded
