#pragma once

#include"RxDiscovery/IncludeExtLibs.h"
#include"RxDiscovery/Export.h"

namespace RxDiscovery {

/**
 Discovery:
  - Support channels, etc
  - K = id
  - V = ServiceData, ChannelData, EntityData, etc

  I need to support:
  - Liveliness of entities - through heartbeats, lease-durations and timeouts.
 */

template <typename K, typename V>
class Discovery;

template <typename K, typename V>
class LookupResource;

template <typename K, typename V>
class ServiceDirectory;

template <typename K, typename V>
class ResourceDirectory;

class ServiceHealth;

class ServiceResourceUsage;

enum class ResourceStatusKind
{
    Register,
    AliveCheck,
    Unregister,
    Active,
    Inactive
};

enum class ResourceManagerStatusKind
{
    Announce,
    Query,
};

enum class ResourceUsage
{
    RUNNING,
    STOPPED,
    BANDWIDTH
};

typedef Status::EventStatusTracker<ResourceStatusKind, int> ResourceStatus;

}
