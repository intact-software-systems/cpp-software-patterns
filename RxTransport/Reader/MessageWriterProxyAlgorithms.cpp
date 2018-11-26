#include "MessageWriterProxyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

SequenceNumberSet MessageWriterProxyAlgorithms::FindWithStatus(const MapChangeForWriter& data, ChangeFromWriterStatusKind kind)
{
    SequenceNumberSet seqSet;

    MutexTypeLocker<MapChangeForWriter> lock(&data);

    for(const auto& it : data)
    {
        if(it.second->CurrentState() == kind)
        {
            seqSet.insert(it.first);
        }
    }

    return seqSet;
}

bool MessageWriterProxyAlgorithms::IsWithStatus(const MapChangeForWriter& data, ChangeFromWriterStatusKind kind)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    for(const auto& it : data)
    {
        if(it.second->CurrentState() == kind)
        {
            return true;
        }
    }

    return false;
}

SequenceNumber MessageWriterProxyAlgorithms::MaxAvailable(const MapChangeForWriter& data)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    for(auto it = data.crbegin(), it_end = data.crend(); it != it_end; ++it)
    {
        if(it->second->CurrentState() == ChangeFromWriterStatusKind::RECEIVED || it->second->CurrentState() == ChangeFromWriterStatusKind::LOST)
        {
            return it->first;
        }
    }

    return SequenceNumber::NIL();
}

bool MessageWriterProxyAlgorithms::UpdateProxy(MapChangeForWriter& data, const SequenceNumberSet& sequenceNumbers, const ChangeFromWriterStatusKind& kind)
{
    for(const auto& number : sequenceNumbers)
    {
        UpdateProxy(data, number, kind);
    }
    return true;
}

bool MessageWriterProxyAlgorithms::UpdateProxy(MapChangeForWriter& data, const SequenceNumber& sequenceNumber, const ChangeFromWriterStatusKind& kind)
{
    {
        MutexTypeLocker<MapChangeForWriter> lock(&data);

        auto it = data.find(sequenceNumber);

        if(it == data.cend())
        {
            MessageWriterProxyStatusPtr status = std::make_shared<MessageWriterProxyStatus>(Duration::FromMinutes(10), 2);
            data.put(sequenceNumber, status);
        }
    }

    switch(kind)
    {
        case ChangeFromWriterStatusKind::LOST:
        {
            MessageWriterProxyAlgorithms::Lost(data, sequenceNumber);
            break;
        }
        case ChangeFromWriterStatusKind::MISSING:
        {
            MessageWriterProxyAlgorithms::Missing(data, sequenceNumber);
            break;
        }
        case ChangeFromWriterStatusKind::RECEIVED:
        {
            MessageWriterProxyAlgorithms::Received(data, sequenceNumber);
            break;
        }
        case ChangeFromWriterStatusKind::FILTERED:
        {
            MessageWriterProxyAlgorithms::Filtered(data, sequenceNumber);
            break;
        }
        case ChangeFromWriterStatusKind::UNKNOWN:
        case ChangeFromWriterStatusKind::NO:
        {
            ICRITICAL() << "Status for " << sequenceNumber << " is " << (char) kind;
            return false;
        }
    }
    return true;
}

bool MessageWriterProxyAlgorithms::Filtered(const MapChangeForWriter& data, const SequenceNumber& sequenceNumber)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    auto it = data.find(sequenceNumber);

    if(it != data.cend())
    {
        it->second->NextState(ChangeFromWriterStatusKind::RECEIVED, sequenceNumber);
        it->second->NextState(ChangeFromWriterStatusKind::FILTERED, sequenceNumber);
    }

    return it != data.cend();
}

void MessageWriterProxyAlgorithms::Missing(const MapChangeForWriter& data, const SequenceNumber& lastAvailableSeqNum)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    for(auto it : data)
    {
        const SequenceNumber& seqNum = it.first;

        if(lastAvailableSeqNum <= seqNum)
        {
            if(it.second->CurrentState() == ChangeFromWriterStatusKind::UNKNOWN)
            {
                it.second->NextState(ChangeFromWriterStatusKind::MISSING, seqNum);
            }
        }
        else
        {
            // ASSUMPTION: sequence numbers are in increasing order
            break;
        }
    }
}

void MessageWriterProxyAlgorithms::Lost(const MapChangeForWriter& data, const SequenceNumber& firstAvailableSeqNum)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    for(auto it : data)
    {
        const SequenceNumber& seqNum = it.first;

        if(seqNum < firstAvailableSeqNum)
        {
            continue;
        }

        MessageWriterProxyStatusPtr change = it.second;

        if(change->CurrentState() == ChangeFromWriterStatusKind::UNKNOWN || change->CurrentState() == ChangeFromWriterStatusKind::MISSING)
        {
            change->NextState(ChangeFromWriterStatusKind::LOST, seqNum);
        }
    }
}

bool MessageWriterProxyAlgorithms::Received(const MapChangeForWriter& data, const SequenceNumber& sequenceNumber)
{
    MutexTypeLocker<MapChangeForWriter> lock(&data);

    auto it = data.find(sequenceNumber);

    if(it != data.cend())
    {
        it->second->NextState(ChangeFromWriterStatusKind::RECEIVED, sequenceNumber);
    }

    return it != data.cend();
}

}}
