#pragma once

#include <RxTransport/CommonDefines.h>
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageReaderProxyAlgorithms
{
    static MapChangeForReader UpdateWithStatus(const MapChangeForReader& data, const SequenceNumberSet& reqSeqNumSet, ChangeForReaderStatusKind kind);
    static MapChangeForReader UpdateWithStatusUntil(const MapChangeForReader& data, const SequenceNumber& reqSeqNumSet, ChangeForReaderStatusKind kind);

    static MapChangeForReader SelectNWithStatus(const MapChangeForReader& data, int n, std::set<ChangeForReaderStatusKind> kinds);

    static bool IsInStatus(const MapChangeForReader& data, const SequenceNumber& handle, ChangeForReaderStatusKind kind);
    static bool IsAnyInStatus(const MapChangeForReader& data, ChangeForReaderStatusKind kind);
};

}}

