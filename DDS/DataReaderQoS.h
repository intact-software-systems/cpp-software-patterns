#ifndef DDS_Subscription_DataReaderQoS_h_IsIncluded
#define DDS_Subscription_DataReaderQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/TopicQoS.h"
#include"DDS/Export.h"

namespace DDS
{

/**
 * @brief The DataReaderQoS class
 */
class DLL_STATE DataReaderQoS
{
public:
    DataReaderQoS();

    DataReaderQoS(const DDS::TopicQoS &qos,
                  DDS::Policy::UserData userData,
                  DDS::Policy::TimeBasedFilter tfilter,
                  DDS::Policy::ReaderDataLifecycle lifecycle);

    DataReaderQoS(  DDS::Policy::UserData             user_data,
                    DDS::Policy::Durability           durability,

                #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                    DDS::Policy::DurabilityService    durability_service,
                #endif  // OMG_DDS_PERSISTENCE_SUPPORT

                    DDS::Policy::Deadline             deadline,
                    DDS::Policy::LatencyBudget        budget,
                    DDS::Policy::Liveliness           liveliness,
                    DDS::Policy::Reliability          reliability,
                    DDS::Policy::DestinationOrder     order,
                    DDS::Policy::History              history,
                    DDS::Policy::ResourceLimits       resources,
                    DDS::Policy::TransportPriority    priority,
                    DDS::Policy::Lifespan             lifespan,
                    DDS::Policy::Ownership            ownership,
                    DDS::Policy::TimeBasedFilter      tfilter,
                    DDS::Policy::ReaderDataLifecycle  lifecycle);

    ~DataReaderQoS();

public:
    void policy(const DDS::Policy::UserData&            user_data);
    void policy(const DDS::Policy::Durability&          durability);

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    void policy(const DDS::Policy::DurabilityService&   durability_service);
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

    void policy(const DDS::Policy::Deadline&            deadline);
    void policy(const DDS::Policy::LatencyBudget&       budget);
    void policy(const DDS::Policy::Liveliness&          liveliness);
    void policy(const DDS::Policy::Reliability&         reliability);
    void policy(const DDS::Policy::DestinationOrder&    order);
    void policy(const DDS::Policy::History&             history);
    void policy(const DDS::Policy::ResourceLimits&      resources);
    void policy(const DDS::Policy::TransportPriority&   priority);
    void policy(const DDS::Policy::Lifespan&            lifespan);
    void policy(const DDS::Policy::Ownership&           ownership);
    void policy(const DDS::Policy::TimeBasedFilter&     tfilter);
    void policy(const DDS::Policy::ReaderDataLifecycle& lifecycle);

public:
    template <typename POLICY>
    const POLICY& policy() const;

    template <typename POLICY>
    POLICY& policy();

    /**
     * @brief Default
     * @return
     */
    static DataReaderQoS Default()
    {
        return DataReaderQoS();
    }

private:
    DDS::Policy::UserData                user_data_;
    DDS::Policy::Durability              durability_;

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    DDS::Policy::DurabilityService       durability_service_;
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

    DDS::Policy::Deadline                deadline_;
    DDS::Policy::LatencyBudget           budget_;
    DDS::Policy::Liveliness              liveliness_;
    DDS::Policy::Reliability             reliability_;
    DDS::Policy::DestinationOrder        order_;
    DDS::Policy::History                 history_;
    DDS::Policy::ResourceLimits          resources_;
    DDS::Policy::TransportPriority       priority_;
    DDS::Policy::Lifespan                lifespan_;
    DDS::Policy::Ownership               ownership_;
    DDS::Policy::TimeBasedFilter         tfilter_;
    DDS::Policy::ReaderDataLifecycle     lifecycle_;
};

}

#endif
