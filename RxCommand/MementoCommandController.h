#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandControllerAction.h"
#include"RxCommand/MementoCommand.h"

namespace Reactor
{

/**
 * Command objects can offer an undo mechanism by retaining the state necessary to
 * roll back the behavior they execute. If the state is heavyweight or hard to restore,
 * however, a MEMENTO (414) that snapshots the receiver’s state before executing the
 * command may provide a simpler, more lightweight option.
 *
 * TODO: Caretakers is not thread safe
 */
template <typename Return>
class MementoCommandController
        : public CommandControllerAction<Return>
        , public Templates::UndoRedoBase
{
public:
    MementoCommandController(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : CommandControllerAction<Return>(policy, threadPool)
    {}
    virtual ~MementoCommandController()
    {}

    CLASS_TRAITS(MementoCommandController)

    // ------------------------------------------
    // Interface UndoRedoBase
    // ------------------------------------------

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

    virtual bool CreateMemento() const
    {
        return caretakers_.CreateMemento();
    }

    // -----------------------------------------------------------
    // Interface CommandObserver <T>
    // -----------------------------------------------------------

    virtual bool OnComplete(Command<Return>* command)
    {
        IINFO() << "Completed command " << command->GetName();
        this->processOnComplete(command);
        return true;
    }

    virtual bool OnNext(Command<Return>* command, Return value)
    {
        IINFO() << "Command " << command->GetName() << " value: " << value;

        caretakers_.CreateMemento();

        this->processOnNext(command, value);

        return true;
    }

    virtual bool OnError(Command<Return>* command, BaseLib::GeneralException exception)
    {
        IINFO() << "Command " << command->GetName() << " exception: " << exception.what();
        this->processOnError(command, exception);
        return true;
    }


    // ------------------------------------------
    // Add functions
    // ------------------------------------------

    bool Add(typename MementoCommandActions<Return>::Ptr command)
    {
        CommandBase::Ptr commandBase = std::dynamic_pointer_cast<CommandBase>(command);

        ASSERT(commandBase != nullptr);

        bool isAdded = this->AddCommand(commandBase);
        if(isAdded)
        {
            //Locker locker(this);
            caretakers_.push_back(command);
        }

        return isAdded;
    }

private:
    Templates::Caretakers caretakers_;
};

// ---------------------------------------------------------------
// MementoCommandControllerKeyValue
// ---------------------------------------------------------------

template <typename K, typename V>
class MementoCommandControllerKeyValue
        : public MementoCommandController<IMap<K,V>>
{
public:
    MementoCommandControllerKeyValue(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : MementoCommandController<IMap<K,V>>(policy, threadPool)
    { }
    virtual ~MementoCommandControllerKeyValue()
    { }

    CLASS_TRAITS(MementoCommandControllerKeyValue)
};

}
