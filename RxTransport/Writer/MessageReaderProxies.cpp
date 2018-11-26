#include "MessageReaderProxies.h"
#include "MessageReaderProxy.h"

namespace BaseLib { namespace Concurrent {

void MessageReaderProxies::Activate()
{
    this->ForEach([](MessageReaderProxyPtr item) { item->Activate(); });
}

void MessageReaderProxies::Deactivate()
{
    this->ForEach([](MessageReaderProxyPtr item) { item->Deactivate(); });
}

void MessageReaderProxies::AddSequenceNumber(SequenceNumber sequenceNumber, ChangeForReaderStatusKind kind)
{
    this->ForEach(
        [&sequenceNumber, &kind](MessageReaderProxyPtr reader)
        {
            ASSERT(!reader->Contains(sequenceNumber));

            MessageReaderProxyStatusPtr status = std::make_shared<MessageReaderProxyStatus>(Duration::FromMinutes(10), 2);
            status->NextState(kind, sequenceNumber);

            bool isAdded = reader->Add(sequenceNumber, status);
            ASSERT(isAdded);
        }
    );

    ASSERT(!this->empty());
}

MapChangeForReader MessageReaderProxies::Next(ReaderId readerId, AckNackPtr ackNack)
{
    return this->FindAndApply<MapChangeForReader>(
        readerId,
        [ackNack](MessageReaderProxyPtr reader)
        {
            MapChangeForReader acked     = reader->Acked(ackNack->AckUntil());
            MapChangeForReader requested = reader->Requested(ackNack->Nacks());

            MapChangeForReader merged;
            merged.insert(acked.begin(), acked.end());
            merged.insert(requested.begin(), requested.end());

            return merged;
        }
    );
}

bool MessageReaderProxies::Next(ReaderId readerId, RequestPtr request)
{
    bool next = this->FindAndApply<bool>(
        readerId,
        [request](MessageReaderProxyPtr reader)
        {
            reader->SetRequestedNumber(request->number());
            return true;
        }
    );

    ASSERT(next);
    return next;
}

std::string MessageReaderProxies::toString() const
{
    std::stringstream ostr;
    ostr << "MessageReaderProxies:" << std::endl;

    for(auto item : this->collection())
    {
        ostr << "\t" << item.second->toString() << std::endl;
    }

    return ostr.str();
}

}}

