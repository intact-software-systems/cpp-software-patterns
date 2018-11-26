#ifndef BaseLib_Policy_Congestion_h_IsIncluded
#define BaseLib_Policy_Congestion_h_IsIncluded

#include"BaseLib/Policy/MaxLimit.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

// ----------------------------------------------
// Policy for congestion control
// ----------------------------------------------

class DLL_STATE Congestion
{
public:
    Congestion(MaxLimit<float> increase, MaxLimit<float> decrease)
        : increase_(increase)
        , decrease_(decrease)
    {}

    const Policy::MaxLimit<float>& IncreaseRate() const
    {
        return increase_;
    }

    const Policy::MaxLimit<float>& DecreaseRate() const
    {
        return decrease_;
    }

    static Congestion Create(MaxLimit<float> increase, MaxLimit<float> decrease)
    {
        return Congestion(increase, decrease);
    }

private:
    Policy::MaxLimit<float> increase_;   // increase rate
    Policy::MaxLimit<float> decrease_;   // when congestion hits then decrease by rate
};

}}

#endif
