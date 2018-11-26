#pragma once

namespace BaseLib { namespace Observer
{

class FilterOutObserver
{
public:
    virtual void OnFilteredOut() = 0;
};

class FilterInObserver
{
public:
    virtual void OnFilteredIn() = 0;
};

class FilterObserver
        : public FilterOutObserver
        , public FilterInObserver
{ };

template <typename T>
class FilterOutObserver1
{
public:
    virtual void OnFilteredOut(T) = 0;
};

template <typename T>
class FilterInObserver1
{
public:
    virtual void OnFilteredIn(T) = 0;
};

template <typename T>
class FilterObserver1
        : public FilterOutObserver1<T>
        , public FilterInObserver1<T>
{ };

}}
