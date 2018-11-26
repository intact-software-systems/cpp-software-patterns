#ifndef DCPS_Status_DataStatus_h_Included
#define DCPS_Status_DataStatus_h_Included

#include<bitset>

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Status
{

/**
 * @brief A data sample state can be either Read, NotRead or Any.
 *
 * For each sample received, the middleware internally maintains a sample_state relative to each DataReader.
 */
class DLL_STATE SampleState : public std::bitset<DEFAULT_STATE_BIT_COUNT>
{
public:
    typedef std::bitset<DEFAULT_STATE_BIT_COUNT> MaskType;

public:
    SampleState() : MaskType() { }
    explicit SampleState(uint32_t i) : MaskType((int32_t)i) { }
    SampleState(const SampleState& src) : MaskType(src) { }
    SampleState(const MaskType& src) : MaskType(src) { }

public:
    /**
     * @brief READ indicates that the DataReader has already accessed that sample by means of read
     *
     * @return
     */
    inline static const SampleState Read()
    {
        return SampleState(0x0001 << 0u);
    }

    /**
     * @brief NOT READ indicates that the DataReader has not accessed that sample before.
     *
     * @return
     */
    inline static const SampleState NotRead()
    {
        return SampleState(0x0001 << 1u);
    }

    inline static const SampleState Any() {
        return SampleState(~0u);
    }
};

/**
 * @brief A data view state can be either New, NotNew or Any
 */
class DLL_STATE ViewState : public std::bitset<DEFAULT_STATE_BIT_COUNT>
{
public:
    typedef std::bitset<DEFAULT_STATE_BIT_COUNT> MaskType;

public:
    ViewState() : MaskType() { }
    explicit ViewState(uint32_t m) : MaskType((int32_t)m) { }
    ViewState(const ViewState& src) : MaskType(src) { }
    ViewState(const MaskType& src) : MaskType(src) { }

public:
    inline static const ViewState NewView()
    {
        return ViewState(0x0001 << 0u);
    }

    inline static const ViewState NotNewView()
    {
        return ViewState(0x0001 << 1u);
    }

    inline static const ViewState Any()
    {
        return ViewState(~0u);
    }
};

/**
 * @brief A data instance state can be Alive, NotAliveDisposed, NotAliveNoWriters, NotAliveMask or Any
 *
 * For each instance the middleware internally maintains an instance_state.
 */
class DLL_STATE InstanceState : public std::bitset<DEFAULT_STATE_BIT_COUNT>
{
public:
    typedef std::bitset<DEFAULT_STATE_BIT_COUNT> MaskType;

public:
    explicit InstanceState(uint32_t m) : MaskType((int32_t)m) { }
    InstanceState() : MaskType() { }
    InstanceState(const InstanceState& src) : MaskType(src) { }
    InstanceState(const MaskType& src) : MaskType(src) { }

public:

    /**
     * @brief ALIVE indicates that (a) samples have been received for the instance, (b) there are
     * live DataWriter entities writing the instance, and (c) the instance has not been explicitly
     * disposed (or else more samples have been received after it was disposed).
     *
     * @return
     */
    inline static const InstanceState Alive()
    {
        return InstanceState(0x0001 << 0u);
    }

    /**
     * @brief NOT ALIVE DISPOSED indicates the instance was explicitly disposed by a DataWriter by
     * means of the dispose operation.
     *
     * @return
     */
    inline static const InstanceState NotAliveDisposed()
    {
        return InstanceState(0x0001 << 1u);
    }

    /**
     * @brief NotAliveNoWriters indicates the instance has been declared as not-alive by the DataReader
     * because it detected that there are no live DataWriter entities writing that instance.
     *
     * @return
     */
    inline static const InstanceState NotAliveNoWriters()
    {
        return InstanceState(0x0001 << 2u);
    }

    inline static const InstanceState NotAliveMask()
    {
        return NotAliveDisposed() | NotAliveNoWriters();
    }

    inline static const InstanceState Any()
    {
        return InstanceState(~0u);
    }
};

/**
 * @brief A container class for SampleState, ViewState and InstanceState.
 */
class DLL_STATE DataState
{
public:
    DataState()
        : sampleState_(SampleState::Any())
        , viewState_(ViewState::Any())
        , instanceState_(InstanceState::Any())
    { }

    /* implicit */ DataState(const SampleState& ss)
        : sampleState_(ss)
        , viewState_(ViewState::Any())
        , instanceState_(InstanceState::Any())
    { }

    /* implicit */ DataState(const ViewState& vs)
        : sampleState_(SampleState::Any())
        , viewState_(vs)
        , instanceState_(InstanceState::Any())
    { }

    /* implicit */ DataState(const InstanceState& is)
        : sampleState_(SampleState::Any())
        , viewState_(ViewState::Any())
        , instanceState_(is)
    { }

    DataState(const SampleState& ss,
              const ViewState& vs,
              const InstanceState& is)
        : sampleState_(ss), viewState_(vs), instanceState_(is)
    { }

    DataState& operator << (const SampleState& ss) {
        sampleState_ = ss;
        return *this;
    }

    DataState& operator << (const InstanceState& is)
    {
        instanceState_ = is;
        return *this;
    }

    DataState& operator << (const ViewState& vs)
    {
        viewState_ = vs;
        return *this;
    }

    const DataState& operator >> (SampleState& ss) const
    {
        ss = sampleState_;
        return *this;
    }

    const DataState& operator >> (InstanceState& is) const
    {
        is = instanceState_;
        return *this;
    }

    const DataState& operator >> (ViewState& vs) const
    {
        vs = viewState_;
        return *this;
    }

    const SampleState& GetSampleState() const
    {
        return sampleState_;
    }

    void SetSampleState(const SampleState& ss)
    {
        *this << ss;
    }

    const InstanceState& GetInstanceState() const
    {
        return instanceState_;
    }

    void SetInstanceState(const InstanceState& is)
    {
        *this << is;
    }

    const ViewState& GetViewState() const
    {
        return viewState_;
    }

    void SetViewState(const ViewState& vs)
    {
        *this << vs;
    }

    static DataState Any()
    {
        return DataState(SampleState::Any(), ViewState::Any(),InstanceState::Any());
    }

    static DataState NewData()
    {
        return DataState(SampleState::NotRead(),ViewState::Any(),InstanceState::Alive());
    }

    static DataState AnyData()
    {
        return DataState(SampleState::Any(),ViewState::Any(),InstanceState::Alive());
    }

    static DataState NewInstance()
    {
        return DataState(SampleState::Any(),ViewState::NewView(),InstanceState::Alive());
    }

private:
    SampleState     sampleState_;
    ViewState       viewState_;
    InstanceState   instanceState_;
};

}}

#endif
