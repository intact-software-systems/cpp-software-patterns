#include"RxSignal/SignallerAlgorithms.h"

namespace BaseLib { namespace Concurrent {

SignallerAlgorithms::~SignallerAlgorithms()
{ }

void SignallerAlgorithms::scheduleSignal(Templates::Signal* signal)
{
    Templates::Signal::Ptr signalPtr(signal);

    ThreadPoolFactory::Instance().GetDefault()->Schedule(std::dynamic_pointer_cast<Runnable>(signalPtr), TaskPolicy::RunOnceImmediately(), false);
}

}}
