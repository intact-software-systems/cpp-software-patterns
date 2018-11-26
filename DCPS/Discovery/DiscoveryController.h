#ifndef DCPS_Discovery_DiscoveryController_h_IsIncluded
#define DCPS_Discovery_DiscoveryController_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Discovery/EndpointDiscoveryController.h"
#include"DCPS/Discovery/ParticipantDiscoveryController.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Discovery
{

/**
 * @brief The DiscoveryControllerState class
 */
class DLL_STATE DiscoveryControllerState
{
public:
    DiscoveryControllerState();
    virtual ~DiscoveryControllerState();

    ParticipantDiscoveryController *ParticipantDiscovery() const;
    EndpointDiscoveryController *EndpointDiscovery() const;

private:
    ParticipantDiscoveryController  *participantDiscovery_;
    EndpointDiscoveryController     *endpointDiscovery_;
};

/**
 * @brief The DiscoveryController class
 *
 * TODO:
 * - Define topics:
 *  - TopicBuiltinTopicData with data needed to establish pub/sub, see RTPS specification
 *      - TODO: Rename, already a class Policy::TopicBuiltinTopicData
 *  - DCPSParticipant,
 *  - PublicationBuiltinTopicData,
 *  - DCPSSubscription,
 *
 * TODO:
 * - DiscoveryController is an observer to the data-reader, data-writer, topic and participant caches/objectmanagers.
 * - For every creation add a dds topic data instance to discovery cache writer.
 *
 * - DiscoveryController for all domains
 *      - Keep a map of domainId and Pair(ParticipantDiscovery, EndpointDiscovery)
 *
 * - DiscoveryController for each domain, in which case I need a DiscoveryFactory
 *      - Keep one ParticipantDiscovery and one EndpointDiscovery
 *
 * TODO Refactor:
 * - Move the discovery topics to a different module DCPSShared
 *
 * - DomainParticipantFactory writes to cache DomainParticipantAccess containing all domains and participants
 *
 * - Two approaches to link to RTPS:
 *      - 1. Create a separate module DCPSShared and move topics and QoS to it, let DCPS and RTPS link it
 *      - 2. Create an "interceptor" that serializes the topics before adding it to a new cache, which RTPS HistoryCache listens to.
 *
 * - 2 is the preferred choice for now. This makes it identical to all other topic handling (from DCPS point of view). Use a cache-writer approach.
 */
class DLL_STATE DiscoveryController
        : public Templates::ContextData<DiscoveryControllerState>
        , public RxData::ObjectObserver<Domain::DomainParticipantPtr>
        , public Runnable
        , public Templates::Lockable<Mutex>
{
private:
    DiscoveryController();
    virtual ~DiscoveryController();

    friend class Singleton<DiscoveryController>;

public:
    static DiscoveryController& Instance();

    void ObserveDomainCaches(DDS::Elements::DomainId domainId);

private:
    /**
     * @brief run
     */
    virtual void run();

public:
    virtual bool OnObjectCreated(Domain::DomainParticipantPtr data);
    virtual bool OnObjectDeleted(Domain::DomainParticipantPtr data);
    virtual bool OnObjectModified(Domain::DomainParticipantPtr data);

private:
    void addDomainCacheObservers(const DDS::Elements::DomainId &domainId);
    void removeDomainCacheObservers(const DDS::Elements::DomainId &domainId);

private:
    static Singleton<DiscoveryController> discoveryController_;

//private:
//    BaseLib::Concurrent::ObserverConnector<RxData::CacheObserver, Cache> cacheObserverConnector_;
};


//template <typename T>
//class DCPSObjectManager : public RxData::ObjectObserver<T>
//{
//public:
//    virtual bool OnObjectCreated(T data);
//    virtual bool OnObjectDeleted(T data);
//    virtual bool OnObjectModified(T data);
//};

}}

#endif
