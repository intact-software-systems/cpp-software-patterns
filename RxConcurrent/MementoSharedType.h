#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/StateMachineCollections.h"
#include"RxConcurrent/Memento.h"

namespace BaseLib { namespace Templates
{

template <typename T, typename Lock = NullMutex>
class MementoSharedTypePtr
    : public SharedTypePtr<T, Lock>
      , public UndoRedoBase
      , public ENABLE_SHARED_FROM_THIS_T2(MementoSharedTypePtr, T, Lock)
{
private:
    typedef Concurrent::StateMachineCollections<std::string, MementoSharedTypePtr<T>>   StateMachineCollection;
    typedef Concurrent::StateCollection<std::string, MementoSharedTypePtr<T>>           MementoCollection;

public:
    MementoSharedTypePtr(T *t, Policy::UndoRedo undo = Policy::UndoRedo::UndoAndRedo())
        : SharedTypePtr<T, Lock>(t, Policy::Flyweight::CopyOnWrite())
        , undo_(undo)
    {}
    MementoSharedTypePtr(std::shared_ptr<T> t, Policy::UndoRedo undo = Policy::UndoRedo::UndoAndRedo())
        : SharedTypePtr<T, Lock>(t, Policy::Flyweight::CopyOnWrite())
        , undo_(undo)
    {}
    virtual ~MementoSharedTypePtr()
    { }

    CLASS_TRAITS(MementoSharedTypePtr)

    // --------------------------------------
    // UndoRedoBase interface
    // --------------------------------------

    virtual bool CreateMemento() const
    {
        Memento<T> memento = createMemento();
        addMemento(memento);
        return true;
    }

    virtual bool Undo()
    {
        return stateCollections_->ExecuteNObjectsAndMove(1, "UNDO", "REDO");
    }

    virtual bool Redo()
    {
        return stateCollections_->ExecuteNObjectsAndMove(1, "REDO", "UNDO");
    }

    virtual void ClearUndo()
    {
        stateCollections_->ClearCollection("UNDO");
    }

    virtual void ClearRedo()
    {
        stateCollections_->ClearCollection("REDO");
    }

private:

    virtual void detach()
    {
        if(this->flyweight_.IsCopyOnWrite())
        {
            // -- debug --
            IINFO() << "Detached the pointer because use count " << this->t_.use_count();
            ASSERT(this->t_.use_count() > 1);
            // -- debug --

            T *local = this->t_.get();
            T t(*local);

            Memento<T> memento(t);
            addMemento(memento);

            this->t_ = std::shared_ptr<T>(new T(*local));
        }
    }

    Memento<T> createMemento() const
    {
        T *local = this->t_.get();
        T t(*local);

        return Memento<T>(t);
    }

    void addMemento(const Memento<T> &memento) const
    {
        typename MementoCollection::Ptr collection = stateCollections_->GetCollection("UNDO");

        collection->template AddAction<bool, Memento<T>>(
            this->shared_from_this(),
                &MementoSharedTypePtr<T>::Set,
                memento);

        stateCollections_->ClearCollection("REDO");
    }

private:
    mutable typename StateMachineCollection::Ptr stateCollections_;
    Policy::UndoRedo undo_;
};

}}
