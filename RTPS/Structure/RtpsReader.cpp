/*
 * RtpsReader.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Structure/RtpsReader.h"

namespace RTPS
{
/*-------------------------------------------------------------------------
constructor and destructor
-------------------------------------------------------------------------*/
RtpsReader::RtpsReader()
	: RtpsEndpoint()
	, expectsInlineQos_(true)
	, heartBeatResponseDelay_(Duration::FromMilliseconds(500))
	, heartBeatSuppressionDuration_(0)
{

}

RtpsReader::RtpsReader(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
	: RtpsEndpoint(guid, topicKind, reliabilityKind)
	, expectsInlineQos_(true)
	, heartBeatResponseDelay_(Duration::FromMilliseconds(500))
	, heartBeatSuppressionDuration_(0)
{

}

RtpsReader::~RtpsReader()
{
}

/*-------------------------------------------------------------------------
initializes message sender - called from virtual machine
-------------------------------------------------------------------------*/
void RtpsReader::SetMessageSender(const RtpsMessageSender::Ptr &messageSender)
{
	messageSender_ = messageSender;
}

} // namespace RTPS
