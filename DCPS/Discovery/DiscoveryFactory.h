#ifndef DCPS_Discovery_DiscoveryFactory_h_IsIncluded
#define DCPS_Discovery_DiscoveryFactory_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Discovery
{

class DiscoveryFactoryState
{};

/**
 * @brief The DiscoveryFactory class
 *
 * TODO: Listen to the addition of new DomainParticipants in different domains?
 */
class DLL_STATE DiscoveryFactory : public Templates::ContextData<DiscoveryFactoryState>
{
public:
    DiscoveryFactory();
    virtual ~DiscoveryFactory();

    friend class Singleton<DiscoveryFactory>;

public:
    /**
     * @brief Instance
     * @return
     */
    static DiscoveryFactory& Instance();


private:
    static Singleton<DiscoveryFactory> discoveryController_;
};

}}

#endif
