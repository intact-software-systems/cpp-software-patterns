#ifndef DDS_Topic_TopicQoS_h_IsIncluded
#define DDS_Topic_TopicQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS
{

/**
 * @brief The TopicQoS class is the qos policies associated to DDS Topics
 */
class DLL_STATE TopicQoS
{
public:
    TopicQoS() {}

    TopicQoS(const DDS::Policy::TopicData&            topic_data,
             const DDS::Policy::Durability&           durability,

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                 const DDS::Policy::DurabilityService&    durability_service,
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

             const DDS::Policy::Deadline&             deadline,
             const DDS::Policy::LatencyBudget&        budget,
             const DDS::Policy::Liveliness&           liveliness,
             const DDS::Policy::Reliability&          reliability,
             const DDS::Policy::DestinationOrder&     order,
             const DDS::Policy::History&              history,
             const DDS::Policy::ResourceLimits&       resources,
             const DDS::Policy::TransportPriority&    priority,
             const DDS::Policy::Lifespan&             lifespan,
             const DDS::Policy::Ownership&            ownership);

    ~TopicQoS();

public:
    void policy(const DDS::Policy::TopicData&          topic_data);
    void policy(const DDS::Policy::Durability&         durability);

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    void policy(const DDS::Policy::DurabilityService&  durability_service);
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

    void policy(const DDS::Policy::Deadline&           deadline);
    void policy(const DDS::Policy::LatencyBudget&      budget);
    void policy(const DDS::Policy::Liveliness&         liveliness);
    void policy(const DDS::Policy::Reliability&        reliability);
    void policy(const DDS::Policy::DestinationOrder&   order);
    void policy(const DDS::Policy::History&            history);
    void policy(const DDS::Policy::ResourceLimits&     resources);
    void policy(const DDS::Policy::TransportPriority&  priority);
    void policy(const DDS::Policy::Lifespan&           lifespan);
    void policy(const DDS::Policy::Ownership&          ownership);

public:
    template <typename POLICY>
    const POLICY& policy() const;

    template <typename POLICY>
    POLICY& policy();


public:
    /**
     * @brief Default
     * @return
     */
    static TopicQoS Default()
    {
        return TopicQoS();
    }

private:
    DDS::Policy::TopicData             topic_data_;
    DDS::Policy::Durability            durability_;

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    DDS::Policy::DurabilityService     durability_service_;
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

    DDS::Policy::Deadline              deadline_;
    DDS::Policy::LatencyBudget         budget_;
    DDS::Policy::Liveliness            liveliness_;
    DDS::Policy::Reliability           reliability_;
    DDS::Policy::DestinationOrder      order_;
    DDS::Policy::History               history_;
    DDS::Policy::ResourceLimits        resources_;
    DDS::Policy::TransportPriority     priority_;
    DDS::Policy::Lifespan              lifespan_;
    DDS::Policy::Ownership             ownership_;
};

}

#endif
