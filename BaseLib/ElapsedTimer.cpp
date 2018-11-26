#include"BaseLib/ElapsedTimer.h"
#include "Utility.h"

namespace BaseLib {

ElapsedTimer::ElapsedTimer(int64 intervalMsecs)
    : startTime_(Timestamp::Now())
    , intervalTime_(Duration::FromMilliseconds(intervalMsecs))
    , totalTime_(Duration::Zero())
{ }

ElapsedTimer::ElapsedTimer(Duration interval)
    : startTime_(Timestamp::Now())
    , intervalTime_(interval)
    , totalTime_(Duration::Zero())
{ }

ElapsedTimer::~ElapsedTimer()
{ }

void ElapsedTimer::Start(Duration interval)
{
    startTime_    = Timestamp::Now();
    totalTime_    = Duration::Zero();
    intervalTime_ = interval;
}

void ElapsedTimer::Restart()
{
    Start(intervalTime_);
}

bool ElapsedTimer::HasExpired() const
{
    return elapsed() >= intervalTime_ + totalTime_;
}

void ElapsedTimer::Update()
{
    if(elapsed() >= intervalTime_ + totalTime_)
    {
        totalTime_ = elapsed();
    }
}

bool ElapsedTimer::CheckHasExpiredAndUpdate()
{
    if(elapsed() >= intervalTime_ + totalTime_)
    {
        totalTime_ = elapsed();
        return true;
    }
    return false;
}

Duration ElapsedTimer::Elapsed() const
{
    return elapsed();
}

Duration ElapsedTimer::Remaining() const
{
    return intervalTime_ - elapsed();
}

Duration ElapsedTimer::IntervalMs() const
{
    return intervalTime_;
}

Duration ElapsedTimer::elapsed() const
{
    return Timestamp::Now() - startTime_;
}

}
