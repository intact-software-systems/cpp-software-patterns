#ifndef DCPS_Domain_DomainParticipantConfig_h_IsIncluded
#define DCPS_Domain_DomainParticipantConfig_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Factory/DCPSConfig.h"

#include"DCPS/Infrastructure/IncludeLibs.h"
#include"DCPS/Pub/IncludeLibs.h"
#include"DCPS/Sub/IncludeLibs.h"

#include"DCPS/Topic/Topic.h"
#include"DCPS/Topic/TopicHolder.h"
#include"DCPS/Topic/ContentFilteredTopic.h"
#include"DCPS/Domain/DomainParticipantListener.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Domain
{

/**
 * @brief The DomainParticipantConfig class
 */
class DLL_STATE DomainParticipantConfig
{
public:
    /**
     * @brief DomainParticipantConfig
     */
    DomainParticipantConfig();

    /**
     * @brief DomainParticipantConfig
     * @param domainId
     * @param qos
     */
    DomainParticipantConfig(const DDS::Elements::DomainId &domainId,
                            const DDS::DomainParticipantQoS &qos);

    /**
     * @brief ~DomainParticipantConfig
     */
    virtual ~DomainParticipantConfig();

    CLASS_TRAITS(DomainParticipantConfig)

public:
    /**
     * @brief SetQoS
     *
     * TODO: Verify DomainParticipantQoS is valid by adding an isValid() function for all QoS implementations
     *
     * @param domainQos
     * @return
     */
    ReturnCode::Type SetQoS(const DDS::DomainParticipantQoS &domainQos);

    /**
     * @brief GetQoS
     * @return
     */
    DDS::DomainParticipantQoS GetQoS() const;

    /**
    * This operation retrieves the default value of the Publisher QoS, that
    * is, the QoS policies which will be used for newly created
    * {@link Publisher} entities in the case where the QoS policies are
    * defaulted in the {@link #createPublisher()} operation.
    *
    * The values retrieved will match the set of values specified on the
    * last successful call to {@link #setDefaultPublisherQos(PublisherQos)},
    * or else, if the call was never made, the default values identified by
    * the DDS specification.
    *
    * @see #setDefaultPublisherQos(PublisherQos)
    */
    DDS::PublisherQoS GetDefaultPublisherQos() const;

    /**
    * This operation sets a default value of the Publisher QoS policies,
    * which will be used for newly created {@link Publisher} entities in the
    * case where the QoS policies are defaulted in the
    * {@link #createPublisher()} operation.
    *
    * @throws InconsistentPolicyException if the resulting policies are
    * not self consistent; in that case, the operation will have no
    * effect.
    *
    * @see #getDefaultPublisherQos()
    */
    bool SetDefaultPublisherQos(const DDS::PublisherQoS &qos);


    /**
    * This operation retrieves the default value of the Subscriber QoS, that
    * is, the QoS policies which will be used for newly created
    * {@link Subscriber} entities in the case where the QoS policies are
    * defaulted in the {@link #createSubscriber()} operation.
    *
    * The values retrieved will match the set of values specified on the
    * last successful call to
    * {@link #setDefaultSubscriberQos(SubscriberQos)}, or else, if the call
    * was never made, the default values identified by the DDS
    * specification.
    *
    * @see #setDefaultSubscriberQos(SubscriberQos)
    */
    DDS::SubscriberQoS GetDefaultSubscriberQos() const;

    /**
    * This operation sets a default value of the Subscriber QoS policies
    * that will be used for newly created {@link Subscriber} entities in the
    * case where the QoS policies are defaulted in the
    * {@link #createSubscriber()} operation.
    *
    * @throws InconsistentPolicyException if the resulting policies are
    * not self consistent; in that case, the operation will have no
    * effect.
    *
    * @see #getDefaultSubscriberQos()
    */
    bool SetDefaultSubscriberQos(const DDS::SubscriberQoS &qos);


    /**
    * This operation retrieves the default value of the Topic QoS, that is,
    * the QoS policies which will be used for newly created {@link Topic}
    * entities in the case where the QoS policies are defaulted in the
    * {@link #createTopic(String, Class)} operation.
    *
    * The values retrieved will match the set of values specified on the
    * last successful call to {@link #setDefaultTopicQos(TopicQos)}, or
    * else, if the call was never made, the default values identified by the
    * DDS specification.
    *
    * @see #setDefaultTopicQos(TopicQos)
    */
    DDS::TopicQoS GetDefaultTopicQos() const;

    /**
    * This operation sets a default value of the Topic QoS policies, which
    * will be used for newly created {@link Topic} entities in the case
    * where the QoS policies are defaulted in the
    * {@link #createTopic(String, Class)} operation.
    *
    * @throws InconsistentPolicyException if the resulting policies are
    * not self consistent; in that case, the operation will have no
    * effect.
    *
    * @see #getDefaultTopicQos()
    */
    bool SetDefaultTopicQos(const DDS::TopicQoS &qos);

    /**
    * This operation retrieves the domain ID used to create the
    * DomainParticipant. The domain ID identifies the DDS domain to which
    * the DomainParticipant belongs. Each DDS domain represents a separate
    * data "communication plane" isolated from other domains.
    */
    DDS::Elements::DomainId GetDomainId() const;

private:
    DDS::Elements::DomainId 			domainId_;
    DDS::DomainParticipantQoS 			domainQos_;

private:
    DDS::SubscriberQoS                  defaultSubscriberQos_;
    DDS::PublisherQoS                   defaultPublisherQos_;
    DDS::TopicQoS                       defaultTopicQos_;

private:
//    DCPS::PublisherInit               publisherInit_;
//    DCPS::SubscriberInit              subscriberInit_;

    // TODO: Similar
    // ConfigValidator configValidator_;
};

}}

#endif
