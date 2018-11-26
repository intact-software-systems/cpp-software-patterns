#ifndef RTPS_Discovery_SPDP_DiscoveredParticipantData_h_IsIncluded
#define RTPS_Discovery_SPDP_DiscoveredParticipantData_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Discovery/ParticipantProxy.h"
#include"RTPS/Export.h"

namespace RTPS { namespace SPDP
{

/*
    struct SPDPdiscoveredParticipantData
    {
        struct DDS::ParticipantBuiltinTopicData ddsParticipantData;
        struct participantProxy participantProxy;
        Duration_t leaseDuration;
    };
*/

/**
 * @brief The SPDPdiscoveredParticipantData class
 */
class DLL_STATE SPDPDiscoveredParticipantData
{
public:
    SPDPDiscoveredParticipantData(ParticipantProxy participantProxy, Duration duration = Duration::FromMilliseconds(5000));
    virtual ~SPDPDiscoveredParticipantData();

    Duration leaseDuration() const;
    void setLeaseDuration(const Duration &leaseDuration);

    ParticipantProxy participantProxy() const;
    void setParticipantProxy(const ParticipantProxy &participantProxy);

private:
    DDS::Discovery::ParticipantBuiltinTopicData ddsParticipantData_;
    ParticipantProxy participantProxy_;
    Duration leaseDuration_;
};

}}

#endif
