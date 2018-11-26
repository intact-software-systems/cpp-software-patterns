#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/SocketObserverBase.h"
#include"NetworkLib/Socket/SocketSelectorPool.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE SocketAcceptorBase
{
public:
    virtual SetSocket AcceptSockets(int64 msecs) = 0;
    virtual bool AddServerSocket(std::shared_ptr<TcpServer> socket) = 0;
    virtual bool RemoveServerSocket(std::shared_ptr<TcpServer> socket) = 0;
    virtual void ClearSockets() = 0;
    virtual void Close() = 0;
    virtual bool IsValid() const = 0;
    virtual unsigned int NumSockets() const = 0;
    virtual bool IsEmpty() const = 0;
};

/**
 * @brief The SocketAcceptorSelector take as input one or several TCP server sockets
 *
 * TODO: Should probably be Observable<SocketDisconnectedObserver>
 *
 * Write accepted sockets to cache
 */
class DLL_STATE SocketAcceptorSelector : public SocketAcceptorBase
                                         , public SocketDisconnectedObserver
{
public:
    SocketAcceptorSelector()
        : selector_()
        , selectorConnector_(this, &selector_)
    {}
    virtual ~SocketAcceptorSelector()
    {}

    virtual SetSocket AcceptSockets(int64 msecs = LONG_MAX)
    {
        SetSocket spawnedSockets;

        for(std::shared_ptr<AbstractSocket> socket : selector_.Select(msecs))
        {
            std::shared_ptr<TcpServer> tcpServer = std::dynamic_pointer_cast<TcpServer>(socket);
            ASSERT(tcpServer);

            TcpSocket* tcpSocket = tcpServer->acceptTcpSocket();

            if(tcpSocket)
            {
                spawnedSockets.insert(std::shared_ptr<AbstractSocket>(tcpSocket));
                signalAccepted(socket);
            }
        }

        return spawnedSockets;
    }

    virtual bool AddServerSocket(std::shared_ptr<TcpServer> socket)
    {
        return selector_.AddSocket(socket);
    }

    virtual bool RemoveServerSocket(std::shared_ptr<TcpServer> socket)
    {
        return selector_.RemoveSocket(socket);
    }

    virtual void ClearSockets()
    {
        selector_.ClearSockets();
    }

    virtual void Close()
    {
        selector_.Close();
    }

    virtual bool IsValid() const
    {
        return selector_.IsValid();
    }

    virtual unsigned int NumSockets() const
    {
        return selector_.NumSockets();
    }

    virtual bool IsEmpty() const
    {
        return selector_.IsEmpty();
    }

    /**
     * @brief OnSocketDisconnected is called back from SocketSelector
     */
    virtual void OnSocketDisconnected(std::shared_ptr<AbstractSocket> socket)
    {
        signalDisconnected(socket);
    }

private:
    virtual void signalAccepted(std::shared_ptr<AbstractSocket>)
    {
        // empty no-op
    }

    virtual void signalDisconnected(std::shared_ptr<AbstractSocket>)
    {
        // empty no-op
    }

private:
    SocketSelectorObservable selector_;

    ObserverConnector<SocketAcceptorSelector, SocketSelectorObservable> selectorConnector_;
};

/**
 * @brief The SocketAcceptorObservable class
 */
class DLL_STATE SocketAcceptorObservable : public SocketAcceptorSelector
                                           , public Observable<SocketAcceptedObserver>
                                           , public Observable<SocketDisconnectedObserver>
{
public:
    SocketAcceptorObservable()
        : socketAcceptorSignaller_(new Signaller1<void, std::shared_ptr<AbstractSocket> >())
        , socketDisconnectedSignaller_(new Signaller1<void, std::shared_ptr<AbstractSocket> >())
    {}

    virtual ~SocketAcceptorObservable()
    {}

    virtual SlotHolder::Ptr Connect(SocketAcceptedObserver* observer)
    {
        return socketAcceptorSignaller_->Connect<SocketAcceptedObserver>(observer, &SocketAcceptedObserver::OnSocketAccepted);
    }

    virtual bool Disconnect(SocketAcceptedObserver* observer)
    {
        return socketAcceptorSignaller_->Disconnect<SocketAcceptedObserver>(observer);
    }

public:
    virtual SlotHolder::Ptr Connect(SocketDisconnectedObserver* observer)
    {
        return socketDisconnectedSignaller_->Connect<SocketDisconnectedObserver>(observer, &SocketDisconnectedObserver::OnSocketDisconnected);
    }

    virtual bool Disconnect(SocketDisconnectedObserver* observer)
    {
        return socketDisconnectedSignaller_->Disconnect<SocketDisconnectedObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        socketAcceptorSignaller_->DisconnectAll();
        socketDisconnectedSignaller_->DisconnectAll();
    }

private:
    virtual void signalAccepted(std::shared_ptr<AbstractSocket> socket)
    {
        socketAcceptorSignaller_->Signal<SocketAcceptedObserver>(socket, &SocketAcceptedObserver::OnSocketAccepted);
    }

    virtual void signalDisconnected(std::shared_ptr<AbstractSocket> socket)
    {
        socketDisconnectedSignaller_->Signal(socket);
    }

private:
    Signaller1<void, std::shared_ptr<AbstractSocket> >::Ptr socketAcceptorSignaller_;
    Signaller1<void, std::shared_ptr<AbstractSocket> >::Ptr socketDisconnectedSignaller_;
};

/**
 * @brief The SocketAcceptorRunnable class
 *
 * TODO:
 *  - support isClosed() to cancel thread!
 *  - Runnables can be given a handle to be able to remove itself from the threadPool!
 */
template <typename ACCEPTOR>
class SocketAcceptorRunnable : public Runnable
                               , public SocketAcceptorBase
                               , public Observable<SocketAcceptedObserver>
                               , public Observable<SocketDisconnectedObserver>
{
public:
    SocketAcceptorRunnable(int64 msecs = LONG_MAX)
        : waitTimeMsecs_(msecs)
    {}

    virtual ~SocketAcceptorRunnable()
    {}

    virtual void run()
    {
        SetSocket socketsAccepted = AcceptSockets(waitTimeMsecs_);

    }

    virtual SetSocket AcceptSockets(int64 msecs)
    {
        return acceptor_.AcceptSockets(msecs);
    }

    virtual bool AddServerSocket(std::shared_ptr<TcpServer> socket)
    {
        return acceptor_.AddServerSocket(socket);
    }

    virtual bool RemoveServerSocket(std::shared_ptr<TcpServer> socket)
    {
        return acceptor_.RemoveServerSocket(socket);
    }

    virtual void ClearSockets()
    {
        acceptor_.ClearSockets();
    }

    virtual void Close()
    {
        acceptor_.Close();
    }

    virtual bool IsValid() const
    {
        return acceptor_.IsValid();
    }

    virtual unsigned int NumSockets() const
    {
        return acceptor_.NumSockets();
    }

    virtual bool IsEmpty() const
    {
        return acceptor_.IsEmpty();
    }

    virtual SlotHolder::Ptr Connect(SocketAcceptedObserver* observer)
    {
        return acceptor_.Connect(observer);
    }

    virtual bool Disconnect(SocketAcceptedObserver* observer)
    {
        return acceptor_.Disconnect(observer);
    }

    virtual SlotHolder::Ptr Connect(SocketDisconnectedObserver* observer)
    {
        return acceptor_.Connect(observer);
    }

    virtual bool Disconnect(SocketDisconnectedObserver* observer)
    {
        return acceptor_.Disconnect(observer);
    }

    virtual void DisconnectAll()
    {
        acceptor_.DisconnectAll();
    }

private:
    ACCEPTOR acceptor_;
    int64    waitTimeMsecs_;
};

}
