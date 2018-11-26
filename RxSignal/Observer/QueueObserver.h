#pragma once

namespace BaseLib { namespace Observer
{

class QueueObserver
{
public:
    virtual ~QueueObserver() {}

    virtual void OnDataIn() = 0;
    virtual void OnDataOut() = 0;
};

template <typename T>
class QueueObserver1
{
public:
    virtual ~QueueObserver1() {}

    virtual void OnDataIn(T value) = 0;
    virtual void OnDataOut(T value) = 0;
};

template <typename K, typename V>
class QueueObserver2
{
public:
    virtual ~QueueObserver2() {}

    virtual void OnDataIn(K key, V value) = 0;
    virtual void OnDataOut(K key, V value) = 0;
};

}}
