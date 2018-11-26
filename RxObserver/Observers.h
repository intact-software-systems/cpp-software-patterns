#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/ObserverBase.h"
#include"RxObserver/EventResult.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// ObserversPolicy
// ----------------------------------------------------

class ObserversPolicy
{
public:
    ObserversPolicy()
    {}
    virtual ~ObserversPolicy()
    {}

    static ObserversPolicy Default()
    {
        return ObserversPolicy();
    }
};

// ----------------------------------------------------
// ObserversState
// ----------------------------------------------------

class DLL_STATE ObserversState
{
public:
    typedef Collection::IList<std::shared_ptr<ObserverAction>> ObserverList;

public:
    ObserversState(ObserverList observers = ObserverList());
    virtual ~ObserversState();

    ObserverList& Obs();
    const ObserverList& Obs() const;

private:
    ObserverList observers_;
};

// ----------------------------------------------------
// Observers
// ----------------------------------------------------

class DLL_STATE Observers
        : public Templates::ReactorMethods<EventResults, Events, BaseLib::Exception>
        , public Templates::ContextObject<ObserversPolicy, ObserversState>
        , public Templates::LockableType<Observers, Mutex>
        , public ENABLE_SHARED_FROM_THIS(Observers)
{
public:
    Observers(const ObserversState::ObserverList &observers, ObserversPolicy policy = ObserversPolicy::Default());
    Observers(ObserversPolicy policy = ObserversPolicy::Default());
    virtual ~Observers();

    CLASS_TRAITS(Observers)

    Observers::Ptr GetPtr();

    // ---------------------------------------------------------
    // Observer managment
    // ---------------------------------------------------------

    void Add(std::shared_ptr<ObserverAction> observer);
    bool Remove(std::shared_ptr<ObserverAction> observer);

    bool Unsubscribe();

    bool IsEmpty() const;
    int Size() const;

    bool IsSubscribed() const;

    bool Block();
    bool Unblock();
    bool IsBlocked() const;

    // ---------------------------------------------------------
    // Reactor methods
    // ---------------------------------------------------------

    virtual EventResults Next(Events events);
    virtual bool         Complete();
    virtual bool         Error(BaseLib::Exception exception);

    // ---------------------------------------------------------
    // template function
    // ---------------------------------------------------------

    template <typename Class>
    bool UnsubscribeType(Class delegate)
    {
        Templates::AnyKey delegateKey(delegate);

        for(ObserversState::ObserverList::iterator it = this->state().Obs().begin(), it_end = this->state().Obs().end(); it != it_end; ++it)
        {
            std::shared_ptr<ObserverAction> observer = *it;

            if(observer->Delegate() == delegateKey)
            {
                observer->Unsubscribe();
                this->state().Obs().erase(it);

                return true;
            }
        }

        return false;
    }

    template <typename Delegate>
    bool IsSubscribedType(Delegate delegate) const
    {
        for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
        {
            if(observer->Delegate() == delegate)
            {
                return observer->IsSubscribed();
            }
        }

        return false;
    }
};

}}
