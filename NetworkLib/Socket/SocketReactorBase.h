#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Socket/SocketStream.h"

#include"NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE SocketReactorBase
{
public:
    typedef std::string SocketPoolName;

    typedef RingBufferQueueMap<char, FlowId> MapFlowBuffer;

    typedef std::map<FlowId, RawSocketReader::Ptr> MapSocketConsumer;

    typedef std::pair<FlowId, RawSocketReader::Ptr> PairSocketConsumer;

    typedef std::map<FlowId, RawSocketWriter::Ptr> MapSocketProducer;

    typedef std::pair<FlowId, RawSocketWriter::Ptr> PairSocketProducer;

public:
    CLASS_TRAITS(SocketReactorBase)

    virtual bool AddSocket(AbstractSocket::Ptr) = 0;

    virtual bool RemoveSocket(AbstractSocket::Ptr) = 0;

    virtual void ClearAll() = 0;
};

}
