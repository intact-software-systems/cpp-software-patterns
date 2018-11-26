#include "BaseLib/Status/EventCount.h"

namespace BaseLib { namespace Status
{

EventCount::EventCount()
{ }

EventCount::~EventCount()
{ }

void EventCount::Accept()
{
    ++total_;
    ++numTimesAccepted_;
}

void EventCount::Ignore()
{
    ++total_;
    ++numTimesIgnored_;
}

void EventCount::Filter()
{
    ++total_;
    ++numTimesFiltered_;
}

void EventCount::Block()
{
    ++total_;
    ++numTimesBlocked_;
}

const Count &EventCount::Total() const
{
    return total_;
}

const Count &EventCount::NumAccepted() const
{
    return numTimesAccepted_;
}

const Count &EventCount::NumFiltered() const
{
    return numTimesFiltered_;
}

const Count &EventCount::NumIgnored() const
{
    return numTimesIgnored_;
}

const Count &EventCount::NumBlocked() const
{
    return numTimesBlocked_;
}


}}
