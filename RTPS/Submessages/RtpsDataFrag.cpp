/*
 * RtpsDataFrag.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include "RTPS/Submessages/RtpsDataFrag.h"

namespace RTPS
{
/* -----------------------------------------------------------------------------------------
The DataFrag Submessage extends the Data Submessage by enabling the serializedData to be fragmented and sent as
multiple DataFrag Submessages. Once the serializedData is re-assembled by the RTPS Reader, the interpretation of the
DataFrag Submessages is identical to that of the Data Submessage.

How to re-assemble serializedData using the information in the DataFrag Submessage is described below.

The total size of the data to be re-assembled is given by dataSize. Each DataFrag Submessage contains a contiguous
segment of this data in its serializedData element. The size of the segment is determined by the size of the serializedData
element. During re-assembly, the offset of each segment is determined by:

(fragmentStartingNum - 1) * fragmentSize

The data is fully re-assembled when all fragments have been received. The total number of fragments to expect equals:

dataSize / fragmentSize + (dataSize % fragmentSize) ? 1 : 0

Note that each DataFrag Submessage may contain multiple fragments. An RTPS Writer will select fragmentSize based
on the smallest message size supported across all underlying transports. If some RTPS Readers can be reached across a
transport that supports larger messages, the RTPS Writer can pack multiple fragments into a single DataFrag
Submessage or may even send a regular Data Submessage if fragmentation is no longer required. For more details, see
Section 8.4.14.1.
--------------------------------------------------------------------------------------------*/
RtpsDataFrag::RtpsDataFrag(const SubmessageHeader &header)
	: RtpsSubmessageBase(header)
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::DATA_FRAG);
}

RtpsDataFrag::RtpsDataFrag()
	: RtpsSubmessageBase()
{
	header_.SetSubmessageKind(SubmessageKind::DATA_FRAG);
}

RtpsDataFrag::~RtpsDataFrag()
{
}

SubmessageKind::Type RtpsDataFrag::GetSubmessageKind() const
{
	ASSERT(header_.GetSubmessageKind() == SubmessageKind::DATA_FRAG);
	return header_.GetSubmessageKind();
}

int64 RtpsDataFrag::GetSubmessageSize() const
{
	int64 headerSize = 2 + 2
							+ EntityId::SIZE()
							+ EntityId::SIZE()
							+ SequenceNumber::SIZE()
							+ FragmentNumber::SIZE()
							+ 2 + 2 + 4
							+ inlineQos_.GetSize()
							+ serializedPayload_.GetSize();
	return headerSize;
}

/*--------------------------------------------------
This Submessage is invalid when any of the following is true:
 submessageLength in the Submessage header is too small.
 writerSN.value is not strictly positive (1, 2, ...) or is SEQUENCENUMBER_UNKNOWN.
 fragmentStartingNum.value is not strictly positive (1, 2, ...) or exceeds the total number of fragments (see below).
 fragmentSize exceeds dataSize.
 The size of serializedData exceeds fragmentsInSubmessage * fragmentSize.
 inlineQos is invalid.
--------------------------------------------------*/
bool RtpsDataFrag::IsValid() const
{
	// TODO: Implement
	return true;
}

/* ----------------------------------------------------------------
In addition to the EndiannessFlag, The DataFrag Submessage introduces the KeyFlag and InlineQosFlag (see
“Contents” on page 48). The PSM maps these flags as follows:

The InlineQosFlag is represented with the literal ‘Q’. Q=1 means that the DataFrag Submessage contains the inlineQos
SubmessageElement.

The value of the InlineQosFlag can be obtained from the expression:

Q = SubmessageHeader.flags & 0x02

The KeyFlag is represented with the literal ‘K.’

The value of the KeyFlag can be obtained from the expression:

K = SubmessageHeader.flags & 0x04

K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
K=1 means that the serializedPayload SubmessageElement contains the serialized Key.
----------------------------------------------------------------  */
bool RtpsDataFrag::HasInlineQos() const
{
	char qosFlag = (header_.GetSubmessageFlags() & DATAFRAG_INLINE_QOS_FLAG);

	return (qosFlag == '1');
}

/* --------------------------------------------------
toggle: flag = flag ^ DATAFRAG_INLINE_QOS_FLAG;
set: 	flag = flag | DATAFRAG_INLINE_QOS_FLAG;
clear:	flag = flag & ~DATAFRAG_INLINE_QOS_FLAG;
----------------------------------------------------*/
void RtpsDataFrag::SetInlineQosFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)
	{
		flag = flag | DATAFRAG_INLINE_QOS_FLAG;		// set bit to 1
		ASSERT((flag & DATAFRAG_INLINE_QOS_FLAG) == '1');
	}
	else
	{
		flag = flag & ~DATAFRAG_INLINE_QOS_FLAG;	// clear bit to 0
		ASSERT((flag & DATAFRAG_INLINE_QOS_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

bool RtpsDataFrag::HasSerializedData() const
{
	char keyFlag = (header_.GetSubmessageFlags() & 0x04);

	if(keyFlag == '0')
	{
		return true;
	}
	return false;
}

/* --------------------------------------------------
The value of the KeyFlag can be obtained from the expression:

K = SubmessageHeader.flags & 0x04

K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
K=1 means that the serializedPayload SubmessageElement contains the serialized Key.

toggle: flag = flag ^ DATAFRAG_SERIALIZED_KEY_FLAG;
set: 	flag = flag | DATAFRAG_SERIALIZED_KEY_FLAG;
clear:	flag = flag & ~DATAFRAG_SERIALIZED_KEY_FLAG;
----------------------------------------------------*/
void RtpsDataFrag::SetSerializedDataFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == false)	// unsets data flag by setting key-bit to 1
	{
		flag = flag | DATAFRAG_SERIALIZED_KEY_FLAG;		// set bit to 1
		ASSERT((flag & DATAFRAG_SERIALIZED_KEY_FLAG) == '1');
	}
	else	// sets data flag by setting bit to 0
	{
		flag = flag & ~DATAFRAG_SERIALIZED_KEY_FLAG;	// clear bit to 0
		ASSERT((flag & DATAFRAG_SERIALIZED_KEY_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}


bool RtpsDataFrag::HasSerializedKey() const
{
	char keyFlag = (header_.GetSubmessageFlags() & 0x04);

	if(keyFlag == '1')
	{
		return true;
	}
	return false;
}
/* --------------------------------------------------
The value of the KeyFlag can be obtained from the expression:

K = SubmessageHeader.flags & 0x04

K=0 means that the serializedPayload SubmessageElement contains the serialized Data.
K=1 means that the serializedPayload SubmessageElement contains the serialized Key.

toggle: flag = flag ^ DATAFRAG_SERIALIZED_KEY_FLAG;
set: 	flag = flag | DATAFRAG_SERIALIZED_KEY_FLAG;
clear:	flag = flag & ~DATAFRAG_SERIALIZED_KEY_FLAG;
----------------------------------------------------*/
void RtpsDataFrag::SetSerializedKeyFlag(bool flagSet)
{
	char flag = header_.GetSubmessageFlags();

	if(flagSet == true)	// sets key flag by setting key-bit to 1
	{
		flag = flag | DATAFRAG_SERIALIZED_KEY_FLAG;		// set bit to 1
		ASSERT((flag & DATAFRAG_SERIALIZED_KEY_FLAG) == '1');
	}
	else	// sets data flag by setting bit to 0
	{
		flag = flag & ~DATAFRAG_SERIALIZED_KEY_FLAG;	// clear bit to 0
		ASSERT((flag & DATAFRAG_SERIALIZED_KEY_FLAG) == '0');
	}

	header_.SetSubmessageFlags(flag);
}

} // namespace RTPS

