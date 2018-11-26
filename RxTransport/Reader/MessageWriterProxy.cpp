#include "MessageWriterProxy.h"
#include "MessageWriterProxyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

MessageWriterProxy::MessageWriterProxy(const WriterId& writerId, MessageWriterPolicy policy)
    : Templates::ContextObjectShared<MessageWriterPolicy, Templates::NullData, Status::FlowStatus>
    (
        new MessageWriterPolicy(policy), new Templates::NullData(), new Status::FlowStatus()
    )
    , Templates::Key1<WriterId>(writerId)
{ }

MessageWriterProxy::~MessageWriterProxy()
{ }

SequenceNumber MessageWriterProxy::MaxAvailable() const
{
    return MessageWriterProxyAlgorithms::MaxAvailable(this->self());
}

SequenceNumberSet MessageWriterProxy::Irrelevant() const
{
    return MessageWriterProxyAlgorithms::FindWithStatus(this->self(), ChangeFromWriterStatusKind::FILTERED);
}

SequenceNumberSet MessageWriterProxy::Missing() const
{
    return MessageWriterProxyAlgorithms::FindWithStatus(this->self(), ChangeFromWriterStatusKind::MISSING);
}

SequenceNumberSet MessageWriterProxy::Lost() const
{
    return MessageWriterProxyAlgorithms::FindWithStatus(this->self(), ChangeFromWriterStatusKind::LOST);
}

SequenceNumberSet MessageWriterProxy::Received() const
{
    return MessageWriterProxyAlgorithms::FindWithStatus(this->self(), ChangeFromWriterStatusKind::RECEIVED);
}

bool MessageWriterProxy::Filtered(const SequenceNumber& sequenceNumber)
{
    return MessageWriterProxyAlgorithms::Filtered(this->self(), sequenceNumber);
}

void MessageWriterProxy::Missing(const SequenceNumber& lastAvailableSeqNum)
{
    return MessageWriterProxyAlgorithms::Missing(this->self(), lastAvailableSeqNum);
}

void MessageWriterProxy::Lost(const SequenceNumber& firstAvailableSeqNum)
{
    return MessageWriterProxyAlgorithms::Lost(this->self(), firstAvailableSeqNum);
}

bool MessageWriterProxy::Received(const SequenceNumber& sequenceNumber)
{
    return MessageWriterProxyAlgorithms::Received(this->self(), sequenceNumber);
}

bool MessageWriterProxy::IsMissing() const
{
    return MessageWriterProxyAlgorithms::IsWithStatus(this->self(), ChangeFromWriterStatusKind::MISSING);
}

bool MessageWriterProxy::Update(const SequenceNumberSet& numbers, ChangeFromWriterStatusKind kind)
{
    return MessageWriterProxyAlgorithms::UpdateProxy(this->self(), numbers, kind);
}

std::string MessageWriterProxy::toString() const
{
    std::stringstream ostr;
    ostr << "MessageWriterProxy(" << this->Id() << ", " << this->self().Size() << "). Changes: (";

    this->ForEachEntry(
        [&ostr](SequenceNumber number, MessageWriterProxyStatusPtr status)
        {
            ostr << "(" << number << ": " << (char) status->CurrentState() << ") ";
        }
    );

    ostr << ")";

    return ostr.str();
}

}}
