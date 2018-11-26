/*
 * RtpsEndpoint.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Structure/RtpsEndpoint.h"

namespace RTPS
{

RtpsEndpoint::RtpsEndpoint()
	: RtpsEntity()
	, topicKind_(TopicKindId::NoKey)
	, reliabilityKind_(ReliabilityKindId::BestEffort)
{

}

RtpsEndpoint::RtpsEndpoint(const GUID &guid)
	: RtpsEntity(guid)
	, topicKind_(TopicKindId::NoKey)
	, reliabilityKind_(ReliabilityKindId::BestEffort)
{

}

RtpsEndpoint::RtpsEndpoint(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
	: RtpsEntity(guid)
	, topicKind_(topicKind)
	, reliabilityKind_(reliabilityKind)
{

}

RtpsEndpoint::~RtpsEndpoint()
{

}

} // namespace RTPS
