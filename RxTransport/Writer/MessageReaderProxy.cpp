#include"MessageReaderProxy.h"
#include"MessageReaderProxyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

MessageReaderProxy::MessageReaderProxy(const ReaderId& readerId, MessageReaderPolicy policy)
    : Templates::ContextObjectShared<MessageReaderPolicy, Templates::NullData, RequestedStatus>
    (
        new MessageReaderPolicy(policy), new Templates::NullData(), new RequestedStatus(Duration::FromMinutes(60))
    )
    , Templates::Key1<ReaderId>(readerId)
{
    // TODO: Otherwise slow start!
    this->status()->Next(100);
}

MessageReaderProxy::~MessageReaderProxy()
{ }

MapChangeForReader MessageReaderProxy::Requested(const SequenceNumberSet& reqSeqNumSet)
{
    return MessageReaderProxyAlgorithms::UpdateWithStatus(this->self(), reqSeqNumSet, ChangeForReaderStatusKind::REQUESTED);
}

MapChangeForReader MessageReaderProxy::Acked(const SequenceNumber& committedSeqNum)
{
    return MessageReaderProxyAlgorithms::UpdateWithStatusUntil(this->self(), committedSeqNum, ChangeForReaderStatusKind::ACKNOWLEDGED);
}

MapChangeForReader MessageReaderProxy::Select(std::set<ChangeForReaderStatusKind> kinds) const
{
    return MessageReaderProxyAlgorithms::SelectNWithStatus(this->self(), std::numeric_limits<int>::max(), kinds);
}

MapChangeForReader MessageReaderProxy::SelectN(int n, ChangeForReaderStatusKind kind) const
{
    return MessageReaderProxyAlgorithms::SelectNWithStatus(this->self(), n, {kind});
}

bool MessageReaderProxy::IsInStatus(const SequenceNumber& number, ChangeForReaderStatusKind kind) const
{
    return MessageReaderProxyAlgorithms::IsInStatus(this->self(), number, kind);
}

bool MessageReaderProxy::IsInStatus(ChangeForReaderStatusKind kind) const
{
    return MessageReaderProxyAlgorithms::IsAnyInStatus(this->self(), kind);
}

void MessageReaderProxy::SetRequestedNumber(const Count& number)
{
    this->status()->Reset(number.Number());
}

Count MessageReaderProxy::RequestedNumber() const
{
    return this->status()->TotalSum();
}

void MessageReaderProxy::Underway(int number)
{
    this->status()->Next(number * -1);
}

std::string MessageReaderProxy::toString() const
{
    std::stringstream ostr;
    ostr << "MessageReaderProxy(" << this->Id() << ", " << this->self().Size() << "). Changes: (";

    this->ForEachEntry(
        [&ostr](SequenceNumber number, MessageReaderProxyStatusPtr status)
        {
            ostr << "(" << number << ": " << (char) status->CurrentState() << ") ";
        }
    );

    ostr << ")";

    return ostr.str();
}

}}
