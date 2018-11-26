/*
 * DomainParticipant.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Domain_DomainParticipant_h_IsIncluded
#define DCPS_Domain_DomainParticipant_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Infrastructure/IncludeLibs.h"
#include"DCPS/Pub/IncludeLibs.h"
#include"DCPS/Sub/IncludeLibs.h"

#include"DCPS/Topic/Topic.h"
#include"DCPS/Topic/TopicHolder.h"
#include"DCPS/Topic/ContentFilteredTopic.h"

#include"DCPS/Domain/DomainParticipantListener.h"
#include"DCPS/Domain/DomainParticipantConfig.h"
#include"DCPS/Domain/DomainParticipantState.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Domain
{

/**
 * @brief
 *
 * The DomainParticipant object plays several roles:
 * It acts as a container for all other Entity objects.
 * It acts as factory for the Publisher, Subscriber, Topic, and MultiTopic Entity objects.
 * It represents the participation of the application on a communication plane that isolates applications running on the
 *    same set of physical computers from each other. A domain establishes a virtual network linking all applications that
 *    share the same domainId and isolating them from applications running on different domains. In this way, several
 *    independent distributed applications can coexist in the same physical network without interfering, or even being aware
 *    of each other.
 * It provides administration services in the domain, offering operations that allow the application to ignore locally any
 *    information about a given participant (ignore_participant), publication (ignore_publication), subscription
 *    (ignore_subscription), or topic (ignore_topic).
 */
class DLL_STATE DomainParticipant
        : public Infrastructure::DomainEntity
        , public ENABLE_SHARED_FROM_THIS(DomainParticipant)
        , public Templates::ContextObject<DomainParticipantConfig, DomainParticipantState, Templates::NullCriticalState>
{
public:
    DomainParticipant(
            const DDS::Elements::DomainId &domainId,
            const DDS::DomainParticipantQoS &qos,
            const DomainParticipantListener::Ptr &listener,
            const Status::StatusMask &mask);

    virtual ~DomainParticipant();

    CLASS_TRAITS(DomainParticipant)

    /**
     * @brief GetPtr
     * @return
     */
    DomainParticipant::Ptr GetPtr()
    {
        return shared_from_this();
    }

public:
    /**
     * @brief This operation creates a Publisher with default QoS policies and
     * no PublisherListener.
     *
     * The created Publisher belongs to the DomainParticipant that is its
     * factory.
     *
     * @see #createPublisher(PublisherQos, PublisherListener, Collection)
     */
    Publication::Publisher::Ptr CreatePublisher();

    /**
     * @brief This operation creates a Publisher.
     *
     * The created Publisher belongs to the DomainParticipant that is its
     * factory.
     *
     * @param qos The desired QoS policies. If the specified QoS
     * policies are not consistent, the operation will
     * fail and no Publisher will be created.
     * @param listener The listener to be attached.
     * @param statuses Of which status changes the listener should be notified. A null collection signifies all status changes.
     * @return
     */
    Publication::Publisher::Ptr CreatePublisher(const DDS::PublisherQoS &qos,
                                                const Publication::PublisherListener::Ptr &listener,
                                                const Status::StatusMask &statuses);

private:
    /**
     * @brief createPublisher
     * @param qos
     * @param listener
     * @param statuses
     * @return
     */
    Publication::Publisher::Ptr createPublisher(
            const DDS::PublisherQoS &qos,
            const Publication::PublisherListener::Ptr &listener,
            const Status::StatusMask &statuses);

public:
    /**
    * @brief This operation creates a Subscriber with default QoS policies and
    * no SubscriberListener.
    *
    * The created Subscriber belongs to the DomainParticipant that is its
    * factory.
    *
    * @see #createSubscriber(SubscriberQos, SubscriberListener, Collection)
    */
    Subscription::Subscriber::Ptr CreateSubscriber();

    /**
    * @brief This operation creates a Subscriber.
    *
    * The created Subscriber belongs to the DomainParticipant that is its
    * factory.
    *
    * @param qos The desired QoS policies. If the specified QoS
    * policies are not consistent, the operation will
    * fail and no Subscriber will be created.
    * @param listener The listener to be attached.
    * @param statuses Of which status changes the listener should be
    * notified. A null collection signifies all status
    * changes.
    *
    * @see #createSubscriber()
    */
    Subscription::Subscriber::Ptr CreateSubscriber(
            const DDS::SubscriberQoS &qos,
            const Subscription::SubscriberListener::Ptr &listener,
            const Status::StatusMask &statuses);

    /**
    * @brief This operation allows access to the built-in Subscriber. Each
    * DomainParticipant contains several built-in {@link Topic} objects as
    * well as corresponding {@link DataReader} objects to access them. All
    * these DataReader objects belong to a single built-in Subscriber.
    *
    * The built-in Topics are used to communicate information about other
    * DomainParticipant, Topic, {@link DataReader}, and {@link DataWriter}
    * objects.
    */
    Subscription::Subscriber::Ptr GetBuiltinSubscriber();

private:
    /**
     * @brief CreatePrivate implementation of the create subscriber.
     *
     * @param qos
     * @param listener
     * @param statuses
     * @return
     */
    Subscription::Subscriber::Ptr createSubscriber(
            const DDS::SubscriberQoS &qos,
            const Subscription::SubscriberListener::Ptr &listener,
            const Status::StatusMask &statuses);

public:
    /**
    * @brief This operation creates a Topic with default QoS policies and no
    * TopicListener.
    *
    * The created Topic belongs to the DomainParticipant that is its
    * factory.
    *
    * @param topicName The name of the new Topic.
    * @param type The type of all samples to be published and
    * subscribed over the new Topic. The Service will
    * attempt to locate an appropriate
    * {@link TypeSupport} instance based on this type.
    */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr CreateTopic(DDS::Elements::TopicName topicName,
                                                 DDS::Elements::TypeName type)
    {
        return createTopic<DATA>(
                    topicName,
                    type,
                    this->config().GetDefaultTopicQos(),
                    typename Topic::TopicListener<DATA>::Ptr(),
                    Status::StatusMask::All());
    }

    /**
     * @brief Create a parameterized Topic.
     */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr CreateTopic(DDS::Elements::TopicName topicName,
                                                 DDS::Elements::TypeName type,
                                                 const DDS::TopicQoS &qos)
    {
        return createTopic<DATA>(
                    topicName,
                    type,
                    qos,
                    typename Topic::TopicListener<DATA>::Ptr(),
                    Status::StatusMask::All());
    }


    /**
     * @brief Create a parameterized Topic.
     */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr CreateTopic(const DDS::TopicQoS &qos)
    {
        return createTopic<DATA>(
                    DDS::Elements::TopicName(Utility::GetTypeName<DATA>()),
                    DDS::Elements::TypeName(Utility::GetTypeName<DATA>()),
                    qos,
                    typename Topic::TopicListener<DATA>::Ptr(),
                    Status::StatusMask::All());
    }

    /**
    * @brief This operation creates a Topic with the desired QoS policies and
    * attaches to it the specified TopicListener.
    *
    * The created Topic belongs to the DomainParticipant that is its
    * factory.
    *
    * @param topicName The name of the new Topic.
    * @param type The type of all samples to be published and subscribed over the new Topic. The Service will
    * attempt to locate an appropriate {@link TypeSupport} instance based on this type.
    * @param qos The desired QoS policies. If the specified QoS policies are not consistent, the operation will
    * fail and no Publisher will be created.
    * @param listener The listener to be attached.
    * @param statuses Of which status changes the listener should be
    * notified. A null collection signifies all status changes.
    */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr CreateTopic(
            const DDS::Elements::TopicName &topicName,
            const DDS::Elements::TypeName &type,
            const DDS::TopicQoS &qos,
            const typename Topic::TopicListener<DATA>::Ptr &listener,
            const Status::StatusMask &statuses)
    {
        return createTopic<DATA>(
                    topicName,
                    type,
                    qos,
                    listener,
                    statuses);
    }

private:

    /**
     * @brief Private implementation of create topic that stores the created topic in the DomainParticipant.
     */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr createTopic(
            DDS::Elements::TopicName topicName,
            DDS::Elements::TypeName type,
            const DDS::TopicQoS &qos,
            typename Topic::TopicListener<DATA>::Ptr listener,
            const Status::StatusMask &statuses)
    {
        typename Topic::Topic<DATA>::Ptr topic(
                    new Topic::Topic<DATA>(
                        topicName,
                        type,
                        qos,
                        listener,
                        statuses,
                        this->GetPtr())
                    );

        this->state().topics().Write(topic, topic->GetInstanceHandle());

        return topic;
    }


public:

    // --- Other operations: -------------------------------------------------

    /**
    * @brief This operation gives access to an existing (or ready to exist) enabled
    * Topic, based on its name. The operation takes as arguments the name of
    * the Topic and a timeout.
    *
    * If a Topic of the same name already exists, it gives access to it,
    * otherwise it waits (blocks the caller) until another mechanism creates
    * it (or the specified timeout occurs). This other mechanism can be
    * another thread, a configuration tool, or some other middleware
    * service. Note that the Topic is a local object that acts as a 'proxy'
    * to designate the global concept of topic. Middleware implementations
    * could choose to propagate topics and make remotely created topics
    * locally available.
    *
    * A Topic obtained by means of findTopic must also be closed by means of
    * {@link Topic#close()} so that the local resources can be released. If
    * a Topic is obtained multiple times by means of findTopic or
    * {@link #createTopic(String, Class)}, it must also be closed that same
    * number of times.
    *
    * Regardless of whether the middleware chooses to propagate topics, the
    * {@link Topic#close()} operation disposes of only the local proxy.
    *
    * @return a non-null Topic.
    *
    * @throws TimeoutException if the specified timeout elapses and no
    * suitable Topic could be found.
    *
    * @see #findTopic(String, long, TimeUnit)
    */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr FindTopic(const DDS::Elements::TopicName &topicName, const Duration &timeout)
    {
        return Topic::Topic<DATA>::Ptr();
    }


    /**
    * @brief This operation gives access to an existing (or ready to exist) enabled
    * Topic, based on its name. The operation takes as arguments the name of
    * the Topic and a timeout.
    *
    * If a Topic of the same name already exists, it gives access to it,
    * otherwise it waits (blocks the caller) until another mechanism creates
    * it (or the specified timeout occurs). This other mechanism can be
    * another thread, a configuration tool, or some other middleware
    * service. Note that the Topic is a local object that acts as a 'proxy'
    * to designate the global concept of topic. Middleware implementations
    * could choose to propagate topics and make remotely created topics
    * locally available.
    *
    * A Topic obtained by means of findTopic must also be closed by means of
    * {@link Topic#close()} so that the local resources can be released. If
    * a Topic is obtained multiple times by means of findTopic or
    * {@link #createTopic(String, Class)}, it must also be closed that same
    * number of times.
    *
    * Regardless of whether the middleware chooses to propagate topics, the
    * {@link Topic#close()} operation disposes of only the local proxy.
    *
    * @return a non-null Topic.
    *
    * @throws TimeoutException if the specified timeout elapses and no
    * suitable Topic could be found.
    *
    * @see #findTopic(String, Duration)
    */
    template <typename DATA>
    typename Topic::Topic<DATA>::Ptr FindTopic(const DDS::Elements::TopicName &topicName, int64 timeoutMs = 1000)
    {
        return Topic::Topic<DATA>::Ptr();
    }

    /**
    * @brief This operation gives access to an existing locally-created
    * TopicDescription based on its name. The operation takes as argument
    * the name of the TopicDescription.
    *
    * If a TopicDescription of the same name already exists, it gives access
    * to it, otherwise it returns null. The operation never blocks.
    *
    * The operation may be used to locate any locally-created {@link Topic},
    * {@link ContentFilteredTopic}, or {@link MultiTopic} object.
    *
    * Unlike {@link #findTopic(String, Duration)}, the operation searches
    * only among the locally created topics. Therefore, it should never
    * create a new TopicDescription. The TopicDescription returned does not
    * require an extra {@link TopicDescription#close()}. It is still
    * possible to close the TopicDescription returned by this method,
    * provided it has no readers or writers, but then it is really closed
    * and subsequent lookups will fail.
    *
    * If the operation fails to locate a TopicDescription, it returns null.
    *
    * @param <TYPE> The type of all samples subscribed to with the
    * TopicDescription.
    * @param name The name of the TopicDescription to look up.
    */
    template <typename DATA>
    Topic::TopicDescription<DATA> LookupTopicDescription(const DDS::Elements::TopicName &topicName)
    {
        //if(this->GetTransientState().topics().containsKey(topicName.GetTopicName()))
        //{
        //  return this->GetTransientState().topics().read(topicName.GetTopicName());
        //}

        return Topic::TopicDescription<DATA>();
    }

    /**
    * @brief This operation creates a ContentFilteredTopic. A ContentFilteredTopic
    * can be used to do content-based subscriptions.
    *
    * @param <TYPE> The type of all samples subscribed to
    * with the new ContentFilteredTopic. It may
    * be the same as the type of the
    * relatedTopic or any supertype of that
    * type.
    * @param name The name of the new ContentFilteredTopic.
    * @param relatedTopic The related Topic being subscribed to.
    * The ContentFilteredTopic only relates to
    * samples published under this Topic,
    * filtered according to their content.
    * @param filterExpression A logical expression that involves the
    * values of some of the data fields in the
    * sample.
    * @param expressionParameters Parameters to the filterExpression.
    */
    template <typename DATA>
    Topic::ContentFilteredTopic<DATA>  CreateContentFilteredTopic(
            const DDS::Elements::TopicName &topicName,
            const typename Topic::Topic<DATA>::Ptr &relatedTopic,
            const Core::Query &query)
    {
        return Topic::ContentFilteredTopic<DATA>::Ptr();
    }

    /**
    * This operation creates a MultiTopic. A MultiTopic can be used to
    * subscribe to multiple topics and combine/filter the received data into
    * a resulting type. In particular, MultiTopic provides a content-based
    * subscription mechanism.
    *
    * @param <TYPE> The type of all samples subscribed to with the new
    * MultiTopic.
    * @param name The name of the new MultiTopic.
    * @param typeName The resulting type. The Service will attempt to
    * locate an appropriate {@link TypeSupport} instance
    * based on this type.
    * @param subscriptionExpression The list of topics and the logic used
    * to combine filter and re-arrange the information from each
    * Topic.
    * @param expressionParameters Parameters to the filterExpression.
    */
    //MultiTopic<TYPE> createMultiTopic(String name,String typeName,String subscriptionExpression,List<String> expressionParameters);

    /**
    * This operation deletes all the entities that were created by means of
    * the "create" operations on the DomainParticipant. That is, it deletes
    * all contained {@link Publisher}, {@link Subscriber}, {@link Topic},
    * {@link ContentFilteredTopic}, and {@link MultiTopic} objects.
    *
    * Prior to deleting each contained entity, this operation will
    * recursively call the corresponding closeContainedEntities operation on
    * each contained entity (if applicable). This pattern is applied
    * recursively. In this manner the operation closeContainedEntities on
    * the DomainParticipant will end up deleting all the entities
    * recursively contained in the DomainParticipant, that is also the
    * {@link DataWriter}, {@link DataReader}, as well as the
    * {@link QueryCondition} and {@link ReadCondition} objects belonging to
    * the contained DataReaders.
    *
    * Once closeContainedEntities returns successfully, the application may
    * delete the DomainParticipant knowing that it has no contained
    * entities.
    *
    * @throws PreconditionNotMetException if any of the contained
    * entities is in a state where it cannot be closed.
    */
    void CloseContainedEntities();

    /**
    * This operation allows an application to instruct the Service to
    * locally ignore a remote domain participant. From that point onwards,
    * the Service will locally behave as if the remote participant did not
    * exist. This means it will ignore any {@link Topic}, publication, or
    * subscription that originates on that domain participant.
    *
    * This operation can be used, in conjunction with the discovery of
    * remote participants offered by means of the "DCPSParticipant" built-in
    * Topic, to provide, for example, access control. Application data can
    * be associated with a DomainParticipant by means of the
    * {@link org.omg.dds.core.policy.UserData}. This application data is propagated as a
    * field in the built-in topic and can be used by an application to
    * implement its own access control policy.
    *
    * The domain participant to ignore is identified by the handle argument.
    * This handle is the one that appears in the {@link Sample} retrieved
    * when reading the data samples available for the built-in DataReader to
    * the "DCPSParticipant" topic. The built-in {@link DataReader} is read
    * with the same read/take operations used for any DataReader.
    *
    * This operation is not required to be reversible. The Service offers no
    * means to reverse it.
    *
    * @throws OutOfResourcesException if the Service is unable to ignore
    * the indicated participant because an internal resource has
    * been exhausted.
    */
    void IgnoreParticipant(const InstanceHandle &handle);

    /**
    * This operation allows an application to instruct the Service to
    * locally ignore a {@link Topic}. This means it will locally ignore any
    * publication or subscription to the Topic.
    *
    * This operation can be used to save local resources when the
    * application knows that it will never publish or subscribe to data
    * under certain topics.
    *
    * The Topic to ignore is identified by the handle argument. This handle
    * is the one that appears in the {@link Sample} retrieved when reading
    * the data samples from the built-in {@link DataReader} to the
    * "TopicBuiltinTopicData" topic.
    *
    * This operation is not required to be reversible. The Service offers no
    * means to reverse it.
    *
    * @throws OutOfResourcesException if the Service is unable to ignore
    * the indicated topic because an internal resource has been
    * exhausted.
    */
    void IgnoreTopic(const InstanceHandle &handle);

    /**
    * This operation allows an application to instruct the Service to
    * locally ignore a remote publication; a publication is defined by the
    * association of a topic name, and user data and partition set on the
    * {@link Publisher}. After this call, any data written related to that
    * publication will be ignored.
    *
    * The {@link DataWriter} to ignore is identified by the handle argument.
    * This handle is the one that appears in the {@link Sample} retrieved
    * when reading the data samples from the built-in {@link DataReader} to
    * the "PublicationBuiltinTopicData" topic.
    *
    * This operation is not required to be reversible. The Service offers no
    * means to reverse it.
    *
    * @throws OutOfResourcesException if the Service is unable to ignore
    * the indicated publication because an internal resource has
    * been exhausted.
    */
    void IgnorePublication(const InstanceHandle &handle);

    /**
    * This operation allows an application to instruct the Service to
    * locally ignore a remote subscription; a subscription is defined by the
    * association of a topic name, and user data and partition set on the
    * {@link Subscriber}. After this call, any data received related to that
    * subscription will be ignored.
    *
    * The {@link DataReader} to ignore is identified by the handle argument.
    * This handle is the one that appears in the {@link Sample} retrieved
    * when reading the data samples from the built-in DataReader to the
    * "DCPSSubscription" topic.
    *
    * This operation is not required to be reversible. The Service offers no
    * means to reverse it.
    *
    * @throws OutOfResourcesException if the Service is unable to ignore
    * the indicated subscription because an internal resource has
    * been exhausted.
    */
    void IgnoreSubscription(const InstanceHandle &handlee);

    /**
    * This operation retrieves the domain ID used to create the
    * DomainParticipant. The domain ID identifies the DDS domain to which
    * the DomainParticipant belongs. Each DDS domain represents a separate
    * data "communication plane" isolated from other domains.
    */
    DDS::Elements::DomainId GetDomainId() const
    {
        return this->config().GetDomainId();
    }

    /**
    * This operation manually asserts the liveliness of the
    * DomainParticipant. This is used in combination with the
    * {@link org.omg.dds.core.policy.Liveliness} to indicate to the Service that the entity
    * remains active.
    *
    * This operation needs to only be used if the DomainParticipant contains
    * {@link DataWriter} entities with the
    * {@link org.omg.dds.core.policy.Liveliness#getKind()} set to
    * {@link org.omg.dds.core.policy.Liveliness.Kind#MANUAL_BY_PARTICIPANT} and it only
    * affects the liveliness of those DataWriter entities. Otherwise, it has
    * no effect.
    *
    * <b>Note</b> - Writing data via the {@link DataWriter#write(Object)}
    * operation on a DataWriter asserts liveliness on the DataWriter itself
    * and its DomainParticipant. Consequently the use of assertLiveliness is
    * only needed if the application is not writing data regularly.
    */
    void AssertLiveliness();

    /**
    * This operation retrieves the list of DomainParticipants that have been
    * discovered in the domain and that the application has not indicated
    * should be "ignored" by means of the
    * {@link #ignoreParticipant(InstanceHandle)} operation.
    *
    * @param participantHandles a container, into which this method
    * will place handles to the discovered participants.
    *
    * @return participantHandles, as a convenience to facilitate chaining.
    *
    * @throws UnsupportedOperationException If the infrastructure does
    * not locally maintain the connectivity information.
    */
    InstanceHandleSeq GetDiscoveredParticipants(const InstanceHandleSeq &participantHandles);

    /**
    * This operation retrieves information on a DomainParticipant that has
    * been discovered on the network. The participant must be in the same
    * domain as the participant on which this operation is invoked and must
    * not have been "ignored" by means of the
    * {@link #ignoreParticipant(InstanceHandle)} operation.
    *
    * Use the operation {@link #getDiscoveredParticipants(Collection)} to
    * find the DomainParticipants that are currently discovered.
    *
    * @param participantData a container, into which this method
    * will store the participant data.
    * @param participantHandle a handle to the participant, the data
    * of which is to be retrieved.
    *
    * @return participantData, as a convenience to facilitate chaining.
    *
    * @throws PreconditionNotMetException if the participantHandle
    * does not correspond to a DomainParticipant such as is
    * described above.
    * @throws UnsupportedOperationException If the infrastructure does
    * not locally maintain the connectivity information.
    */
    //ParticipantBuiltinTopicData GetDiscoveredParticipantData(ParticipantBuiltinTopicData participantData, const InstanceHandle &participantHandle);

    /**
    * This operation retrieves the list of {@link Topic}s that have been
    * discovered in the domain and that the application has not indicated
    * should be "ignored" by means of the
    * {@link #ignoreTopic(InstanceHandle)} operation.
    *
    * @param topicHandles a container, into which this method
    * will place handles to the discovered topics.
    *
    * @return topicHandles, as a convenience to facilitate chaining.
    *
    * @throws UnsupportedOperationException If the infrastructure does
    * not locally maintain the connectivity information.
    */
    InstanceHandleSeq GetDiscoveredTopics(const InstanceHandleSeq &topicHandles);

    /**
    * This operation retrieves information on a {@link Topic} that has
    * been discovered on the network. The topic must be in the same
    * domain as the participant on which this operation is invoked and must
    * not have been "ignored" by means of the
    * {@link #ignoreTopic(InstanceHandle)} operation.
    *
    * Use the operation {@link #getDiscoveredTopics(Collection)} to
    * find the Topics that are currently discovered.
    *
    * @param topicData a container, into which this method
    * will store the participant data.
    * @param topicHandle a handle to the topic, the data
    * of which is to be retrieved.
    *
    * @return topicData, as a convenience to facilitate chaining.
    *
    * @throws PreconditionNotMetException if the topicHandle
    * does not correspond to a Topic such as is described above.
    * @throws UnsupportedOperationException If the infrastructure does
    * not locally maintain the connectivity information or if the
    * infrastructure does not hold the information necessary to
    * fill in the topicData.
    */
    //TopicBuiltinTopicData GetDiscoveredTopicData(TopicBuiltinTopicData topicData, const InstanceHandle &topicHandle);

    /**
    * This operation checks whether or not the given handle represents an
    * {@link Entity} that was created from the DomainParticipant. The
    * containment applies recursively. That is, it applies both to entities
    * ({@link TopicDescription}, {@link Publisher}, or {@link Subscriber})
    * created directly using the DomainParticipant as well as entities
    * created using a contained Publisher or Subscriber as the factory, and
    * so forth.
    *
    * The instance handle for an Entity may be obtained from built-in topic
    * data, from various statuses, or from the Entity operation
    * {@link Entity#getInstanceHandle()}.
    */
    bool ContainsEntity(const InstanceHandle &handle);

    /**
    * This operation returns the current value of the time that the service
    * uses to time stamp data writes and to set the reception time stamp for
    * the data updates it receives.
    *
    * @param currentTime a container for the current time, which the
    * Service will overwrite with the result of this operation.
    * @return currentTime, as a convenience to facilitate chaining.
    */
    //Time GetCurrentTime(Time currentTime);

    /**
     * @brief SetListener
     * @param listener
     * @param statusMask
     * @return
     */
    ReturnCode::Type SetListener(const DomainParticipantListener::Ptr &listener, const Status::StatusMask &statusMask);

    /**
     * @brief GetListener
     * @return
     */
    DomainParticipantListener::Ptr GetListener();

    /**
     * @brief GetInstanceHandle Overridden from from Entity
     * @return
     */
    virtual InstanceHandle      GetInstanceHandle() const;

    /**
     * @brief Enable Overridden from from Entity
     * @return
     */
    virtual ReturnCode::Type 	Enable();

private:
    /**
     * @brief One listener possible.
     *
     * TODO: refactor to context object?
     */
    DomainParticipantListener::WeakPtr	domainListener_;
};

}}

#endif

