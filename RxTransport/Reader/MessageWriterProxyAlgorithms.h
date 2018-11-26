#pragma once

#include <RxTransport/CommonDefines.h>
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageWriterProxyAlgorithms
{
    static SequenceNumberSet FindWithStatus(const MapChangeForWriter& data, ChangeFromWriterStatusKind kind);
    static bool              IsWithStatus(const MapChangeForWriter& data, ChangeFromWriterStatusKind kind);
    static SequenceNumber    MaxAvailable(const MapChangeForWriter& data);

    static bool UpdateProxy(MapChangeForWriter& data, const SequenceNumberSet& sequenceNumbers, const ChangeFromWriterStatusKind& kind);
    static bool UpdateProxy(MapChangeForWriter& data, const SequenceNumber& sequenceNumber, const ChangeFromWriterStatusKind& kind);
    static bool Filtered(const MapChangeForWriter& data, const SequenceNumber& sequenceNumber);
    static void Missing(const MapChangeForWriter& data, const SequenceNumber& sequenceNumber);
    static void Lost(const MapChangeForWriter& data, const SequenceNumber& firstAvailableSeqNum);
    static bool Received(const MapChangeForWriter& data, const SequenceNumber& sequenceNumber);
};

}}
