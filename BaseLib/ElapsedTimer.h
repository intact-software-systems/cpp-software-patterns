#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Export.h"

namespace BaseLib {

/**
 * @brief The ElapsedTimer class provides lap stop-watch functionality. The constructor
 * is given an intervalTime in milliseconds, and the class functions check wether
 * the interval (lap) has expired.
 */
class DLL_STATE ElapsedTimer
{
public:
    ElapsedTimer(int64 intervalMsecs);
    ElapsedTimer(Duration interval = Duration::Infinite());
    ~ElapsedTimer();

public:
    void Start(Duration interval = Duration::Infinite());
    void Restart();
    void Update();

    bool HasExpired() const;
    bool CheckHasExpiredAndUpdate();

    Duration Elapsed() const;
    Duration Remaining() const;
    Duration IntervalMs() const;

    friend std::ostream& operator<<(std::ostream& ostr, const ElapsedTimer& timer)
    {
        ostr << TYPE_NAME(timer) << "(" << timer.startTime_ << "," << timer.intervalTime_ << "," << timer.totalTime_ << ")";
        return ostr;
    }

private:
    Duration elapsed() const;

private:
    Timestamp startTime_;
    Duration  intervalTime_;
    Duration  totalTime_;
};

}

