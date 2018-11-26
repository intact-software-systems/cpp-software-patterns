#include"MessageWriterProxies.h"
#include"MessageWriterProxy.h"

namespace BaseLib { namespace Concurrent {

void MessageWriterProxies::Activate()
{
    this->ForEach([](MessageWriterProxyPtr item) { item->Activate(); });
}

void MessageWriterProxies::Deactivate()
{
    this->ForEach([](MessageWriterProxyPtr item) { item->Deactivate(); });
}

bool MessageWriterProxies::Update(const WriterId& writerId, const SequenceNumber& sequenceNumber, ChangeFromWriterStatusKind kind)
{
    return this->FindAndApply<bool>(
        writerId,
        [&sequenceNumber, &kind](MessageWriterProxyPtr writer)
        {
            return writer->Update(sequenceNumber, kind);
        }
    );
}

bool MessageWriterProxies::Update(const WriterId& writerId, const SequenceNumberSet& sequenceNumbers, ChangeFromWriterStatusKind kind)
{
    return this->FindAndApply<bool>(
        writerId,
        [&sequenceNumbers, &kind](MessageWriterProxyPtr writer)
        {
            return writer->Update(sequenceNumbers, kind);
        }
    );
}

void MessageWriterProxies::AddForAll(const SequenceNumber& sequenceNumber, ChangeFromWriterStatusKind kind)
{
    this->ForEach(
        [&sequenceNumber, &kind](MessageWriterProxyPtr writer)
        {
            writer->Update(sequenceNumber, kind);
        }
    );
}

bool MessageWriterProxies::IsMissingFor(const WriterId& writerId) const
{
    return this->FindAndApply<bool>(writerId, [](MessageWriterProxyPtr proxy) { return proxy->IsMissing(); });
}

std::string MessageWriterProxies::toString() const
{
    std::stringstream ostr;
    ostr << "MessageWriterProxies:" << std::endl;
    for(auto item : this->collection())
    {
        ostr << "\t" << item.second << std::endl;
    }
    return ostr.str();
}

}}

