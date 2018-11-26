/*
 * StatelessReader.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include"RTPS/Stateless/StatelessReader.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"
#include"RTPS/Elements/IncludeLibs.h"

namespace RTPS
{

/*-----------------------------------------------------------
	class StatelessReader
-----------------------------------------------------------*/
StatelessReader::StatelessReader()
{
}

StatelessReader::StatelessReader(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
	: RtpsReader(guid, topicKind, reliabilityKind)
{

}

StatelessReader::~StatelessReader()
{

}

/*-----------------------------------------------------------
This transition is triggered by the reception of a DATA message by the RTPS Reader ‘the_rtps_reader.’ The DATA
message encapsulates the change ‘a_change.’ The encapsulation is described in Section 8.3.7.2.
The stateless nature of the StatelessReader prevents it from maintaining the information required to determine the highest
sequence number received so far from the originating RTPS Writer. The consequence is that in those cases the
corresponding DDS DataReader may be presented duplicate or out-of order changes. Note that if the DDS DataReader is
configured to order data by ‘source timestamp,’ any available data will still be presented in-order when accessing the data
through the DDS DataReader.

As mentioned in Section 8.4.3, actual stateless implementations may try to avoid this limitation and maintain this
information in non-permanent fashion (using for example a cache that expires information after a certain time) to
approximate, to the extent possible, the behavior that would result if the state were maintained.

The transition performs the following logical actions in the virtual machine:

a_change := new CacheChange(DATA);
the_rtps_reader.reader_cache.add_change(a_change);
-----------------------------------------------------------*/
bool StatelessReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsData *data)
{
	GUID writerGuid(messageReceiver->GetSourceGuidPrefix(), data->GetWriterId());

	RtpsData::Ptr dataPtr(data);
	CacheChange::Ptr cacheChange = CacheChange::Ptr( new CacheChange(ChangeKindId::ALIVE, writerGuid, data->GetWriterSN(), dataPtr));
	readerCache_.AddChange(cacheChange);

	return true;
}

bool StatelessReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsGap *gap)
{
	ASSERT(messageReceiver);
	ASSERT(gap);

	ICRITICAL() << "GAP is not supported by " << typeid(this).name();
	return false;
}

bool StatelessReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsHeartbeat *heartbeat)
{
	ASSERT(messageReceiver);
	ASSERT(heartbeat);

	ICRITICAL() << "Heartbeat is not supported by " << typeid(this).name();
	return false;
}

} // namespace RTPS
