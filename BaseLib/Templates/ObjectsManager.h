#pragma once

#include <BaseLib/Status/ActivationStatus.h>

namespace BaseLib { namespace Templates {

template <typename K, typename V, typename ManagerId>
class ObjectsManagerBase
    : public Templates::Lifecycle
      , public Templates::Key1<ManagerId>
{
public:
    ObjectsManagerBase(const ManagerId& id)
        : Templates::Key1<ManagerId>(id)
    { }

    virtual ~ObjectsManagerBase()
    { }

    virtual bool ActivateById(K key)       = 0;
    virtual bool DeactivateById(K key)     = 0;
    virtual bool IsActiveById(K key) const = 0;
};

// TODO: Support LeasedValue<K,V> value;

template <typename K, typename V, typename ManagerId>
class ObjectsManager
    : public ObjectsManagerBase<K, V, ManagerId>
      , public Status::ActivationStatus
      , public Collection::details::StdMapCollectionType<K, V>
{
public:
    ObjectsManager(const ManagerId& id)
        : ObjectsManagerBase<K, V, ManagerId>(id)
    { }

    virtual ~ObjectsManager()
    { }

    virtual bool Activate() override
    {
        this->ForEach([](V base) { base->Activate(); });
        return Status::ActivationStatus::Activate();
    }

    virtual bool Deactivate() override
    {
        this->ForEach([](V base) { base->Deactivate(); });
        return Status::ActivationStatus::Deactivate();
    }

    virtual bool IsActive() const override
    {
        return Status::ActivationStatus::IsActive();
    }

    virtual bool ActivateById(K channelId)
    {
        return this->Apply(
            channelId,
            [](V base) { return base->Activate(); }
        );
    }

    virtual bool DeactivateById(K channelId)
    {
        return this->Apply(
            channelId,
            [](V base) { return base->Deactivate(); }
        );
    }

    virtual bool IsActiveById(K channelId) const
    {
        return this->Apply(
            channelId,
            [](V base) { return base->IsActive(); }
        );
    }

    virtual bool Initialize()
    {
        return true;
    }

    virtual bool IsInitialized() const
    {
        return true;
    }

    virtual bool Finalize()
    {
        return false;
    }

    virtual bool IsFinalized() const
    {
        return false;
    }
};

}}
