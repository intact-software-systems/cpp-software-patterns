#include"RxConcurrent/Memento.h"

namespace BaseLib { namespace Templates
{

UndoRedoBase::~UndoRedoBase()
{}

Caretakers::Caretakers()
{}

Caretakers::~Caretakers()
{}

bool Caretakers::CreateMemento() const
{
    for(ConstIterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
    {
        std::shared_ptr<UndoRedoBase> base = *it;

        base->CreateMemento();
    }

    return true;
}

bool Caretakers::Undo()
{
    for(Iterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
    {
        std::shared_ptr<UndoRedoBase> base = *it;

        base->Undo();
    }

    return !List::empty();
}

bool Caretakers::Redo()
{
    for(Iterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
    {
        std::shared_ptr<UndoRedoBase> base = *it;

        base->Redo();
    }

    return !List::empty();
}

void Caretakers::ClearUndo()
{
    for(Iterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
    {
        std::shared_ptr<UndoRedoBase> base = *it;

        base->ClearUndo();
    }

}

void Caretakers::ClearRedo()
{
    for(Iterator it = List::begin(), it_end = List::end(); it != it_end; ++it)
    {
        std::shared_ptr<UndoRedoBase> base = *it;

        base->ClearRedo();
    }
}

}}
