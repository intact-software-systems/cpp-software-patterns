#include"MicroMiddleware/Net/IPSubscriberListener.h"
#include"MicroMiddleware/Net/IPSubscriber.h"

namespace MicroMiddleware {

IPSubscriberListener::IPSubscriberListener(const InterfaceHandle& interfaceHandle, bool autoStart)
    : runThread_(autoStart)
    , interfaceHandle_(interfaceHandle)
{
    if(runThread_) start();
}

IPSubscriberListener::~IPSubscriberListener()
{
    MutexLocker lock(&clientMutex_);
    cleanUp();
}

void IPSubscriberListener::initIPSubscriberListener()
{
    IPSubscriber* ipSubscriber = new IPSubscriber(this, interfaceHandle_);

    ASSERT(setIPSubscriber_.count(ipSubscriber) <= 0);
    setIPSubscriber_.insert(ipSubscriber);
}

void IPSubscriberListener::run()
{
    try
    {
        clientMutex_.lock();
        initIPSubscriberListener();
        clientMutex_.unlock();

        while(runThread_)
        {
            msleep(1000);
            //check all IPSubscriberListenerClients
        }
    }
    catch(Exception ex)
    {
        IDEBUG() << "ERROR! Exception caught! " << ex.msg();
    }
    catch(...)
    {
        IDEBUG() << "ERROR! Exception caught! ";
    }
}

void IPSubscriberListener::RMInvocation(int methodId, RmiBase* rmiServerBase)
{
    switch(methodId)
    {
        // TODO add middleware exception handling etc.....
        default:
            IDEBUG() << "ERROR! function id not supported : " << methodId;
            break;
    }
}

bool IPSubscriberListener::StartIPSubscriber(const InterfaceHandle& interfaceHandle)
{
    MutexLocker lock(&clientMutex_);

    IPSubscriber* ipSubscriber = findIPSubscriber(interfaceHandle);
    if(ipSubscriber != NULL)
    {
        IDEBUG() << "ERROR! IPSubscriber already exists for given interface " << interfaceHandle;
        return false;
    }

    ipSubscriber = new IPSubscriber(this, interfaceHandle, true);

    ASSERT(setIPSubscriber_.count(ipSubscriber) <= 0);
    setIPSubscriber_.insert(ipSubscriber);

    return true;
}

void IPSubscriberListener::StopIPSubscriber(IPSubscriber* ipSubscriber)
{
    MutexLocker lock(&clientMutex_);

    if(setIPSubscriber_.count(ipSubscriber) <= 0)
        IDEBUG() << "WARNING! IPSubscriber not found in set! ";

    //ASSERT(setIPSubscriber_.count(ipSubscriber) >= 1);

    setIPSubscriber_.erase(ipSubscriber);
}

bool IPSubscriberListener::StopIPSubscriber(const InterfaceHandle& interfaceHandle)
{
    MutexLocker lock(&clientMutex_);

    IPSubscriber* ipSubscriber = findIPSubscriber(interfaceHandle);
    if(ipSubscriber == NULL)
    {
        IDEBUG() << "ERROR! IPSubscriber does not exist for given interface " << interfaceHandle;
        return false;
    }

    delete ipSubscriber;
    setIPSubscriber_.erase(ipSubscriber);
    return true;
}

void IPSubscriberListener::Stop(bool waitForTermination)
{
    MutexLocker lock(&clientMutex_);
    if(isRunning())
    {
        runThread_ = false;
        if(waitForTermination)
        {
            wait();
        }

        cleanUp();
    }
}

void IPSubscriberListener::cleanUp()
{
    for(SetIPSubscriber::iterator it = setIPSubscriber_.begin(), it_end = setIPSubscriber_.end(); it != it_end; ++it)
    {
        IPSubscriber* ipSubscriber = *it;
        delete ipSubscriber;
    }
    setIPSubscriber_.clear();
}

IPSubscriber* IPSubscriberListener::findIPSubscriber(const InterfaceHandle& handle)
{
    for(SetIPSubscriber::iterator it = setIPSubscriber_.begin(), it_end = setIPSubscriber_.end(); it != it_end; ++it)
    {
        IPSubscriber* ipSubscriber = *it;
        ASSERT(ipSubscriber);

        if(ipSubscriber->GetInterfaceHandle() == handle)
        {
            return ipSubscriber;
        }
    }

	return NULL;
}

}; // namespace MicroMiddleware



