#pragma once

#include <RxTransport/Reader/MessageReaderPolicy.h>
#include <RxTransport/CommonDefines.h>
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * Proxy is stored in MessageWriter
 *
 * - Fields include:
 * - MessageReaderPolicy
 * - Flow status,
 *      - requested/demanded messages (accumulated (plus-minus)),
 *      - in-flight,
 *      - acked/nacked,
 *      - other statistics?
 *
 * One queue for each reader? One queue for each group of readers?
 *  - Include FlowController<T>
 */
class DLL_STATE MessageReaderProxy
    : public Collection::details::StdMapCollectionType<SequenceNumber, MessageReaderProxyStatusPtr>
      , public Status::ActivationStatus
      , protected Templates::ContextObjectShared<MessageReaderPolicy, Templates::NullData, RequestedStatus>
      , public Templates::Key1<ReaderId>
      , public Templates::LockableType<MessageReaderProxy>
{
public:
    MessageReaderProxy(const ReaderId& readerId, MessageReaderPolicy policy);
    virtual ~MessageReaderProxy();

    MapChangeForReader Requested(const SequenceNumberSet& numbers);
    MapChangeForReader Acked(const SequenceNumber& number);

    MapChangeForReader Select(std::set<ChangeForReaderStatusKind> kinds) const;
    MapChangeForReader SelectN(int n, ChangeForReaderStatusKind kind) const;

    bool IsInStatus(const SequenceNumber& ackedSeq, ChangeForReaderStatusKind kind) const;
    bool IsInStatus(ChangeForReaderStatusKind kind) const;

    void  SetRequestedNumber(const Count& number);
    Count RequestedNumber() const;
    void  Underway(int number);

    // ---------------------------------------------
    // Print proxy data
    // ---------------------------------------------

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const MessageReaderProxy& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageReaderProxy>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
