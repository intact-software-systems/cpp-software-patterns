#pragma once

#include "RxActor/CommonDefines.h"

namespace RxActor {

// ----------------------------------------------------------------
// ProcessorContext == ActionContext
// ----------------------------------------------------------------

class ProcessorContext
{
public:
    virtual ~ProcessorContext()
    { }

    virtual void OnPreStart()                                = 0;
    virtual void OnPostStop()                                = 0;
    virtual void OnPreRestart(BaseLib::Exception exception)  = 0;
    virtual void OnPostRestart(BaseLib::Exception exception) = 0;
};

typedef std::shared_ptr<ProcessorContext> ProcessorContextPtr;

template <typename T>
class Processor : public ProcessorContext
{
public:
    virtual ~Processor()
    { }

    virtual bool Process(RxActor::Actor<T>*, T value) = 0;
};

class ProcessorContextInvoker : public ProcessorContext
{
public:
    ProcessorContextInvoker(
        std::function<void()> onPreStart,
        std::function<void()> onPostStop,
        std::function<void(Exception)> onPreRestart,
        std::function<void(Exception)> onPostRestart
    )
        : onPreStart_(onPreStart)
        , onPostStop_(onPostStop)
        , onPreRestart_(onPreRestart)
        , onPostRestart_(onPostRestart)
    { }

    virtual ~ProcessorContextInvoker()
    { }

    virtual void OnPreStart()
    {
        onPreStart_();
    }

    virtual void OnPostStop()
    {
        onPostStop_();
    }

    virtual void OnPreRestart(Exception e)
    {
        onPreRestart_(e);
    }

    virtual void OnPostRestart(Exception e)
    {
        onPostRestart_(e);
    }

private:
    std::function<void()>          onPreStart_    = [] { };
    std::function<void()>          onPostStop_    = [] { };
    std::function<void(Exception)> onPreRestart_  = [](Exception e) { };
    std::function<void(Exception)> onPostRestart_ = [](Exception e) { };
};


template <typename T>
class ProcessorInvoker : public Processor<T>
{
public:
    ProcessorInvoker(const ProcessorContextPtr& context, std::function<bool(RxActor::Actor<T>*, T)> processor)
        : context_(context)
        , processor_(processor)
    { }

    virtual ~ProcessorInvoker()
    { }

    // alternative: Act(T)
    virtual bool Process(RxActor::Actor<T>* actor, T value)
    {
        return processor_(actor, value);
    }

    virtual void OnPreStart()
    {
        context_->OnPreStart();
    }

    virtual void OnPostStop()
    {
        context_->OnPostStop();
    }

    virtual void OnPreRestart(Exception e)
    {
        context_->OnPreRestart(e);
    }

    virtual void OnPostRestart(Exception e)
    {
        context_->OnPostRestart(e);
    }

private:
    ProcessorContextPtr                        context_;
    std::function<bool(RxActor::Actor<T>*, T)> processor_;
};

}