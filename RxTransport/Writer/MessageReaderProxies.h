#pragma once

#include <RxTransport/CommonDefines.h>
#include <RxTransport/Message.h>
#include <RxTransport/Export.h>

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageReaderProxies
    : public Collection::details::StdMapCollectionType<ReaderId, MessageReaderProxyPtr>
{
public:
    virtual ~MessageReaderProxies()
    { }

    void Activate();
    void Deactivate();

    void AddSequenceNumber(SequenceNumber sequenceNumber, ChangeForReaderStatusKind kind);
    MapChangeForReader Next(ReaderId readerId, AckNackPtr ackNack);
    bool Next(ReaderId readerId, RequestPtr request);

    // ---------------------------------------------
    // Print proxy data
    // ---------------------------------------------

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const MessageReaderProxies& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageReaderProxies>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
