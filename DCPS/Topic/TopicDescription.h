/*
 * TopicDescription.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Topic_TopicDescription_h_IsIncluded
#define DCPS_Topic_TopicDescription_h_IsIncluded

#include"DCPS/Topic/TopicHolder.h"

namespace DCPS { namespace Topic
{
/**
 * @brief This class is an abstract class. It is the base class for Topic, ContentFilteredTopic, and MultiTopic.
 */
template <typename DATA>
class TopicDescription : public TopicHolder
{
public:
    TopicDescription()
        : participant_()
        , topicName_()
        , typeName_()
    { }

    TopicDescription(const Domain::DomainParticipantPtr &participant,
                     const DDS::Elements::TopicName &topicName,
                     const DDS::Elements::TypeName &typeName)
        : participant_(participant)
        , topicName_(topicName)
        , typeName_(typeName)
    { }

    virtual ~TopicDescription()
    { }

    CLASS_TRAITS(TopicDescription)

public:
    void SetParticipant(const Domain::DomainParticipantPtr &p)      { participant_ = p; }
    void SetTopicName(DDS::Elements::TopicName name)                { topicName_ = name; }
    void SetTypeName(DDS::Elements::TypeName name)                  { typeName_ = name; }

    virtual Domain::DomainParticipantPtr        GetParticipant()    const { return participant_.lock(); }
    virtual const DDS::Elements::TopicName&     GetTopicName()      const { return topicName_; }
    virtual const DDS::Elements::TypeName&      GetTypeName()       const { return typeName_; }

protected:
    Domain::DomainParticipantWeakPtr    participant_;
    DDS::Elements::TopicName            topicName_;
    DDS::Elements::TypeName             typeName_;
};

}}

#endif
