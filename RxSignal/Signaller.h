#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/SignalSlotBase.h"
#include"RxSignal/SignallerAlgorithms.h"

#include"RxSignal/Export.h"

namespace BaseLib { namespace Concurrent
{

/**
 * @brief The Signaller class, aka. Publisher, EventDispatcher, to be used by Subject.
 *
 * TODO:
 * - Add support for any number of arguments by using NoArgFunctionPointerInvoker, OneArgFunctionPointerInvoker, TwoArgFunctionPointerInvoker, etc ...
 * - Add support for replay of events
 * - Add support for scalable asynchronous dispatch of events.
 * - Add a baseclass "class Event" or similar with support for invoke or dispatch of event to handler/observer.
 * - Add event meta data for monitoring: timestamp, etc
 */
class DLL_STATE Signaller
        : public SignallerBase
        , public SignallerAlgorithms
{
public:
    Signaller();
    virtual ~Signaller();

    friend class SignallerAlgorithms;

    CLASS_TRAITS(Signaller)

    virtual void Block();
    virtual void Unblock();

    virtual bool IsBlocked() const;

    InstanceHandle Handle() const;

protected:
    void incHandle();

private:
    InstanceHandle              handle_;
    Templates::ProtectedBool    block_;
};

}}
