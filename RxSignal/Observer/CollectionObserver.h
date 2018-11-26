#pragma once

namespace BaseLib { namespace Observer
{

class CollectionObserver
{
public:
    virtual void OnDataIn() = 0;
    virtual void OnDataOut() = 0;
    virtual void OnDataModified() = 0;
};

template <typename T>
class CollectionObserver1
{
public:
    virtual void OnDataIn(T value) = 0;
    virtual void OnDataOut(T value) = 0;
    virtual void OnDataModified(T value) = 0;
};

}}
