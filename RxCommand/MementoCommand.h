#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Command.h"

namespace Reactor
{

template <typename Return>
class MementoCommandActions
        : public CommandActions<Return>
        , public Templates::UndoRedoBase
{
protected:
    typedef MutexTypeLocker<MementoCommandActions<Return>>  Locker;

public:
    MementoCommandActions(CommandPolicy policy = CommandPolicy::Default())
        : CommandActions<Return>(policy)
    {}
    virtual ~MementoCommandActions()
    {}

    CLASS_TRAITS(MementoCommandActions)

    // ------------------------------------------
    // Interface UndoRedoBase
    // ------------------------------------------

    virtual bool CreateMemento() const
    {
        return caretakers_.CreateMemento();
    }

    virtual bool Undo()
    {
        return caretakers_.Undo();
    }

    virtual bool Redo()
    {
        return caretakers_.Redo();
    }

    virtual void ClearUndo()
    {
        caretakers_.ClearUndo();
    }

    virtual void ClearRedo()
    {
        caretakers_.ClearRedo();
    }

    // ------------------------------------------
    // Add functions
    // ------------------------------------------

    template <typename T>
    void Add(typename Templates::MementoAction0<T, Return>::Ptr action)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>::Ptr>(action));
        CommandActions<Return>::Add(action);
    }

    template <typename T, typename Arg1>
    void Add(typename Templates::MementoAction1<T, Return, Arg1>::Ptr action, Arg1 arg1)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>>(action));
        CommandActions<Return>::template Add<Arg1>(action, arg1);
    }

    template <typename T, typename Arg1, typename Arg2>
    void Add(typename Templates::MementoAction2<T, Return, Arg1, Arg2>::Ptr action, Arg1 arg1, Arg2 arg2)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>>(action));
        CommandActions<Return>::template Add<Arg1, Arg2>(action, arg1, arg2);
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3>
    void Add(typename Templates::MementoAction3<T, Return, Arg1, Arg2, Arg3>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>>(action));
        CommandActions<Return>::template Add<Arg1, Arg2, Arg3>(action, arg1, arg2, arg3);
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void Add(typename Templates::MementoAction4<T, Return, Arg1, Arg2, Arg3, Arg4>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>>(action));
        CommandActions<Return>::template Add<Arg1, Arg2, Arg3, Arg4>(action, arg1, arg2, arg3, arg4);
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    void Add(typename Templates::MementoAction5<T, Return, Arg1, Arg2, Arg3, Arg4, Arg5>::Ptr action, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        createCaretaker<T>(std::dynamic_pointer_cast<Templates::Originator<T>>(action));
        CommandActions<Return>::template Add<Arg1, Arg2, Arg3, Arg4, Arg5>(action, arg1, arg2, arg3, arg4, arg5);
    }

    // -----------------------------------------------------------
    // Interface RxObserver<T>
    // -----------------------------------------------------------

    virtual bool OnComplete()
    {
        caretakers_.CreateMemento();
        return this->processOnComplete();
    }

    virtual bool OnNext(Return)
    {
        // Note: Currently, this is only used from this command's CompositionStrategy.
        return true;
    }

    virtual bool OnError(GeneralException throwable)
    {
        return this->processOnError(throwable);
    }

private:
    // -----------------------------------------------------------
    // private functions
    // -----------------------------------------------------------

    template <typename T>
    void createCaretaker(std::shared_ptr<Templates::Originator<T>> originator)
    {
        std::shared_ptr<Templates::Caretaker<T>> caretaker(new Templates::Caretaker<T>(originator));

        caretakers_.push_back(caretaker);
    }

private:
    Templates::Caretakers caretakers_;
};

}

