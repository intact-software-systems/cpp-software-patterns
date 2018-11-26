/*
 * TopicListener.h
 *
 *  Created on: Sep 13, 2012
 *      Author: knuthelv
 */

#ifndef DCPS_Topic_TopicListener_h_IsIncluded
#define DCPS_Topic_TopicListener_h_IsIncluded

#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Topic/TopicFwd.h"

namespace DCPS { namespace Topic
{

/*----------------------------------------------------------------------------------
 Since Topic is a kind of Entity, it has the ability to have an associated listener.
 In this case, the associated listener should be of concrete type TopicListener.
----------------------------------------------------------------------------------*/
template <typename DATA>
class TopicListener : public DCPS::Infrastructure::Listener
{
public:
    virtual ~TopicListener() {}

    CLASS_TRAITS(TopicListener)

    virtual void OnInconsistentTopic(typename DCPS::Topic::Topic<DATA> *topic,
                                     const Status::InconsistentTopicStatus &status) = 0;
};

template <typename DATA>
class NoOpTopicListener : public TopicListener<DATA>
{
public:
    virtual ~NoOpTopicListener() {}

    CLASS_TRAITS(NoOpTopicListener)

    virtual void OnInconsistentTopic(typename DCPS::Topic::Topic<DATA> *topic,
                                     const Status::InconsistentTopicStatus &status) { }
};

}}

#endif
