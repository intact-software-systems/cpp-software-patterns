#include"RxObserver/Observers.h"
#include"RxObserver/FlowControlStrategy.h"
#include"RxObserver/ObserverEvent.h"
#include"RxObserver/EventResult.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// ObserversState
// ----------------------------------------------------

ObserversState::ObserversState(ObserverList observers)
    : observers_(observers)
{ }

ObserversState::~ObserversState()
{ }

ObserversState::ObserverList &ObserversState::Obs()
{
    return this->observers_;
}

const ObserversState::ObserverList &ObserversState::Obs() const
{
    return this->observers_;
}

// ----------------------------------------------------
// Observers
// ----------------------------------------------------
Observers::Observers(const ObserversState::ObserverList &observers, ObserversPolicy policy)
    : Templates::ContextObject<ObserversPolicy, ObserversState>
      (
          policy,
          ObserversState(observers)
      )
{}

Observers::Observers(ObserversPolicy policy)
    : Templates::ContextObject<ObserversPolicy, ObserversState>
      (
          policy,
          ObserversState()
      )
{}

Observers::~Observers()
{}

Observers::Ptr Observers::GetPtr()
{
    return this->shared_from_this();
}

// ----------------------------------------------------
// Observers managment
// ----------------------------------------------------

void Observers::Add(std::shared_ptr<ObserverAction> observer)
{
    Locker lock(this);

    IINFO() << "Adding observer " << observer.operator *();

    this->state().Obs().push_back(observer);
}

bool Observers::Remove(std::shared_ptr<ObserverAction> observer)
{
    Locker lock(this);

    for(ObserversState::ObserverList::iterator it = this->state().Obs().begin(), it_end = this->state().Obs().end(); it != it_end; ++it)
    {
        std::shared_ptr<ObserverAction> attachedObserver = *it;

        if(attachedObserver->EventId() == observer->EventId())
        {
            //IINFO() << attachedObserver.operator *() << " == " << observer.operator *();
            this->state().Obs().erase(it);
            return true;
        }
    }

    IINFO() << "Attached: " << this->state().Obs().size();
    ICRITICAL() << "Unable to find observer to remove: " << observer.operator *();

    return false;
}

bool Observers::Unsubscribe()
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        observer->Unsubscribe();
    }
    return true;
}

bool Observers::IsEmpty() const
{
    Locker lock(this);
    return this->state().Obs().empty();
}

int Observers::Size() const
{
    Locker lock(this);
    return this->state().Obs().size();
}

bool Observers::IsSubscribed() const
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        if(observer->IsSubscribed())
            return true;
    }
    return false;
}

bool Observers::Block()
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        observer->Block();
    }
    return true;
}

bool Observers::Unblock()
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        observer->Unblock();
    }
    return true;
}

bool Observers::IsBlocked() const
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        if(!observer->IsBlocked())
            return false;
    }
    return true;
}

// ---------------------------------------------------------
// Reactor methods
// ---------------------------------------------------------

EventResults Observers::Next(Events events)
{
    return Strategy::PlayEventsToObservers::Instance().Perform(
                this->GetPtr(),
                events,
                Policy::Computation::Parallel(),
                Policy::Timeout::FromMilliseconds(1000)
    );
}

bool Observers::Complete()
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        observer->OnComplete();
    }
    return true;
}

bool Observers::Error(Exception exception)
{
    Locker lock(this);
    for(std::shared_ptr<ObserverAction> observer : this->state().Obs())
    {
        observer->OnError(exception);
    }
    return true;
}

}}

