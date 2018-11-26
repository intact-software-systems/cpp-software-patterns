#pragma once

#include"RxTransport/CommonDefines.h"
#include"MessageReader.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class MessageReaders
    : public Collection::details::StdMapCollectionType<ReaderId, std::shared_ptr<MessageReader<T>>>
{
    typedef Collection::details::StdMapCollectionType<ReaderId, std::shared_ptr<MessageReader<T>>> M;

public:
    virtual ~MessageReaders()
    { }

    void Activate()
    {
        this->ForEach([](std::shared_ptr<MessageReader<T>> item) { item->Activate(); });
    }

    void Deactivate()
    {
        this->ForEach([](std::shared_ptr<MessageReader<T>> item) { item->Deactivate(); });
    }

    virtual std::shared_ptr<MessageReader<T>> Remove(const ReaderId& key)
    {
        return M::template RemoveAndApply<std::shared_ptr<MessageReader<T>>>(
            key,
            [](std::shared_ptr<MessageReader<T>> item)
            {
                item->Deactivate();
                return item;
            }
        );
    }

    // ---------------------------------------------
    // Print data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << Utility::GetTypeName<MessageReaders<T>>() << " = " << this->Size() << ":";

        for(auto entry : this->collection())
        {
            ostr << entry.second << std::endl;
        }

        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageReaders<T>& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageReaders<T>>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
