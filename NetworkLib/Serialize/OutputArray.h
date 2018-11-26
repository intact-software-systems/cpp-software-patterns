#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/IOBase.h"

namespace NetworkLib {

template <typename T, typename SharedBuffer = typename RingBufferQueue<T>::Ptr>
class OutputArray : public BufferReader<T>
                    , public ENABLE_SHARED_FROM_THIS_T1(OutputArray, T)
{
public:
    OutputArray(const SharedBuffer& buffer)
        : buffer_(buffer)
    {}

    virtual ~OutputArray()
    {}

    CLASS_TRAITS(OutputArray)

    typename OutputArray::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual std::vector<T> Read()
    {
        return buffer_->PeekAll();
    }

private:
    SharedBuffer buffer_;
};

}
