#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/StateMachineCollections.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Templates
{

template <typename T>
class Memento : protected ComparableMutableType<T>
{
public:
    Memento() : ComparableMutableType<T>()
    {}
    Memento(const T &t) : ComparableMutableType<T>(t)
    {}
    virtual ~Memento()
    {}

    T GetState() const
    {
        return this->Clone();
    }

    void SetState(const T &t)
    {
        this->Set(t);
    }
};

/**
-> Identify a class that needs to be able to take a snapshot of its state.(the originator role.)
-> Design a class that does nothing more than accept and return this snapshot.(The memento role).
-> Caretaker Role asks the Originator to return a Memento and cause the Originator's previous state to be restored of desired.
-> Notion of undo or rollback has now been objectified(i.e promoted to full object status).
 */
template <typename T>
class Originator : public ComparableMutableType< Memento<T> >
{
public:
    virtual ~Originator()
    {}

    CLASS_TRAITS(Originator)

    virtual bool SetMemento(Memento<T> t)
    {
        this->Set(t);
        return true;
    }

    virtual Memento<T> CreateMemento() const
    {
        return this->Clone();
    }

    virtual void SetOriginatorState(const T &t)
    {
        this->Set(Memento<T>(t));
    }

    virtual T GetOriginatorState() const
    {
        return this->delegate().GetState();
    }
};

// ------------------------------------------------
// Action base classes to be implemented
// ------------------------------------------------

template <typename T, typename Return>
class MementoAction0
        : public Action0<Return>
        , public Originator<T>
{
public:
    virtual ~MementoAction0() {}

    CLASS_TRAITS(MementoAction0)
};

template <typename T, typename Return, typename Arg1>
class MementoAction1
        : public Action1<Return, Arg1>
        , public Originator<T>
{
public:
    virtual ~MementoAction1() {}

    CLASS_TRAITS(MementoAction1)
};

template <typename T, typename Return, typename Arg1, typename Arg2>
class MementoAction2
        : public Action2<Return, Arg1, Arg2>
        , public Originator<T>
{
public:
    virtual ~MementoAction2() {}

    CLASS_TRAITS(MementoAction2)
};

template <typename T, typename Return, typename Arg1, typename Arg2, typename Arg3>
class MementoAction3
        : public Action3<Return, Arg1, Arg2, Arg3>
        , public Originator<T>
{
public:
    virtual ~MementoAction3() {}

    CLASS_TRAITS(MementoAction3)
};

template <typename T, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class MementoAction4
        : public Action4<Return, Arg1, Arg2, Arg3, Arg4>
        , public Originator<T>
{
public:
    virtual ~MementoAction4() {}

    CLASS_TRAITS(MementoAction4)
};

template <typename T, typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class MementoAction5
        : public Action5<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
        , public Originator<T>
{
public:
    virtual ~MementoAction5() {}

    CLASS_TRAITS(MementoAction5)
};

// ------------------------------------------------
// Undo redo base
// ------------------------------------------------

class DLL_STATE UndoRedoBase
{
public:
    virtual ~UndoRedoBase();

    virtual bool CreateMemento() const = 0;

    virtual bool Undo() = 0;

    virtual bool Redo() = 0;

    virtual void ClearUndo() = 0;

    virtual void ClearRedo() = 0;
};

/**
 * Perhaps inherit from map and implement specialized iterator functionality for undo, redo, etc.
 *
 * TODO: Use collections for states: UNDO, REDO, CURRENT
 *
 * CURRENT -> UNDO
 *
 * Use Caretaker in SharedTypePtr
 */
template <typename T>
class Caretaker : public UndoRedoBase
                , public Lockable<Mutex>
{
private:
    typedef Concurrent::StateMachineCollections<std::string, Originator<T>>   StateMachineCollection;
    typedef Concurrent::StateCollection<std::string, Originator<T>>           MementoCollection;

public:
    Caretaker(typename Originator<T>::Ptr originator)
        : stateCollections_(new StateMachineCollection())
        , originator_(originator)
    {}
    virtual ~Caretaker()
    {}

    CLASS_TRAITS(Caretaker)

    virtual bool CreateMemento() const
    {
        Memento<T> memento = originator_->CreateMemento();

        typename MementoCollection::Ptr collection = stateCollections_->GetCollection("UNDO");

        collection->template AddAction<bool, Memento<T>>(originator_, &Originator<T>::SetMemento, memento);

        stateCollections_->ClearCollection("REDO");

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
    mutable typename StateMachineCollection::Ptr    stateCollections_;
    mutable typename Originator<T>::Ptr             originator_;
};

/**
 * @brief The Caretakers class
 *
 * TODO: Make synchronized
 */
class DLL_STATE Caretakers
        : public Collection::IList<std::shared_ptr<UndoRedoBase>>
        , public UndoRedoBase
{
public:
    Caretakers();
    virtual ~Caretakers();

    virtual bool CreateMemento() const;
    virtual bool Undo();
    virtual bool Redo();
    virtual void ClearUndo();
    virtual void ClearRedo();
};

}}
