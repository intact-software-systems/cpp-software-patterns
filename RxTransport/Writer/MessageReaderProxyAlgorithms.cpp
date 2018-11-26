#include "MessageReaderProxyAlgorithms.h"

namespace BaseLib { namespace Concurrent {

MapChangeForReader MessageReaderProxyAlgorithms::UpdateWithStatus(const MapChangeForReader& data, const SequenceNumberSet& reqSeqNumSet, ChangeForReaderStatusKind kind)
{
    MapChangeForReader updated;

    for(auto number : reqSeqNumSet)
    {
        MutexTypeLocker<MapChangeForReader> lock(&data);

        auto it = data.find(number);
        if(it != data.cend())
        {
            it->second->NextState(kind, number);
            updated.insert(std::pair<SequenceNumber, MessageReaderProxyStatusPtr>(it->first, it->second));
        }
        else
        {
            ASSERT(it != data.cend());
        }
    }

    return updated;
}

MapChangeForReader MessageReaderProxyAlgorithms::UpdateWithStatusUntil(const MapChangeForReader& data, const SequenceNumber& lastNumber, ChangeForReaderStatusKind kind)
{
    MutexTypeLocker<MapChangeForReader> lock(&data);

    MapChangeForReader updated;

    for(auto item : data)
    {
        const SequenceNumber& number = item.first;
        if(number > lastNumber) break;

        if(item.second->CurrentState() != kind)
        {
            item.second->NextState(kind, number);
            updated.insert(std::pair<SequenceNumber, MessageReaderProxyStatusPtr>(item.first, item.second));
        }
        else
        {
            IINFO() << "Change " << number << " already " << (char) kind;
        }
    }

    return updated;
}

MapChangeForReader MessageReaderProxyAlgorithms::SelectNWithStatus(const MapChangeForReader& data, int n, std::set<ChangeForReaderStatusKind> kinds)
{
    ASSERT(n > 0);
    ASSERT(!kinds.empty());

    int selected = 0;

    MapChangeForReader requestedChanges;

    MutexTypeLocker<MapChangeForReader> lock(&data);

    // Assumption: SequenceNumber are ordered by increasing numbers

    for(auto it = data.cbegin(), it_end = data.cend(); it != it_end && selected <= n; ++it)
    {
        MessageReaderProxyStatusPtr changeForReader = it->second;
        if(kinds.find(changeForReader->CurrentState()) != kinds.cend())
        {
            requestedChanges.insert(std::pair<SequenceNumber, MessageReaderProxyStatusPtr>(it->first, it->second));
            selected++;
        }
    }

    return requestedChanges;
}

bool MessageReaderProxyAlgorithms::IsInStatus(const MapChangeForReader& data, const SequenceNumber& handle, ChangeForReaderStatusKind kind)
{
    MutexTypeLocker<MapChangeForReader> lock(&data);

    auto it = data.find(handle);

    return it != data.cend()
           ? it->second->CurrentState() == kind
           : false;
}

bool MessageReaderProxyAlgorithms::IsAnyInStatus(const MapChangeForReader& data, ChangeForReaderStatusKind kind)
{
    MutexTypeLocker<MapChangeForReader> lock(&data);

    for(auto entry : data)
    {
        if(entry.second->CurrentState() == kind)
        {
            return true;
        }
    }

    return false;
}

}}
