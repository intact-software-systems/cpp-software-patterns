/*
 * RtpsDataFrag.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsDataFrag_h_IsIncluded
#define RTPS_Submessages_RtpsDataFrag_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/Export.h"
namespace RTPS
{

/* -----------------------------------------------------------------------------------------
This Submessage is sent from an RTPS Writer (NO_KEY or WITH_KEY) to an RTPS Reader (NO_KEY or
WITH_KEY).

The DataFrag Submessage extends the Data Submessage by enabling the serializedData to be fragmented and sent as
multiple DataFrag Submessages. The fragments contained in the DataFrag Submessages are then re-assembled by
the RTPS Reader.

Defining a separate DataFrag Submessage in addition to the Data Submessage, offers the following advantages:

• It keeps variations in contents and structure of each Submessage to a minimum. This enables more efficient
implementations of the protocol as the parsing of network packets is simplified.
• It avoids having to add fragmentation information as in-line QoS parameters in the Data Submessage. This may not
only slow down performance, it also makes on-the-wire debugging more difficult, as it is no longer obvious whether
data is fragmented or not and which message contains what fragment(s).
--------------------------------------------------------------------------------------------*/

class DLL_STATE RtpsDataFrag : public RtpsSubmessageBase
{
public:
    RtpsDataFrag(const SubmessageHeader &header);
    RtpsDataFrag();
    virtual ~RtpsDataFrag();

    CLASS_TRAITS(RtpsDataFrag)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt16(extraFlags_);

        // TODO: Define size separately. Not hardcoded!
        unsigned short octetsToInlineQos = 32 * 3;
        writer->WriteInt16(octetsToInlineQos);
        readerId_.Write(writer);
        writerId_.Write(writer);
        writerSN_.Write(writer);

        fragmentStartingNum_.Write(writer);
        writer->WriteInt16(fragmentsInSubmessage_);
        writer->WriteInt16(fragmentSize_);
        writer->WriteInt32(sampleSize_);

        if(!inlineQos_.IsEmpty())
        {
            inlineQos_.Write(writer);
        }

        // TODO: If and only if
        // header_ data flag
        // D = SubmessageHeader.flags & 0x04
        // or header_ key flag
        // K = SubmessageHeader.flags & 0x08
        serializedPayload_.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        extraFlags_ = reader->ReadInt16();
        unsigned short octetsToInlineQos = reader->ReadInt16();
        readerId_.Read(reader);
        writerId_.Read(reader);
        writerSN_.Read(reader);

        fragmentStartingNum_.Read(reader);
        fragmentsInSubmessage_ = reader->ReadInt16();
        fragmentSize_ = reader->ReadInt16();
        sampleSize_ = reader->ReadInt16();

        if(HasInlineQos())
        {
            inlineQos_.Read(reader);
        }

        if(HasSerializedData() || HasSerializedKey())
        {
            serializedPayload_.Read(reader);
        }
    }

public:
    virtual SubmessageKind::Type	GetSubmessageKind() const;
    virtual int64                   GetSubmessageSize() const;
    virtual bool					IsValid() const;

public:
    /* --------------------------------------------------
    The InlineQosFlag is represented with the literal ‘Q.’
    Q=1 means that the Data Submessage contains the inlineQos
    SubmessageElement.

    The value of the InlineQosFlag can be obtained from the expression:
    Q = SubmessageHeader.flags & 0x02
    ----------------------------------------------------*/
    bool HasInlineQos() const;
    void SetInlineQosFlag(bool flag);

    /* --------------------------------------------------
    The KeyFlag is represented with the literal ‘K.’
    The value of the KeyFlag can be obtained from the expression:

    K = SubmessageHeader.flags & 0x04

    K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
    K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
    ----------------------------------------------------*/
    bool HasSerializedData() const;
    bool HasSerializedKey() const;

    void SetSerializedDataFlag(bool flag);
    void SetSerializedKeyFlag(bool flag);

public:
    const EntityId& 			GetReaderId()					const { return readerId_; }
    const EntityId& 			GetWriterId()					const { return writerId_; }
    const SequenceNumber&		GetWriterSN()					const { return writerSN_; }
    const ParameterList&		GetInlineQos()					const { return inlineQos_; }
    const SerializedPayload&	GetSerializedPayload()			const { return serializedPayload_; }
    const FragmentNumber&		GetFragmentStartingNum()		const { return fragmentStartingNum_; }
    unsigned short				GetFragmentsInSubmessage()		const { return fragmentsInSubmessage_; }
    unsigned short				GetFragmentSize()				const { return fragmentSize_; }
    int64				        GetSampleSize()					const { return sampleSize_; }

    void SetReaderId(const EntityId &readerId)				{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  			{ writerId_ = writerId; }
    void SetWriterSN(const SequenceNumber &seq)				{ writerSN_ = seq; }
    void SetInlineQos(const ParameterList &qos)				{ inlineQos_ = qos; }
    void SetSerializedPayload(const SerializedPayload &p) 	{ serializedPayload_ = p; }
    void SetFragmentStartingNum(const FragmentNumber &f)	{ fragmentStartingNum_ = f; }
    void SetFragmentsInSubmessage(unsigned short f)			{ fragmentsInSubmessage_ = f; }
    void SetFragmentSize(unsigned short f)					{ fragmentSize_ = f; }
    void SetSamepleSize(int64 s)					        { sampleSize_ = s; }

private:
    unsigned short 		extraFlags_;
//	unsigned short 		octetsToInlineQos_;
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber 		writerSN_;

    // -- start special for fragments
    FragmentNumber		fragmentStartingNum_;
    unsigned short		fragmentsInSubmessage_;
    unsigned short		fragmentSize_;
    int64		        sampleSize_;
    // -- end special for fragments

    ParameterList		inlineQos_;
    SerializedPayload	serializedPayload_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsDataFrag_h_IsIncluded
