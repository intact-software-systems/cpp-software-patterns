#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

namespace UndoRedoKind {
enum Type
{
    UNDO,
    REDO,
    UNDO_AND_REDO
};
}

class DLL_STATE UndoRedo
{
public:
    UndoRedo(UndoRedoKind::Type kind, int undoDepth, int redoDepth)
        : kind_(kind)
        , undoDepth_(undoDepth)
        , redoDepth_(redoDepth)
    { }

    virtual ~UndoRedo()
    { }

    UndoRedoKind::Type Kind() const
    {
        return kind_;
    }

    int UndoDepth() const
    {
        return undoDepth_;
    }

    int RedoDepth() const
    {
        return redoDepth_;
    }

    static UndoRedo Undo(int depth = 5)
    {
        return UndoRedo(UndoRedoKind::UNDO, depth, 0);
    }

    static UndoRedo Redo(int depth = 5)
    {
        return UndoRedo(UndoRedoKind::REDO, 0, depth);
    }

    static UndoRedo UndoAndRedo(int depth = 5)
    {
        return UndoRedo(UndoRedoKind::UNDO_AND_REDO, depth, depth);
    }

    bool operator==(const UndoRedo& other)
    {
        return kind_ == other.kind_;
    }

private:
    UndoRedoKind::Type kind_;
    int                undoDepth_;
    int                redoDepth_;
};

}}
