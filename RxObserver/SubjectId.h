#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

class DLL_STATE SubjectId
        : public Templates::Function
{
public:
    template <typename EventSignature>
    SubjectId(EventSignature id)
        : Templates::Function(id)
    {}

    SubjectId(Templates::Any any);

    virtual ~SubjectId();
};

}}

