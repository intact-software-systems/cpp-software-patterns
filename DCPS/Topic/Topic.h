/*
 * Topic.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Topic_Topic_h_IsIncluded
#define DCPS_Topic_Topic_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/CommonDefines.h"
#include"DCPS/Domain/DomainFwd.h"
#include"DCPS/Topic/TopicDescription.h"
#include"DCPS/Topic/TopicListener.h"
#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Topic
{

/**
 * @brief
 *
 * A Topic is identified by its name, which must be unique in the whole Domain. In addition (by virtue of extending
 * TopicDescription) it fully specifies the type of the data that can be communicated when publishing or subscribing to the
 * Topic.
 *
 * Topic is the only TopicDescription that can be used for publications and therefore associated to a DataWriter.
 *
 * All operations except for the base-class operations set_qos, get_qos, set_listener, get_listener, enable, and
 * get_status_condition may return the value NOT_ENABLED.
 *
 * TODO: What is TypeSupport? RegisterType?
 *     Do I register Topic in a type "ServiceConnectionManager"?
 *
 * TODO: Check in constructor if the topicQos is vali
 */
template <typename DATA>
class Topic : public TopicDescription<DATA>, public DCPS::Infrastructure::Entity
{
public:
    /**
     * @description
     *
     * Topic empty constructor
     */
    Topic()
        : Topic::TopicDescription()
        , Infrastructure::Entity()
        , inconsistentTopicStatus_()
    {}

    /**
     * @description
     *
     * Topic full constructor
     */
    Topic(DDS::Elements::TopicName topicName,
          DDS::Elements::TypeName typeName,
          DDS::TopicQoS topicQos,
          typename TopicListener<DATA>::Ptr listener,
          Status::StatusMask mask,
          Domain::DomainParticipantPtr participant)
        : Topic::TopicDescription(participant, topicName, typeName)
        , Infrastructure::Entity(mask)
        , topicQos_(topicQos)
        , topicListener_(listener)
        , inconsistentTopicStatus_()
    { }

    /**
     * @description
     *
     * Topic empty destructor
     */
    virtual ~Topic()
    { }

    CLASS_TRAITS(Topic)

    /**
     * @brief SetQoS
     * @param topicQos
     * @return
     */
    ReturnCode::Type SetQoS(const DDS::TopicQoS &topicQos);

    /**
     * @brief GetQoS
     * @return
     */
    DDS::TopicQoS GetQoS() const;

    /**
     * @brief SetListener
     * @param listener
     * @param statusMask
     * @return
     */
    ReturnCode::Type SetListener(const typename TopicListener<DATA>::Ptr &listener, const Status::StatusMask &statusMask);

    /**
     * @brief GetListener
     * @return
     */
    typename TopicListener<DATA>::Ptr GetListener();

    /**
     * @brief GetInstanceHandle
     * @return
     */
    virtual InstanceHandle GetInstanceHandle() const;

    /**
     * @brief Enable
     * @return
     */
    virtual ReturnCode::Type Enable();

    /**
     * @brief This method allows the application to retrieve the INCONSISTENT_TOPIC status of the Topic.
     * Each DomainEntity has a set of relevant communication statuses. A change of status causes the corresponding Listener
     * to be invoked and can also be monitored by means of the associated StatusCondition.
     * The complete list of communication status, their values, and the DomainEntities they apply to is provided in
     * Section 7.1.4.1, “Communication Status,” on page 120.
     */
    Status::InconsistentTopicStatus GetInconsistentTopicStatus();

private:
    /**
     * @brief topicQos_
     */
    DDS::TopicQoS topicQos_;

    /**
     * @brief topicListener_
     */
    typename TopicListener<DATA>::WeakPtr topicListener_;

    /**
     * @brief inconsistentTopicStatus_ count of different topics with the same topic name but
     * different characteristics (typename or incompatible Qos).
     */
    Status::InconsistentTopicStatus inconsistentTopicStatus_;
};

/**
 * @brief Topic<DATA>::SetQoS
 *
 * Possible error codes returned in addition to the standard ones: INCONSISTENT_POLICY, IMMUTABLE_POLICY.
 *
 * TODO: Verify TopicQos is valid by adding an isValid() function for all QoS implementations
 *
 * @param topicQos
 * @return
 */
template <typename DATA>
ReturnCode::Type Topic<DATA>::SetQoS(const DDS::TopicQoS &topicQos)
{
    topicQos_ = topicQos;
    return ReturnCode::Ok;
}

template <typename DATA>
DDS::TopicQoS Topic<DATA>::GetQoS() const
{
    return topicQos_;
}

/**
 * @brief
 *
 * This operation installs a Listener on the Entity. The listener will only be invoked on the changes of communication status
 * indicated by the specified mask.
 *
 * It is permitted to use ‘nil’ as the value of the listener. The ‘nil’ listener behaves as a Listener whose operations perform
 * no action.
 *
 * Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will replace it
 * with the new one. Consequently if the value ‘nil’ is passed for the listener parameter to the set_listener operation, any
 * existing listener will be removed.
 */
template <typename DATA>
ReturnCode::Type Topic<DATA>::SetListener(const typename TopicListener<DATA>::Ptr &listener, const Status::StatusMask &statusMask)
{
    this->topicListener_ = listener;
    this->statusMask_ = statusMask;

    return ReturnCode::Ok;
}

/**
 * @brief Topic<DATA>::GetListener allows access to the existing Listener attached to the Entity.
 * @return
 */
template <typename DATA>
typename TopicListener<DATA>::Ptr Topic<DATA>::GetListener()
{
    return topicListener_.lock();
}

template <typename DATA>
InstanceHandle Topic<DATA>::GetInstanceHandle() const
{
    return InstanceHandle::NIL();
}

/**
 * @brief
 *
 * According spec:
 * - Calling enable on an already enabled Entity returns OK and has no effect.
 * - Calling enable on an Entity whose factory is not enabled will fail and
 * return PRECONDITION_NOT_MET.
 */
template <typename DATA>
ReturnCode::Type Topic<DATA>::Enable()
{
    return EnableParticipant(this->participant_.lock());
}

/**
 * @brief
 *
 * This method allows the application to retrieve the INCONSISTENT_TOPIC status of the Topic.
 * Each DomainEntity has a set of relevant communication statuses. A change of status causes the corresponding Listener
 * to be invoked and can also be monitored by means of the associated StatusCondition.
 * The complete list of communication status, their values, and the DomainEntities they apply to is provided in
 * Section 7.1.4.1, “Communication Status,” on page 120.
 */
template <typename DATA>
Status::InconsistentTopicStatus Topic<DATA>::GetInconsistentTopicStatus()
{
    // TODO: Anything to check before returning InconsistentTopicStatus?
    // TODO: Set statusReadFlag = true

    return inconsistentTopicStatus_;
}

}} // namespace DCPS::Topic

#endif // DCPS_Topic_Topic_h_IsIncluded
