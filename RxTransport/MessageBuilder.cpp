#include "RxTransport/MessageBuilder.h"

namespace BaseLib { namespace Concurrent {

MessageBuilder::MessageBuilder()
    : from_()
    , to_()
    , policy_(MessagePolicy::Default())
    , messages_()
    , channelAccessorKind_(ChannelAccessorKind::NO)
{ }

MessageBuilder::~MessageBuilder()
{ }

MessageBuilder& MessageBuilder::Accessor(ChannelAccessorKind kind)
{
    channelAccessorKind_ = kind;
    return *this;
}

MessageBuilder& MessageBuilder::Handle(ChannelHandle channelHandle)
{
    handle_ = channelHandle;
    return *this;
}

MessageBuilder& MessageBuilder::FromTo(GUUID from, GUUID to)
{
    from_ = from;
    to_   = to;
    return *this;
}

MessageBuilder& MessageBuilder::With(MessagePolicy policy)
{
    policy_ = policy;
    return *this;
}

MessageBuilder& MessageBuilder::Add(std::shared_ptr<SubMessage> msg)
{
    messages_.push_back(msg);
    return *this;
}

bool MessageBuilder::IsContent() const
{
    return !messages_.empty();
}

int MessageBuilder::NumberOf(MessageKind kind) const
{
    int count = 0;

    for(const auto& message : messages_)
    {
        if(message->Kind() == kind)
        {
            ++count;
        }
    }
    return count;
}

size_t MessageBuilder::TotalNumberOf() const
{
    return messages_.size();
}

}}
