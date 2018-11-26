/*
 * DataRepresentation.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_QosChecker_h_Included
#define DDS_Policy_QosChecker_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/TopicQoS.h"
#include"DDS/PublisherQoS.h"
#include"DDS/DataWriterQoS.h"
#include"DDS/SubscriberQoS.h"
#include"DDS/DataReaderQoS.h"
#include"DDS/DomainParticipantQoS.h"
#include"DDS/Policy/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy {

/**
 @brief Helper class to check consistency of QoS
*/
class DLL_STATE QosChecker
{
private:
    QosChecker();

public:
    static bool IsConsistent(const TopicQoS &topicQos);
    static bool IsCompatible(const TopicQoS &offeredTopicQos, const TopicQoS &requestedTopicQos);
    static bool IsChangeable(const TopicQoS &offeredTopicQos, const TopicQoS &requestedTopicQos);

};

}} // DDS::Policy

#endif //DDS_Policy_QosChecker_h_Included
