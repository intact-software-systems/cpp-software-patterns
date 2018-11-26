#pragma once

#include<vector>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware {

class IPSubscriber;

class DLL_STATE IPSubscriberListener : public Thread
{
public:
    IPSubscriberListener(const InterfaceHandle& interfaceHandle, bool autoStart = false);
    virtual ~IPSubscriberListener();

    bool StartIPSubscriber(const InterfaceHandle& in);
    bool StopIPSubscriber(const InterfaceHandle& in);
    void StopIPSubscriber(IPSubscriber* sub);
    void Stop(bool waitForTermination = false);

    InterfaceHandle GetInterfaceHandle() const
    {
        MutexLocker lock(&clientMutex_);
        return interfaceHandle_;
    }

    virtual void RMInvocation(int methodId, RmiBase* rmiBase);

protected:
    virtual void run();

private:
    void cleanUp();
    void initIPSubscriberListener();

    IPSubscriber* findIPSubscriber(const InterfaceHandle& handle);

private:
    inline bool isRunning() { return runThread_; }

protected:
    bool runThread_;
    InterfaceHandle interfaceHandle_;
    mutable Mutex clientMutex_;

    typedef std::set<IPSubscriber*> SetIPSubscriber;
    SetIPSubscriber                 setIPSubscriber_;
};

}

