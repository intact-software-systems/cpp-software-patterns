#pragma once

#include"RxTransport/CommonDefines.h"
#include"MessageWriter.h"

namespace BaseLib { namespace Concurrent {

template <typename T>
class MessageWriters
    : public Collection::details::StdMapCollectionType<WriterId, std::shared_ptr<MessageWriter<T>>>
{
    typedef Collection::details::StdMapCollectionType<WriterId, std::shared_ptr<MessageWriter<T>>> M;

public:
    virtual ~MessageWriters()
    { }

    void Activate()
    {
        this->ForEach([](std::shared_ptr<MessageWriter<T>> item) { item->Activate(); });
    }

    void Deactivate()
    {
        this->ForEach([](std::shared_ptr<MessageWriter<T>> item) { item->Deactivate(); });
    }

    virtual std::shared_ptr<MessageWriter<T>> Remove(const WriterId& key)
    {
        return M::template RemoveAndApply<std::shared_ptr<MessageWriter<T>>>(
            key,
            [](std::shared_ptr<MessageWriter<T>> item)
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
        ostr << Utility::GetTypeName<MessageWriters<T>>() << " = " << this->Size() << ":";

        for(auto entry : this->collection())
        {
            ostr << entry.second << std::endl;
        }

        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageWriters<T>& proxy)
    {
        ostr << proxy.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageWriters<T>>& proxy)
    {
        ostr << proxy->toString();
        return ostr;
    }
};

}}
