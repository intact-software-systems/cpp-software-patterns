#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE IntervalKind
{
    FromStart,
    FromStop
};

/**
 * @brief The Interval class
 *
 * TODO: Rename to TimeInterval
 * TODO: Interval should include (Interval::FromEndOfTask, Interval::FromBeginningOfTask),
 */
class DLL_STATE Interval
{
public:
    Interval(Duration initialDelay, Duration period, IntervalKind kind = IntervalKind::FromStop)
        : initial_(initialDelay)
        , period_(period)
        , kind_(kind)
    { }

    ~Interval()
    { }

    const Duration& Initial() const
    {
        return initial_;
    }

    const Duration& Period() const
    {
        return period_;
    }

    IntervalKind Kind() const
    {
        return kind_;
    }

    /**
     * Default is start immediately and run every 1000 milliseconds
     */
    static Interval Default()
    {
        return Interval(Duration::Zero(), Duration::FromMilliseconds(1000));
    }

    static Interval RunImmediatelyAndThenEvery(int periodMs)
    {
        return Interval(Duration::Zero(), Duration::FromMilliseconds(periodMs));
    }

    static Interval RunImmediately()
    {
        return Interval(Duration::Zero(), Duration::Zero());
    }

    static Interval DelayedRunAndThenEvery(int initialMs, int periodMs)
    {
        return Interval(Duration::FromMilliseconds(initialMs), Duration::FromMilliseconds(periodMs));
    }

    static Interval DelayedRunAndThenEvery(Duration initial, Duration period)
    {
        return Interval(initial, period);
    }

    static Interval RunEveryMs(int periodMs)
    {
        return Interval(Duration::FromMilliseconds(periodMs), Duration::FromMilliseconds(periodMs));
    }

    static Interval RunEveryMinute(int minutes)
    {
        return Interval(Duration::FromMinutes(minutes), Duration::FromMinutes(minutes));
    }

    static Interval FromMinutes(int minutes)
    {
        return RunEveryMinute(minutes);
    }

    static Interval Infinite()
    {
        return Interval(Duration::Zero(), Duration::Infinite(), IntervalKind::FromStart);
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Interval& qos)
    {
        ostr << "Interval(" << qos.initial_ << "," << qos.period_ << ")";
        return ostr;
    }

    //void generateSleepTime(Packet *pPckt)
    //{
    //    double temp = (SERVICE_RATE/((double)(pPckt->packetSize)));
    //    wait_time = (1/temp) * 1000;
    //    wakeup_time = wait_time + get_current_time();
    //}

private:
    Duration     initial_;
    Duration     period_;
    IntervalKind kind_;
};

}}
