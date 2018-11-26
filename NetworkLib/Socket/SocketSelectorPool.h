#ifndef NetworkLib_SocketSelectorPool_h_IsIncluded
#define NetworkLib_SocketSelectorPool_h_IsIncluded

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Serialize/InputArray.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"
#include"NetworkLib/Socket/TcpServer.h"
#include"NetworkLib/Socket/TcpSocket.h"
#include"NetworkLib/Socket/Pipe.h"
#include"NetworkLib/Socket/SocketObserverBase.h"

#include"NetworkLib/Export.h"

namespace NetworkLib
{

class DLL_STATE SocketSelectorBase : public Templates::EventSelectorMethod<Collection::ISet<AbstractSocket::Ptr>, AbstractSocket::Ptr>
{
public:
    virtual bool AddSocket(AbstractSocket::Ptr socket) = 0;
    virtual bool RemoveSocket(AbstractSocket::Ptr socket) = 0;
    virtual void ClearSockets() = 0;
    virtual void Close() = 0;
    virtual bool IsValid() const = 0;
    virtual unsigned int NumSockets() const = 0;
    virtual bool IsEmpty() const = 0;
};

/**
 * TODO:
 * - NumBytesAvailablePolicy
 * - Call-back to listener if a socket is invalidated
 * - Use a Pipe to wake up any thread waiting in Select, i.e., write some garbage to it then the select will return
 *      - Wake-up when adding or removing sockets
 * - Not thread safe!
 * - Move to SocketSelector.h/cpp
 *
 */
class DLL_STATE SocketSelector : public SocketSelectorBase
{
public:
    typedef fd_set  FileDescriptorSet;

public:
    SocketSelector()
    { }
    virtual ~SocketSelector()
    { }

    /**
     * TODO:
     * - Add termination case...
     * - More efficient initialization of fdset using FD_CLR(socket, &fdset);
     */
    virtual SetSocket Select(int64 msecs = LONG_MAX)
    {
        FileDescriptorSet fdset;

        while(isValid())
        {
            FD_ZERO(&fdset);
            int maxFileDescriptor = 0;

            initializeFileDescriptorSet(fdset, maxFileDescriptor, sockets_);

            int numDescriptorsReady = waitForData(fdset, maxFileDescriptor, msecs);

            // Timeout?
            if(numDescriptorsReady == 0)
            {
                return SetSocket();
            }
            // Woken up?
            else if(isNotified(fdset) && numDescriptorsReady == 1)
            {
                continue;
            }
            else if(numDescriptorsReady == -1)
            {
                IWARNING() << "Select returned -1";
            }

            break;
        }

        return isValid()
               ? getSocketsReadyForRead(fdset)
               : SetSocket();
    }

    virtual bool AddSocket(AbstractSocket::Ptr socket)
    {
        if(!socket->isValid())
        {
            IWARNING() << "Attempt to add invalid socket!";
            return false;
        }

        std::pair<SetSocket::iterator, bool> inserted = this->sockets_.insert(socket);

        this->notifySelect();

        return inserted.second;
    }

    virtual bool RemoveSocket(AbstractSocket::Ptr socket)
    {
        SetSocket::iterator it = sockets_.find(socket);
        if(it == sockets_.end())
        {
            IWARNING() << "Could not locate " << socket;
            return false;
        }

        sockets_.erase(it);

        // -- debug --
        ASSERT(sockets_.find(socket) == sockets_.end());
        // -- debug --

        this->notifySelect();

        return true;
    }

    virtual void ClearSockets()
    {
        sockets_.clear();

        this->notifySelect();
    }

    virtual void Close()
    {
        notificationPipe_.Close();
    }

    virtual bool IsValid() const
    {
        return isValid();
    }

    virtual unsigned int NumSockets() const
    {
        return sockets_.size();
    }

    virtual bool IsEmpty() const
    {
        return sockets_.empty();
    }

protected:
    SetSocket getSocketsReadyForRead(FileDescriptorSet &fdset)
    {
        SetSocket readySockets;
        SetSocket invalidSockets;

        for(SetSocket::iterator it = sockets_.begin(), it_end = sockets_.end(); it != it_end; ++it)
        {
            AbstractSocket::Ptr socket = *it;

            // -- debug --
            ASSERT(socket);
            // -- debug --

            int64 bytesAvailable = socket->bytesAvailable();
            if(bytesAvailable <= 0)
            {
                // -- debug --
                ASSERT(!socket->isValid());
                // -- debug --

                invalidSockets.insert(socket);
            }
            else if(FD_ISSET(socket->socketDescriptor(), &fdset))
            {
                readySockets.insert(socket);
            }
        }

        if(!invalidSockets.empty())
        {
            removeInvalidSockets(invalidSockets);
        }

        return readySockets;
    }

    int waitForData(FileDescriptorSet &fdset, int maxFileDescriptor, int64 milliseconds)
    {
        int numReadyDescriptors = 0;

        if(milliseconds == LONG_MAX)
        {
            numReadyDescriptors = ::select(maxFileDescriptor + 1, &fdset, NULL, NULL, NULL);
        }
        else
        {
            struct timeval tv = {0, 0};
            Utility::ClockInitWaitTimevalMsec(tv, milliseconds);

            numReadyDescriptors = ::select(maxFileDescriptor + 1, &fdset, NULL, NULL, &tv);
        }

        if(numReadyDescriptors < 0)
            logSelectError(errno);

        return numReadyDescriptors;
    }

    bool initializeFileDescriptorSet(FileDescriptorSet& fdset, int& maxFileDescriptor, SetSocket& sockets)
    {
        maxFileDescriptor = 0;
        FD_ZERO(&fdset);

        FD_SET(notificationPipe_.ReadDescriptor(), &fdset);

        SetSocket invalidSockets;

        for(SetSocket::iterator it = sockets.begin(), it_end = sockets.end(); it != it_end; ++it)
        {
            AbstractSocket::Ptr socket = *it;

            // -- debug --
            ASSERT(socket);
            // -- debug --

            if(socket->isValid())
            {
                FD_SET(socket->socketDescriptor(), &fdset);

                maxFileDescriptor = std::max<int>(maxFileDescriptor, socket->socketDescriptor());
            }
            else
            {
                invalidSockets.insert(socket);
            }
        }

        if(!invalidSockets.empty())
        {
            removeInvalidSockets(invalidSockets);
        }

        return !sockets.empty();
    }

    /**
     * @brief identifyInvalidSockets
     * @param invalidSockets
     */
    bool identifyInvalidSockets(SetSocket &invalidSockets)
    {
        for(SetSocket::iterator it = sockets_.begin(), it_end = sockets_.end(); it != it_end; ++it)
        {
            AbstractSocket::Ptr socket = *it;

            if(!socket->isValid())
                invalidSockets.insert(socket);
        }

        return !invalidSockets.empty();
    }

    int removeInvalidSockets(SetSocket &invalidSockets)
    {
        for(SetSocket::iterator it = invalidSockets.begin(), it_end = invalidSockets.end(); it != it_end; ++it)
        {
            AbstractSocket::Ptr socket = *it;

            signalDisconnected(socket);

            size_t numErased = sockets_.erase(socket);

            // -- debug --
            ASSERT(numErased == 1);
            IDEBUG() << "Removed socket: " << socket;
            // -- debug --

            //socket->close();
        }

        return invalidSockets.size();
    }

    void notifySelect()
    {
        static std::string msg("C");
        this->notificationPipe_.Write(msg.c_str(), msg.length());
    }

    bool isNotified(FileDescriptorSet &fdset)
    {
        if(FD_ISSET(notificationPipe_.ReadDescriptor(), &fdset))
        {
            int av = notificationPipe_.BytesAvailable();

            // -- debug --
            ASSERT(av > 0);
            // -- debug --

            std::vector<char> garbage(av);

            notificationPipe_.Read(&garbage.front(), av);

            return true;
        }

        return false;
    }

    bool isValid() const
    {
        return notificationPipe_.IsValidRead();
    }

    void logSelectError(int error)
    {
        /**
         [EAGAIN]           The kernel was (perhaps temporarily) unable to allocate the requested number of file descriptors.
         [EBADF]            One of the descriptor sets specified an invalid descriptor.
         [EINTR]            A signal was delivered before the time limit expired and before any of the selected events occurred.
         [EINVAL]           The specified time limit is invalid.  One of its components is negative or too large.
         [EINVAL]           ndfs is greater than FD_SETSIZE and _DARWIN_UNLIMITED_SELECT is not defined.
        */
        switch(error)
        {
        case EAGAIN:
            IWARNING() << "EAGAIN: The kernel was (perhaps temporarily) unable to allocate the requested number of file descriptors.";
            break;
        case EBADF:
            IWARNING() << "EBADF: One of the descriptor sets specified an invalid descriptor.";
            break;
        case EINTR:
            IWARNING() << "EINTR: A signal was delivered before the time limit expired and before any of the selected events occurred.";
            break;
        case EINVAL:
            IWARNING() << "EINVAL: The specified time limit is invalid.  One of its components is negative or too large";
            break;
        default:
            IWARNING() << "Unidentified select error!";
            break;
        }
    }

private:
    virtual void signalDisconnected(AbstractSocket::Ptr)
    {
        // A noop
    }

protected:
    SetSocket sockets_;
    Pipe      notificationPipe_;
};

class DLL_STATE SocketSelectorObservable : public SocketSelector
                                         , public Observable<SocketDisconnectedObserver>
{
public:
    SocketSelectorObservable()
        : SocketSelector()
        , socketDisconnectedSignaller_(new Signaller1<void, AbstractSocket::Ptr>())
    {}
    virtual ~SocketSelectorObservable()
    {}

    virtual SlotHolder::Ptr Connect(SocketDisconnectedObserver *observer)
    {
        return socketDisconnectedSignaller_->Connect<SocketDisconnectedObserver>(observer, &SocketDisconnectedObserver::OnSocketDisconnected);
    }

    virtual bool Disconnect(SocketDisconnectedObserver *observer)
    {
        return socketDisconnectedSignaller_->Disconnect<SocketDisconnectedObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        socketDisconnectedSignaller_->DisconnectAll();
    }

private:
    virtual void signalDisconnected(AbstractSocket::Ptr socket)
    {
        socketDisconnectedSignaller_->Signal(socket);
    }

private:
    Signaller1<void, AbstractSocket::Ptr>::Ptr  socketDisconnectedSignaller_;
};

template <typename SELECTOR>
class SocketSelectorRunnable : public Runnable
                            , public SocketSelectorBase
                            , public Observable<SocketReadyToReadObserver>
                            , public Observable<SocketDisconnectedObserver>
{
public:
    SocketSelectorRunnable(int64 msecs = LONG_MAX)
        : waitTimeMsecs_(msecs)
        , socketSelectorSignaller_( new Signaller1<void, AbstractSocket::Ptr>() )
    {}

    virtual ~SocketSelectorRunnable()
    {}

    virtual void run()
    {
        SetSocket setReadyToRead = Select(waitTimeMsecs_);

        for(SetSocket::iterator it = setReadyToRead.begin(), it_end = setReadyToRead.end(); it != it_end; ++it)
        {
            AbstractSocket::Ptr socket = *it;

            signalObservers(socket);
        }
    }

    SetSocket Select(int64 msecs)
    {
        return selector_.Select(msecs);
    }

    virtual bool AddSocket(AbstractSocket::Ptr socket)
    {
        return selector_.AddSocket(socket);
    }

    virtual bool RemoveSocket(AbstractSocket::Ptr socket)
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

    virtual typename SlotHolder::Ptr Connect(SocketReadyToReadObserver *observer)
    {
        return socketSelectorSignaller_->Connect<SocketReadyToReadObserver>(observer, &SocketReadyToReadObserver::OnSocketReadyToRead);
    }

    virtual bool Disconnect(SocketReadyToReadObserver *observer)
    {
        return socketSelectorSignaller_->Disconnect<SocketReadyToReadObserver>(observer);
    }

    virtual SlotHolder::Ptr Connect(SocketDisconnectedObserver *observer)
    {
        return selector_.Connect(observer);
    }

    virtual bool Disconnect(SocketDisconnectedObserver *observer)
    {
        return selector_.RemoveObserver(observer);
    }

private:
    void signalObservers(AbstractSocket::Ptr socket)
    {
        socketSelectorSignaller_->Signal(socket);
    }

private:
    SELECTOR        selector_;
    int64   waitTimeMsecs_;

private:
    Signaller1<void, AbstractSocket::Ptr>::Ptr  socketSelectorSignaller_;
};

// TODO: MulticastSocketWriter: Writes to many sockets in one write-call.
// TODO: MulticastSocketReader: Reads from many sockets and distributes to de-serializing handlers
// - Read into ring-buffer, back-off if ring-buffer is full, or, use WFQ with data-chunks of size=data-gramsize
// - If UDP, then use data-gram size and WFQ
// - If TCP, then wait until available bytes is at least data-gram size.

/**
 * @brief The SelectSocketReader class
 */
class DLL_STATE SelectSocketReader : public SocketReadyToReadObserver
{
public:
    SelectSocketReader(const InputArray<char>::Ptr &dataStream)
        : dataStream_(dataStream)
    {}
    virtual ~SelectSocketReader()
    {}

    /**
     * @brief OnSocketReadyRead
     *
     * TODO: This is wrong! different sockets will write to same ring-buffer and clutter it up!
     *      - I need a ConcurrentMap<Socket, RingBuffer<char> >
     *      - Is it possible to use WaitSet and GuardConditions, similar to ThreadPool?
     *
     * @param socket
     */
    virtual void OnSocketReadyToRead(AbstractSocket::Ptr socket)
    {
        typedef std::vector<char>   CharVector;

        //FlowId flowId         = socket->socketDescriptor();
        int    bytesAvailable = socket->bytesAvailable();

        CharVector dataChunk(bytesAvailable);

        int bytesRead = socket->Read(&dataChunk[0], bytesAvailable);

        // -- debug --
        if(bytesRead != bytesAvailable)
            IWARNING() << "Read " << bytesRead << " expected " << bytesAvailable;
        // -- debug --

        if(bytesRead > 0)
        {
            // Fill up Ring-buffer, which is shared by deserializers - BufferReaders
            // Or, fill up ConcurrentWFQueue including flowId -> Not a great idea, rather use WFQ<DataMessage> of a given size to RTPS?
            dataStream_->Write(dataChunk);
        }
    }

private:
    InputArray<char>::Ptr dataStream_;
};

}

#endif
