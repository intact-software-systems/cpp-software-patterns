#include "DCPS/Discovery/DiscoveryFactory.h"

namespace DCPS { namespace Discovery
{

Singleton<DiscoveryFactory> DiscoveryFactory::discoveryController_;


DiscoveryFactory::DiscoveryFactory()
{ }

DiscoveryFactory::~DiscoveryFactory()
{ }

DiscoveryFactory& DiscoveryFactory::Instance()
{
    return DiscoveryFactory::discoveryController_.GetRef();
}

}}
