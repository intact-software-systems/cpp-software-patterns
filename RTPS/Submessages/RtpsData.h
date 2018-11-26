/*
 * RtpsData.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsData_h_IsIncluded
#define RTPS_Submessages_RtpsData_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Submessages/RtpsSubmessageBase.h"
#include"RTPS/Headers/SubmessageHeader.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

/* -----------------------------------------------------------------------------------------
This Submessage is sent from an RTPS Writer (NO_KEY_ or WITH_KEY) to an RTPS Reader (NO_KEY or WITH_KEY).

The Submessage notifies the RTPS Reader of a change to a data-object belonging to the RTPS Writer. The possible
changes include both changes in value as well as changes to the lifecycle of the data-object.
--------------------------------------------------------------------------------------------*/
class DLL_STATE RtpsData : public RtpsSubmessageBase
{
public:
    RtpsData(const SubmessageHeader &header);
    RtpsData(const SerializedPayload &payload);
    RtpsData();
    virtual ~RtpsData();

    CLASS_TRAITS(RtpsData)

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
        unsigned short octetsToInlineQos_ = reader->ReadInt16();
        readerId_.Read(reader);
        writerId_.Read(reader);
        writerSN_.Read(reader);

        if(HasInlineQos())
        {
            inlineQos_.Read(reader);
        }

        if(HasSerializedPayload())
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
    The DataFlag is represented with the literal ‘D.’ The value of the DataFlag can be obtained from the expression.
    D = SubmessageHeader.flags & 0x04

    The KeyFlag is represented with the literal ‘K.’ The value of the KeyFlag can be obtained from the expression.
    K = SubmessageHeader.flags & 0x08

    The DataFlag is interpreted in combination with the KeyFlag as follows:
    - D=0 and K=0 means that there is no serializedPayload SubmessageElement.
    - D=1 and K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
    - D=0 and K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
    - D=1 and K=1 is an invalid combination in this version of the protocol.
    ----------------------------------------------------*/
    bool HasSerializedPayload() const;
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

    void SetReaderId(const EntityId &readerId)				{ readerId_ = readerId; }
    void SetWriterId(const EntityId &writerId)  			{ writerId_ = writerId; }
    void SetWriterSN(const SequenceNumber &seq)				{ writerSN_ = seq; }
    void SetInlineQos(const ParameterList &qos)				{ inlineQos_ = qos; }
    void SetSerializedPayload(const SerializedPayload &p) 	{ serializedPayload_ = p; }

public:
    /*static unsigned int SIZE()
    {
        static unsigned int headerSize = 2 + 2
                                    + EntityId::SIZE()
                                    + EntityId::SIZE()
                                    + SequenceNumber::SIZE()
                                    + ParameterList::SIZE()
                                    + SerializedPayload::SIZE();
        return headerSize;
    }*/


private:
    unsigned short 		extraFlags_;
//	unsigned short 		octetsToInlineQos_;
    EntityId			readerId_;
    EntityId 			writerId_;
    SequenceNumber 		writerSN_;
    ParameterList		inlineQos_;
    SerializedPayload	serializedPayload_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsData_h_IsIncluded
