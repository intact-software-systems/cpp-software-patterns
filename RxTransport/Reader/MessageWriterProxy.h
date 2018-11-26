#pragma once

#include <RxTransport/Writer/MessageWriterPolicy.h>
#include <RxTransport/CommonDefines.h>

#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * Proxy is stored in MessageReader
 *
 * - Fields include:
 * - MessageWriterPolicy
 * - Flow status,
 *      - requested/demanded messages (accumulated (plus-minus)),
 *      - in-flight,
 *      - acked/nacked,
 *      - other statistics?
 *
 * One queue for each reader? One queue for each group of readers?
 *  - Include FlowController<T>
 */
class DLL_STATE MessageWriterProxy
    : public Collection::details::StdMapCollectionType<SequenceNumber, MessageWriterProxyStatusPtr>
      , public Status::ActivationStatus
      , protected Templates::ContextObjectShared<MessageWriterPolicy, Templates::NullData, Status::FlowStatus>
      , public Templates::Key1<WriterId>
{
public:
    MessageWriterProxy(const WriterId& writerId, MessageWriterPolicy policy);
    virtual ~MessageWriterProxy();

    SequenceNumber MaxAvailable() const;

    SequenceNumberSet Irrelevant() const;
    SequenceNumberSet Missing() const;
    SequenceNumberSet Lost() const;
    SequenceNumberSet Received() const;

    bool Filtered(const SequenceNumber& sequenceNumber);
    void Missing(const SequenceNumber& lastAvailableSeqNum);
    void Lost(const SequenceNumber& firstAvailableSeqNum);
    bool Received(const SequenceNumber& sequenceNumber);

    bool Update(const SequenceNumberSet& numbers, ChangeFromWriterStatusKind kind);

    bool IsMissing() const;

    // ---------------------------------------------
    // Print proxy data
    // ---------------------------------------------

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const MessageWriterProxy& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageWriterProxy>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
