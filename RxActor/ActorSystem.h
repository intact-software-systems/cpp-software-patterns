#pragma once

#include "RxActor/CommonDefines.h"
#include "RxActor/Supervisor.h"
#include "RxActor/Export.h"
#include "Actor.h"

namespace RxActor {

typedef Templates::Key2<DomainId, ScopeId>                                                 ActorSystemHandle;
typedef Templates::ObjectsManager<ActorPath, ActorBasePtr, ActorSystemHandle>              ActorScopeManager;
typedef std::shared_ptr<ActorScopeManager>                                                 ActorScopeManagerPtr;
typedef Collection::details::StdMapCollectionType<ActorSystemHandle, ActorScopeManagerPtr> LookupActorManager;

class DLL_STATE ActorSystem
    : public Templates::FactorySingleton<ActorSystem>
      , protected Templates::ContextDataShared<LookupActorManager>
{
public:
    ActorSystem() : Templates::ContextDataShared<LookupActorManager>()
    { }

    virtual ~ActorSystem()
    { }

    // -------------------------------------------------
    // Activation functions. TODO: Which vocabulary makes sense for Actors?
    // -------------------------------------------------

    bool Activate(ActorId handle);
    bool Deactivate(ActorId handle);
    bool IsActive(ActorId handle);

    bool Activate(ActorPath handle);
    bool Deactivate(ActorPath handle);
    bool IsActive(ActorPath handle);

    // -------------------------------------------------
    // Create functions
    // -------------------------------------------------

    template <typename T>
    std::shared_ptr<details::Actor<T>> ActorOf(ChannelHandle handle, Supervisor supervisor, ActorPolicy policy)
    {
        ActorScopeManagerPtr manager = this->context()->GetOrCreate(
            ActorSystemHandle{handle.first(), handle.second()},
            [&handle]()
            {
                std::shared_ptr<ActorScopeManager> instance = std::make_shared<ActorScopeManager>(ActorSystemHandle{handle.first(), handle.second()});

                bool initialized = instance->Initialize();
                ASSERT(initialized);

                return instance;
            }
        );

        ActorBasePtr actor = manager->GetOrCreate(
            handle.third(),
            [&handle, &supervisor, &policy]()
            {
                std::shared_ptr<details::Actor<T>> actor = std::make_shared<details::Actor<T>>(handle, ActorId::Create(), supervisor, policy);

                bool initialized = actor->Initialize();
                ASSERT(initialized);

                return actor;
            }
        );

        return std::dynamic_pointer_cast<details::Actor<T>>(actor);
    }

    // ---------------------------------------------
    // Print data
    // ---------------------------------------------

    std::string ToString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const ActorSystem& proxy)
    {
        ostr << proxy.ToString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<ActorSystem>& proxy)
    {
        ostr << proxy->ToString();
        return ostr;
    }
};

}



