#pragma once

#include"BaseLib/Templates/FunctionHolder.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// EventData
// ----------------------------------------------------

class EventData : public Templates::Function
{
public:
    template <typename EventId>
    EventData(EventId eventId)
        : Function(eventId)
    {}

    virtual ~EventData()
    {}

    CLASS_TRAITS(EventData)
};

// ----------------------------------------------------
// Event0
// ----------------------------------------------------

class Event0 : public EventData
{
public:
    template <typename Function>
    Event0(Function event)
        : EventData(event)
    {}
    virtual ~Event0()
    {}

    CLASS_TRAITS(Event0)
};

// ----------------------------------------------------
// Event1
// ----------------------------------------------------

template <typename ARG1>
class Event1 : public EventData
{
public:
    template <typename Function>
    Event1(Function event, ARG1 arg1)
        : EventData(event)
        , arg1_(arg1)
    {}
    virtual ~Event1()
    {}

    CLASS_TRAITS(Event1)

    virtual ARG1 Arg1() const
    {
        return arg1_;
    }

private:
    ARG1 arg1_;
};

// ----------------------------------------------------
// Event2
// ----------------------------------------------------

template <typename ARG1, typename ARG2>
class Event2 : public EventData
{
public:
    template <typename Function>
    Event2(Function event, ARG1 arg1, ARG2 arg2)
        : EventData(event)
        , arg1_(arg1)
        , arg2_(arg2)
    {}

    virtual ~Event2()
    {}

    CLASS_TRAITS(Event2)

    virtual ARG1 Arg1() const
    {
        return arg1_;
    }

    virtual ARG2 Arg2() const
    {
        return arg2_;
    }

private:
    ARG1 arg1_;
    ARG2 arg2_;
};

// ----------------------------------------------------
// Event3
// ----------------------------------------------------

template <typename ARG1, typename ARG2, typename ARG3>
class Event3 : public EventData
{
public:
    template <typename Function>
    Event3(Function event, ARG1 arg1, ARG2 arg2, ARG3 arg3)
        : EventData(event)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
    {}

    virtual ~Event3()
    {}

    CLASS_TRAITS(Event3)

    virtual ARG1 Arg1() const
    {
        return arg1_;
    }

    virtual ARG2 Arg2() const
    {
        return arg2_;
    }

    virtual ARG3 Arg3() const
    {
        return arg3_;
    }

private:
    ARG1 arg1_;
    ARG2 arg2_;
    ARG3 arg3_;
};

// ----------------------------------------------------
// Event4
// ----------------------------------------------------

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class Event4 : public EventData
{
public:
    template <typename Function>
    Event4(Function event, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)
        : EventData(event)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
    {}

    virtual ~Event4() {}

    CLASS_TRAITS(Event4)

    virtual ARG1 Arg1() const
    {
        return arg1_;
    }

    virtual ARG2 Arg2() const
    {
        return arg2_;
    }

    virtual ARG3 Arg3() const
    {
        return arg3_;
    }

    virtual ARG4 Arg4() const
    {
        return arg4_;
    }

private:
    ARG1 arg1_;
    ARG2 arg2_;
    ARG3 arg3_;
    ARG4 arg4_;
};

// ----------------------------------------------------
// Event5
// ----------------------------------------------------

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
class Event5 : public EventData
{
public:
    template <typename Function>
    Event5(Function event, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)
        : EventData(event)
        , arg1_(arg1)
        , arg2_(arg2)
        , arg3_(arg3)
        , arg4_(arg4)
        , arg5_(arg5)
    {}

    virtual ~Event5()
    {}

    CLASS_TRAITS(Event5)

    virtual ARG1 Arg1() const
    {
        return arg1_;
    }

    virtual ARG2 Arg2() const
    {
        return arg2_;
    }

    virtual ARG3 Arg3() const
    {
        return arg3_;
    }

    virtual ARG4 Arg4() const
    {
        return arg4_;
    }

    virtual ARG5 Arg5() const
    {
        return arg5_;
    }

private:
    ARG1 arg1_;
    ARG2 arg2_;
    ARG3 arg3_;
    ARG4 arg4_;
    ARG5 arg5_;
};

}}

