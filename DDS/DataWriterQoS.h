#ifndef DDS_Publication_DataWriterQoS_h_IsIncluded
#define DDS_Publication_DataWriterQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/TopicQoS.h"
#include"DDS/Export.h"

namespace DDS
{

/**
 * @brief The DataWriterQoS class
 */
class DLL_STATE DataWriterQoS
{
public:
    DataWriterQoS();

    DataWriterQoS(const TopicQoS& tqos);

    DataWriterQoS(	DDS::Policy::UserData                user_data,
                    DDS::Policy::Durability              durability,

                #ifdef  OMG_DDS_PERSISTENCE_SUPPORT
                    DDS::Policy::DurabilityService       durability_service,
                #endif  // OMG_DDS_PERSISTENCE_SUPPORT

                    DDS::Policy::Deadline                deadline,
                    DDS::Policy::LatencyBudget           budget,
                    DDS::Policy::Liveliness              liveliness,
                    DDS::Policy::Reliability             reliability,
                    DDS::Policy::DestinationOrder        order,
                    DDS::Policy::History                 history,
                    DDS::Policy::ResourceLimits          resources,
                    DDS::Policy::TransportPriority       priority,
                    DDS::Policy::Lifespan                lifespan,
                    DDS::Policy::Ownership               ownership,

                #ifdef  OMG_DDS_OWNERSHIP_SUPPORT
                    DDS::Policy::OwnershipStrength       strength,
                #endif  // OMG_DDS_OWNERSHIP_SUPPORT

                    DDS::Policy::WriterDataLifecycle     lifecycle);

    ~DataWriterQoS();

public:
    void policy(const DDS::Policy::UserData&          user_data);
    void policy(const DDS::Policy::Durability&        durability);

#ifdef  OMG_DDS_PERSISTENCE_SUPPORT
    void policy(const DDS::Policy::DurabilityService& durability_service);
#endif  // OMG_DDS_PERSISTENCE_SUPPORT

    void policy(const DDS::Policy::Deadline&          deadline);
    void policy(const DDS::Policy::LatencyBudget&     budget);
    void policy(const DDS::Policy::Liveliness&        liveliness);
    void policy(const DDS::Policy::Reliability&       reliability);
    void policy(const DDS::Policy::DestinationOrder&  order);
    void policy(const DDS::Policy::History&           history);
    void policy(const DDS::Policy::ResourceLimits&    resources);
    void policy(const DDS::Policy::TransportPriority& priority);
    void policy(const DDS::Policy::Lifespan&          lifespan);
    void policy(const DDS::Policy::Ownership&         ownership);

#ifdef  OMG_DDS_OWNERSHIP_SUPPORT
    void policy(const DDS::Policy::OwnershipStrength& strength);
#endif  // OMG_DDS_OWNERSHIP_SUPPORT

    void policy(const DDS::Policy::WriterDataLifecycle&   lifecycle);

public:
    template <typename POLICY>
    const POLICY& policy() const;


public:
    static DataWriterQoS Default()
    {
        return DataWriterQoS();
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

#ifdef  OMG_DDS_OWNERSHIP_SUPPORT
    DDS::Policy::OwnershipStrength       strength_;
#endif  // OMG_DDS_OWNERSHIP_SUPPORT

    DDS::Policy::WriterDataLifecycle     lifecycle_;
};

}

#endif
