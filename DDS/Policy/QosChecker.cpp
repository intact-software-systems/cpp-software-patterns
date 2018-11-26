#include "DDS/Policy/QosChecker.h"

namespace DDS { namespace Policy {

QosChecker::QosChecker()
{
}

bool QosChecker::IsConsistent(const TopicQoS &topicQos)
{
    // TODO: Check qos
    return true;
}

bool QosChecker::IsCompatible(const TopicQoS &offeredTopicQos, const TopicQoS &requestedTopicQos)
{

    return true;
}

bool QosChecker::IsChangeable(const TopicQoS &offeredTopicQos, const TopicQoS &requestedTopicQos)
{

    return true;
}

}}
