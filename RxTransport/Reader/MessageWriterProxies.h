#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageWriterProxies
    : public Collection::details::StdMapCollectionType<WriterId, MessageWriterProxyPtr>
{
public:
    virtual ~MessageWriterProxies()
    { }

    void Activate();
    void Deactivate();

    bool Update(const WriterId& writerId, const SequenceNumber& sequenceNumber, ChangeFromWriterStatusKind kind);
    bool Update(const WriterId& writerId, const SequenceNumberSet& sequenceNumbers, ChangeFromWriterStatusKind kind);

    void AddForAll(const SequenceNumber& sequenceNumber, ChangeFromWriterStatusKind kind);

    bool IsMissingFor(const WriterId& writerId) const;

    // ---------------------------------------------
    // Print proxy data
    // ---------------------------------------------

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& ostr, const MessageWriterProxies& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageWriterProxies>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}

