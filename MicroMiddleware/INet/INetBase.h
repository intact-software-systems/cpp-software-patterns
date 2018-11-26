#ifndef MicroMiddleware_INetBase_h_Included
#define MicroMiddleware_INetBase_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/INet/INetInterfaces.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class RmiObjectBase;

class DLL_STATE INetBase : public INetMessageParser
{
public:
    typedef std::list<NetworkLib::NetAddress>			ListNetAddress;

public:
    INetBase(SocketStream *socketStream, const InterfaceHandle &handle);
    INetBase(const InterfaceHandle &handle);
    virtual ~INetBase();

    virtual void InitSocketObjects(AbstractSocket::Ptr abstractSocket);

public:
    int socketDescriptor() const;
    virtual bool IsConnected() const;
    virtual bool IsBound() const;

    virtual bool IsBufferedRead() const;
    virtual bool IsBufferedWrite() const;

    virtual bool DoKeepAlive();

public:
    virtual bool ExecuteRpc(RpcIdentifier *, RmiObjectBase *obj = NULL);
    virtual bool ReceiveRpcObject(RmiObjectBase *, SerializeReader::Ptr&);

    virtual bool BeginMarshal(RpcIdentifier *, SerializeWriter::Ptr &);
    virtual bool EndMarshal(SerializeWriter::Ptr &);
    virtual bool EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses);

    virtual void EndUnmarshal(SerializeReader::Ptr &);

public:
    inline void RpcReadingFinishedLock()	 	{ rpcReadingFinishedMutex_.lock(); }
    inline void RpcReadingFinishedUnlock()	 	{ rpcReadingFinishedMutex_.unlock(); }
    bool WaitReadFinished(int ms = 2000);

protected:
    virtual bool BeginUnmarshal(RpcIdentifier *, SerializeReader::Ptr&);

    virtual void ReadLock();
    virtual void ReadUnlock();

    virtual void Close();

protected:
    ElapsedTimer				aliveTimer_;

private:
    void cleanUp();
    void NotifyReadWait();

private:
    SocketStream::Ptr		socketStream_;
    SerializeWriter::Ptr	socketWriter_;
    SerializeReader::Ptr 	socketReader_;
    SerializeWriter::Ptr	headerWriter_;
    AbstractSocket::Ptr 	abstractSocket_;
    InterfaceHandle 		handle_;

    // Between BeginMarshal() EndMarshal() this is the current rpc
    // for buffered output
    RpcIdentifier	currentRpc_;

private:
    mutable Mutex	rpcReadingFinishedMutex_;
    WaitCondition	waitRpcReadingFinished_;
    bool			readingFinished_;

private:
    short			myHostEndian;
    mutable Mutex	rmiMutex_;
    mutable Mutex   readMutex_;
    mutable Mutex	writeMutex_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_INetBase_h_Included




