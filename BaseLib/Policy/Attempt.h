#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * LifetimeKind
 */
namespace AttemptKind {
enum Type
{
    UNTIL_SUCCESS,
    NUM_SUCCESSFUL_TIMES,
    FOREVER
};
}

/**
 * Attempt: Policy on max number of times to complete some task.
 */
class DLL_STATE Attempt
{
public:
    Attempt(AttemptKind::Type kind, int numSuccessfulTimes, int maxNumAttempts)
        : kind_(kind)
        , numSuccessfulTimes_(numSuccessfulTimes)
        , maxNumAttempts_(maxNumAttempts)
    { }

    virtual ~Attempt()
    { }

    // -----------------------------------------
    // Getters
    // -----------------------------------------

    AttemptKind::Type Kind() const
    {
        return kind_;
    }

    int MaxNumAttempts() const
    {
        return maxNumAttempts_;
    }

    int MinNumSuccesses() const
    {
        return numSuccessfulTimes_;
    }

    // -----------------------------------------
    // Static constructors
    // -----------------------------------------

    static Attempt UntilSuccess(int maxNumAttempts)
    {
        return Attempt(AttemptKind::UNTIL_SUCCESS, 1, maxNumAttempts);
    }

    static Attempt NumSuccessfulTimes(int numSuccessfulTimes, int maxNumAttempts)
    {
        return Attempt(AttemptKind::NUM_SUCCESSFUL_TIMES, numSuccessfulTimes, maxNumAttempts);
    }

    static Attempt Forever()
    {
        return Attempt(AttemptKind::FOREVER, INT_MAX, INT_MAX);
    }

    static Attempt Default()
    {
        return UntilSuccess(5);
    }

    // -----------------------------------------
    // various
    // -----------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const Attempt& qos)
    {
        ostr << TYPE_NAME(qos) << "(" << qos.kind_ << "," << qos.numSuccessfulTimes_ << "," << qos.maxNumAttempts_ << ")";
        return ostr;
    }

private:
    AttemptKind::Type kind_;

    int numSuccessfulTimes_;
    int maxNumAttempts_;
};

}}
