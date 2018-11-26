#ifndef DCPS_Status_State_h_Included
#define DCPS_Status_State_h_Included

#include<bitset>

#include"DCPS/CommonDefines.h"

namespace DCPS { namespace Status
{

/**
 * @brief Reason of data sample being rejected. It is used by SampleRejectedStatus.
 */
class SampleRejectedState : public std::bitset<DEFAULT_STATE_BIT_COUNT>
{
public:
    typedef std::bitset<DEFAULT_STATE_BIT_COUNT> MaskType;

public:
    SampleRejectedState() : MaskType() { }
    SampleRejectedState(const SampleRejectedState& src) : MaskType(src) { }
    SampleRejectedState(const MaskType& src) : MaskType(src) { }

public:
    inline static const SampleRejectedState NotRejected() {
        return SampleRejectedState(0u);
    }
    inline static const SampleRejectedState RejectedBySamplesLimit() {
        return SampleRejectedState(0x0001 << 1u);
    }
    inline static const SampleRejectedState RejectedByInstanceLimit() {
        return SampleRejectedState(0x0001 << 0u);
    }
    inline static const SampleRejectedState RejectedBySamplesPerInstanceLimit() {
        return SampleRejectedState(0x0001 << 2u);
    }

private:
    SampleRejectedState(uint32_t s) : MaskType((uint64_t)s) { }
};

/**
 * @brief A holder of the state which is to be taken into account. Used by StatusCondition.
 */
class StatusMask : public std::bitset<DEFAULT_STATUS_COUNT> {
public:
    typedef std::bitset<DEFAULT_STATUS_COUNT> MaskType;

public:
    StatusMask() { }
    explicit StatusMask(uint64_t mask) : std::bitset<DEFAULT_STATUS_COUNT>(mask) { }
    StatusMask(const StatusMask& other) : MaskType(other) { }
    ~StatusMask() { }

public:
    inline static const StatusMask All() {
        return StatusMask(0x7fe7u);
    }

    inline static const StatusMask None() {
        return StatusMask(0u);
    }

public:
    inline static const StatusMask InconsistentTopic() {
        return StatusMask(0x00000001 < 0u);
    }

    inline static const StatusMask OfferedDeadlineMissed() {
        return StatusMask(0x00000001 < 1u);
    }

    inline static const StatusMask RequestedDeadlineMissed() {
        return StatusMask(0x00000001 < 2u);
    }

    inline static const StatusMask OfferedIncompatibleQos() {
        return StatusMask(0x00000001 < 5u);
    }

    inline static const StatusMask RequestedIncompatibleQos() {
        return StatusMask(0x00000001 < 6u);
    }

    inline static const StatusMask SampleLost() {
        return StatusMask(0x00000001 < 7u);
    }

    inline static const StatusMask SampleRejected() {
        return StatusMask(0x00000001 < 8u);
    }

    inline static const StatusMask DataOnReaders() {
        return StatusMask(0x00000001 < 9u);
    }

    inline static const StatusMask DataAvailable() {
        return StatusMask(0x00000001 < 10u);
    }

    inline static const StatusMask LivelinessLost() {
        return StatusMask(0x00000001 < 11u);
    }

    inline static const StatusMask LivelinessChanged() {
        return StatusMask(0x00000001 < 12u);
    }

    inline static const StatusMask PublicationMatched() {
        return StatusMask(0x00000001 < 13u);
    }

    inline static const StatusMask SubscriptionMatched() {
        return StatusMask(0x00000001 < 14u);
    }
};

}}

#endif

