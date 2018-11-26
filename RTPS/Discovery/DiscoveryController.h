#ifndef RTPS_Discovery_DiscoveryController_h_IsIncluded
#define RTPS_Discovery_DiscoveryController_h_IsIncluded

#include"RTPS/Elements/GUID.h"
#include"RTPS/Export.h"

namespace RTPS
{

/**
 * @brief The DiscoveryController class
 * TODO: An interface implemented by actual controllers, for example, SPDP and SEDP
 */
class DLL_STATE DiscoveryController
{
public:
    virtual bool AddParticipant(const GUID &guid) = 0;

    virtual bool RemoveParticipant(const GUID &guid) = 0;
};

}

#endif
