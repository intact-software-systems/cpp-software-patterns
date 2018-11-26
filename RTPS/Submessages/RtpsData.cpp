/*
 * RtpsData.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsData.h"

namespace RTPS
{

/* -----------------------------------------------------------------------------------------
The RTPS Writer sends the Data Submessage to the RTPS Reader to communicate changes to the data-objects within the
writer. Changes include both changes in value as well as changes to the lifecycle of the data-object.

Changes to the value are communicated by the presence of the serializedPayload. When present, the serializedPayload is
interpreted either as the value of the data-object or as the key that uniquely identifies the data-object from the set of
registered objects.

- If the DataFlag is set and the KeyFlag is not set, the serializedPayload element is interpreted as the value of the dtatobject.
- If the KeyFlag is set and the DataFlag is not set, the serializedPayload element is interpreted as the value of the key
that identifies the registered instance of the data-object.

If the InlineQosFlag is set, the inlineQos element contains QoS values that override those of the RTPS Writer and should
be used to process the update. For a complete list of possible in-line QoS parameters, see Table 8.80.

The Writer is uniquely identified by its GUID. The Writer GUID is obtained using the state of the Receiver:

writerGUID = { Receiver.sourceGuidPrefix, Data.writerId }

The Reader is uniquely identified by its GUID. The Reader GUID is obtained using the state of the Receiver:

readerGUID = { Receiver.destGuidPrefix, Data.readerId }

The Data.readerId can be ENTITYID_UNKNOWN, in which case the Data applies to all Readers of that writerGUID
within the Participant identified by the GuidPrefix_t Receiver.destGuidPrefix.
--------------------------------------------------------------------------------------------*/

RtpsData::RtpsData(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::DATA);
}

RtpsData::RtpsData(const SerializedPayload &payload)
	: RtpsSubmessageBase()
	, serializedPayload_(payload)
{
	header_.SetSubmessageKind(SubmessageKind::DATA);
}

RtpsData::RtpsData()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::DATA);
}

RtpsData::~RtpsData()
{
}

/* ---------------------------------------------------------
	pure virtual functions reimplemented
---------------------------------------------------------*/
SubmessageKind::Type RtpsData::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::DATA);
	return header_.GetSubmessageKind();
}

int64 RtpsData::GetSubmessageSize() const
{
	int64 headerSize = 2 + 2
							+ EntityId::SIZE()
							+ EntityId::SIZE()
							+ SequenceNumber::SIZE()
							+ inlineQos_.GetSize()
							+ serializedPayload_.GetSize();
	return headerSize;
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
- submessageLength in the Submessage header is too small.
- writerSN.value is not strictly positive (1, 2, ...) or is SEQUENCENUMBER_UNKNOWN.
- inlineQos is invalid.
--------------------------------------------------*/
bool RtpsData::IsValid() const
{
	// TODO: Implement
	return true;
}

/* --------------------------------------------------
The InlineQosFlag is represented with the literal "Q"
Q=1 means that the Data Submessage contains the inlineQos
SubmessageElement.

The value of the InlineQosFlag can be obtained from the expression:
Q = SubmessageHeader.flags & DATA_INLINE_QOS_FLAG
----------------------------------------------------*/
bool RtpsData::HasInlineQos() const
{
	char qosFlag = (header_.GetSubmessageFlags() & DATA_INLINE_QOS_FLAG);

	return (qosFlag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ DATA_INLINE_QOS_FLAG;
set: 	flag = flag | DATA_INLINE_QOS_FLAG;
clear:	flag = flag & ~DATA_INLINE_QOS_FLAG;
----------------------------------------------------*/
void RtpsData::SetInlineQosFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | DATA_INLINE_QOS_FLAG;		// set bit to 1
		ASSERT((flag & DATA_INLINE_QOS_FLAG) == '1');
	}
	else
	{
		flag = flag & ~DATA_INLINE_QOS_FLAG;	// clear bit to 0
		ASSERT((flag & DATA_INLINE_QOS_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

/* --------------------------------------------------
The DataFlag is represented with the literal "D" The value of the DataFlag can be obtained from the expression.
D = SubmessageHeader.flags & 0x04

The KeyFlag is represented with the literal "K" The value of the KeyFlag can be obtained from the expression.
K = SubmessageHeader.flags & 0x08

The DataFlag is interpreted in combination with the KeyFlag as follows:
- D=0 and K=0 means that there is no serializedPayload SubmessageElement.
- D=1 and K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
- D=0 and K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
- D=1 and K=1 is an invalid combination in this version of the protocol.
----------------------------------------------------*/
bool RtpsData::HasSerializedPayload() const
{
	char dataFlag = (header_.GetSubmessageFlags() & DATA_SERIALIZED_DATA_FLAG);
	char keyFlag = (header_.GetSubmessageFlags() & DATA_KEY_DATA_FLAG);

	if(dataFlag == '0' && keyFlag == '0')
	{
		return false;
	}
	else if(dataFlag == '1' && keyFlag == '0')
	{
		return true;
	}
	else if(dataFlag == '0' && keyFlag == '1')
	{
		return true;
	}
	else if(dataFlag == '1' && keyFlag == '1')
	{
		ICRITICAL() << "Invalid combination of keys: Both dataFlag and keyFlag is set!";
	}
	else
	{
		ICRITICAL() << "Did not recognize dataFlag/keyFlag combination! " << dataFlag << " " << keyFlag;
	}

	return false;
}

bool RtpsData::HasSerializedData() const
{
	char dataFlag = (header_.GetSubmessageFlags() & DATA_SERIALIZED_DATA_FLAG);
	char keyFlag = (header_.GetSubmessageFlags() & DATA_KEY_DATA_FLAG);

	if(dataFlag == '1' && keyFlag == '0')
	{
		return true;
	}
	else if(dataFlag == '1' && keyFlag == '1')
	{
		ICRITICAL() << "Invalid combination of keys: Both dataFlag and keyFlag is set!";
	}

	return false;
}

/* --------------------------------------------------
toggle: flag = flag ^ DATA_SERIALIZED_DATA_FLAG;
set: 	flag = flag | DATA_SERIALIZED_DATA_FLAG;
clear:	flag = flag & ~DATA_SERIALIZED_DATA_FLAG;
----------------------------------------------------*/
void RtpsData::SetSerializedDataFlag(bool flagSet)
{
	SetSerializedKeyFlag(false);

	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | DATA_SERIALIZED_DATA_FLAG;	// set bit to 1
		ASSERT((flag & DATA_SERIALIZED_DATA_FLAG) == '1');
	}
	else
	{
		flag = flag & ~DATA_SERIALIZED_DATA_FLAG;	// clear bit to 0
		ASSERT((flag & DATA_SERIALIZED_DATA_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

bool RtpsData::HasSerializedKey() const
{
	char dataFlag = (header_.GetSubmessageFlags() & DATA_SERIALIZED_DATA_FLAG);
	char keyFlag = (header_.GetSubmessageFlags() & DATA_KEY_DATA_FLAG);

	if(dataFlag == '0' && keyFlag == '1')
	{
		return true;
	}
	else if(dataFlag == '1' && keyFlag == '1')
	{
		ICRITICAL() << "Invalid combination of keys: Both dataFlag and keyFlag is set!";
	}

	return false;
}

/* --------------------------------------------------
toggle: flag = flag ^ DATA_KEY_DATA_FLAG;
set: 	flag = flag | DATA_KEY_DATA_FLAG;
clear:	flag = flag & ~DATA_KEY_DATA_FLAG;
----------------------------------------------------*/
void RtpsData::SetSerializedKeyFlag(bool flagSet)
{
	SetSerializedDataFlag(false);

	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | DATA_KEY_DATA_FLAG;	// set bit to 1
		ASSERT((flag & DATA_KEY_DATA_FLAG) == '1');
	}
	else
	{
		flag = flag & ~DATA_KEY_DATA_FLAG;	// clear bit to 0
		ASSERT((flag & DATA_KEY_DATA_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}


} // namespace RTPS

