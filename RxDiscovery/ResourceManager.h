#pragma once

#include "RxDiscovery/CommonDefines.h"

namespace RxDiscovery {

struct ResourceManagerPolicy
{
    Policy::Liveliness liveliness = Policy::Liveliness::ManualByParticipant(Duration::FromSeconds(10));
    Policy::Activation activation = Policy::Activation::AutoActivate();
};

struct ResourcePolicy
{
    Policy::Liveliness liveliness = Policy::Liveliness::ManualByParticipant(Duration::FromSeconds(10));
    Policy::Activation activation = Policy::Activation::AutoActivate();
};

struct ResourceManagerData
{
};

template <typename Proxy>
struct Locator
{
    std::function<Proxy ()> proxyCreator_; //  = [] () { return Proxy(); };


};


/**
 * Rename to ServiceData?
 *
 * - Status on "Resource Usage"? Use my Metric implementation
 * - Status on "Health"
 */
template <typename K, typename V>
struct ResourceData : public Status::ActivationStatus
{
    const V data;
    const K key;

    const ResourcePolicy policy;
    const ResourceStatus status;
};

/**
 * Discovery:
 * - The algorithm and data-flow involved in spreading the meta-information about a new "service"
 * - Policies in discovery may be, see DDS, liveliness, activation
 *
 * LookupService:
 * - Queries the discovery regarding how to use service (join, login, contact, write-to, read-from)
 * - Returns a Proxy that can be used to take part/contact the service.
 *
 * Rename to ServiceDirectory
 *  - How is it a directory? /channel.name/a, /channel.name/b
 *  - The ServiceDirectory itself has the directory-name, and stores services in its own directory
 *
 * ServiceDirectory
 * - has an id and a <key,value> map of services
 * - value is a data structure containing meta information regarding a service, locators, names, policies, etc.
 *
 * Associate a ServiceDirectory with
 * - ServiceManager that manages local proxies, instantiated with a ProxyFactory.
 *
 *
 * Other names: ChannelDirectory, TopicDirectory, ParticipantDirectory, EntityDirectory, RepositoryDirectory, CacheDirectory
 *
 *
 * Obtain reference to a Service via a LeasePointer?
 *
 * template <typename T>
 * class Lease
 * {
 *    T value;
 *
 *    T lock(); // checks if t is expired, outdated, etc.
 * };
 *
 */
template <typename K, typename V, typename ManagerId>
class ResourceManager
    : public Templates::KeyValueObjectObserver<K, V>
      , public Templates::ObjectsManagerBase<K, V, ManagerId>
      , public Templates::ContextObjectShared<ResourceManagerPolicy, ResourceManagerData>
{
public:
    ResourceManager(const ManagerId& id, ResourceManagerPolicy* config, ResourceManagerData* data)
        : Templates::ObjectsManagerBase<K, V, ManagerId>(id)
        , Templates::ContextObjectShared<ResourceManagerPolicy, ResourceManagerData>(config, data)
    { }

    virtual ~ResourceManager()
    { }

    virtual bool CheckIn(K key, V value)
    {
        // Alive checker: Cache with RenewOnWrite
        return false;
    }


    void UpdateResourceUsage()
    {

    }

    // --------------------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------------------

    virtual bool Activate()
    {
        return false;
    }

    virtual bool Deactivate()
    {
        return false;
    }

    virtual bool IsActive() const
    {
        return false;
    }

    virtual bool ActivateById(K key)
    {
        return false;
    }

    virtual bool DeactivateById(K key)
    {
        return false;
    }

    virtual bool IsActiveById(K key) const
    {
        return false;
    }

    // --------------------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------------------

    virtual bool Initialize()
    {
        return false;
    }

    virtual bool IsInitialized() const
    {
        return false;
    }

    virtual bool Finalize()
    {
        return false;
    }

    virtual bool IsFinalized() const
    {
        return false;
    }

    // --------------------------------------------------------
    // Interface ReactorMethods
    // --------------------------------------------------------

    virtual void OnObjectCreated(K key, V value)
    {
        // Add locator

    }

    virtual void OnObjectDeleted(K key)
    { }

    virtual void OnObjectModified(K key, V value)
    { }
};

}


