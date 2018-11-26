#ifndef DDS_Domain_DomainParticipantFactoryQoS_h_IsIncluded
#define DDS_Domain_DomainParticipantFactoryQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS
{

/**
 * @brief The DomainParticipantFactoryQoS class holds the QoS relevant for the singleton DomainParticipantFactory.
 */
class DLL_STATE DomainParticipantFactoryQoS
{
public:
    DomainParticipantFactoryQoS();
    DomainParticipantFactoryQoS(const DDS::Policy::EntityFactory& entityFactory);

public:
    template <typename POLICY>
    const POLICY& policy() const;

    template <typename POLICY>
    POLICY& policy();

public:
    void policy(const DDS::Policy::EntityFactory& efp);

    /**
     * @brief Default
     * @return
     */
    static DomainParticipantFactoryQoS Default()
    {
        return DomainParticipantFactoryQoS();
    }

private:
    DDS::Policy::EntityFactory entityFactory_;
};

}

#endif
