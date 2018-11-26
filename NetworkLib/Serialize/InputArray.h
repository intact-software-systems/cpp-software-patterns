#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/IOBase.h"

namespace NetworkLib
{

template <typename T, typename SharedBuffer = typename RingBufferQueue<T>::Ptr >
class InputArray : public BufferWriter<T>
                 , public ENABLE_SHARED_FROM_THIS_T1(InputArray, T)
{
public:
    InputArray(const SharedBuffer &buffer)
        : buffer_(buffer)
    {}
	virtual ~InputArray()
    {}

    CLASS_TRAITS(InputArray)

    typename InputArray::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual bool Write(const std::vector<T> &bytes)
    {
        return buffer_->Enqueue(bytes, bytes.size());
    }

    virtual void Clear()
    {
        buffer_->Clear();
    }

private:
    SharedBuffer buffer_;
};

}
