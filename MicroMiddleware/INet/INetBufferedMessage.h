#ifndef MicroMiddleware_INetBufferedMessage_h_Included
#define MicroMiddleware_INetBufferedMessage_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/INet/INetInterfaces.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class RmiObjectBase;

/* ------------------------------------------------------------------
TODO:
    - maintain list of UDP buffered SerializeWriters
    - Only one UDP buffered SerializeReader
    - Entry point for INetOverlayPublisher:
            - PublishTopic(rpc, obj, listOfSubscribers);
    - Entry point for INetOverlaySubscriber:
            - run(): delivers full msg to INetConnection, which is waiting in BeginUnmarshal()
            - INetConnection parses msg and notifies correct INetOverlaySubscriber
-------------------------------------------------------------------*/
class DLL_STATE INetBufferedMessage : public INetMessageParser
                                    , public BaseLib::Thread
{
public:
    typedef std::pair<RpcIdentifier, SerializeReader::Ptr> 	PairRpcMessage;
    typedef std::shared_ptr<PairRpcMessage> 		PairRpcMessagePtr;
    typedef BaseLib::PCList<PairRpcMessagePtr>			PCListMessages;

    typedef std::list<NetworkLib::NetAddress>			ListNetAddress;

public:
    INetBufferedMessage();
    virtual ~INetBufferedMessage();

    virtual void InitSocketObjects(AbstractSocket::Ptr abstractSocket);

public:
    int socketDescriptor() const;
    virtual bool IsConnected() const;
    virtual bool IsBound() const;
    virtual bool IsBufferedRead() const;
    virtual bool IsBufferedWrite() const;

public:
    virtual bool ExecuteRpc(RpcIdentifier*, RmiObjectBase *obj = NULL);
    virtual bool ReceiveRpcObject(RmiObjectBase*, SerializeReader::Ptr&);

    virtual bool BeginMarshal(RpcIdentifier *, SerializeWriter::Ptr &);
    virtual bool EndMarshal(SerializeWriter::Ptr &);
    virtual bool EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses);

    virtual void EndUnmarshal(SerializeReader::Ptr &);

protected:
    virtual bool BeginUnmarshal(RpcIdentifier*, SerializeReader::Ptr &);

    virtual void Close();
    virtual bool DoKeepAlive();

private:
    bool newSerializeWriter(SerializeWriter::Ptr &writer);

private:
    virtual void run();

private:
    SocketStream::Ptr			socketStream_;
    BufferedSocketReader::Ptr	bufferedSocketReader_;
    BufferedSocketWriter::Ptr	bufferedSocketWriter_;
    AbstractSocket::Ptr 		abstractSocket_;
    InterfaceHandle 			handle_;

    PCListMessages				queueIncomingRpcMessages_;

private:
    ElapsedTimer 	aliveTimer_;

private:
    mutable Mutex	mutex_;
    bool 			runThread_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetBufferedMessage_h_Included




