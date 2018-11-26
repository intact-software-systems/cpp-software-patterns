#ifndef DDS_Domain_DomainParticipantQoS_h_IsIncluded
#define DDS_Domain_DomainParticipantQoS_h_IsIncluded

#include"DDS/Policy/IncludeLibs.h"
#include"DDS/Export.h"

namespace DDS
{

/**
 * @brief The DomainParticipantQoS class
 */
class DLL_STATE DomainParticipantQoS
{
public:
    DomainParticipantQoS();
    DomainParticipantQoS(const DDS::Policy::UserData& user_data, const DDS::Policy::EntityFactory& entity_factory);

public:
    template <typename POLICY>
    const POLICY& policy() const;

    template <typename POLICY>
    POLICY& policy();

public:
    void policy(const DDS::Policy::UserData& ud);
    void policy(const DDS::Policy::EntityFactory& efp);

    /**
     * @brief Default
     * @return
     */
    static DomainParticipantQoS Default()
    {
        return DomainParticipantQoS();
    }

private:
    DDS::Policy::UserData user_data_;
    DDS::Policy::EntityFactory entity_factory_;
};

}

#endif
