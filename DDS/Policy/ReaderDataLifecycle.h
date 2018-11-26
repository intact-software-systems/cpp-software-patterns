/*
 * ReaderDataLifecycle.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_ReaderDataLifecycle_h_Included
#define DDS_Policy_ReaderDataLifecycle_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @abstract
 *
 * This policy controls the behavior of the DataReader with regards to the lifecycle of the data-instances it manages, that is,
 * the data-instances that have been received and for which the DataReader maintains some internal resources.
 * The DataReader internally maintains the samples that have not been taken by the application, subject to the constraints
 * imposed by other QoS policies such as HISTORY and RESOURCE_LIMITS.
 *
 * The DataReader also maintains information regarding the identity, view_state and instance_state of data-instances even
 * after all samples have been 'taken.' This is needed to properly compute the states when future samples arrive.
 * Under normal circumstances the DataReader can only reclaim all resources for instances for which there are no writers
 * and for which all samples have been 'taken.' The last sample the DataReader will have taken for that instance will have
 * an instance_state of either NOT_ALIVE_NO_WRITERS or NOT_ALIVE_DISPOSED depending on whether the last
 * writer that had ownership of the instance disposed it or not. Refer to Figure 7.11 for a statechart describing the transitions
 * possible for the instance_state. In the absence of the READER_DATA_LIFECYCLE QoS this behavior could cause
 * problems if the application "forgets" to 'take' those samples. The 'untaken' samples will prevent the DataReader from
 * reclaiming the resources and they would remain in the DataReader indefinitely.
 *
 * The autopurge_nowriter_samples_delay defines the maximum duration for which the DataReader will maintain
 * information regarding an instance once its instance_state becomes NOT_ALIVE_NO_WRITERS. After this time elapses,
 * the DataReader will purge all internal information regarding the instance, any untaken samples will also be lost.
 * The autopurge_disposed_samples_delay defines the maximum duration for which the DataReader will maintain samples
 * for an instance once its instance_state becomes NOT_ALIVE_DISPOSED. After this time elapses, the DataReader will
 * purge all samples for the instance.
 */
class DLL_STATE ReaderDataLifecycle : public QosPolicyBase
{
public:
    ReaderDataLifecycle()
        : nowriterDelay_(BaseLib::Duration::Infinite())
        , disposedSamplesDelay_(BaseLib::Duration::Infinite())
    { }

    ReaderDataLifecycle(const BaseLib::Duration& the_nowriter_delay,
                        const BaseLib::Duration& the_disposed_samples_delay)
        : nowriterDelay_(the_nowriter_delay)
        , disposedSamplesDelay_(the_disposed_samples_delay)
    { }

    DEFINE_POLICY_TRAITS(ReaderDataLifecycles, 17, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *) const
    {

    }

    virtual void Read(NetworkLib::SerializeReader *)
    {

    }

public:
    const BaseLib::Duration& GetNowriterDelay() 			const { return nowriterDelay_; }
    const BaseLib::Duration& GetDisposedSamplesDelay() 	const { return disposedSamplesDelay_; }

    void SetNowriterDelay(BaseLib::Duration& d) 					{ nowriterDelay_ = d; }
    void SetDisposedSamplesDelay(const BaseLib::Duration& d) 	{ disposedSamplesDelay_ = d; }

public:
    static ReaderDataLifecycle NoAutoPurgeDisposedSamples()
    {
        return ReaderDataLifecycle();
    }

    static ReaderDataLifecycle AutoPurgeDisposedSamples(const BaseLib::Duration& d)
    {
        return ReaderDataLifecycle(d, Duration::Zero());
    }

private:
    BaseLib::Duration nowriterDelay_;
    BaseLib::Duration disposedSamplesDelay_;
};

}}

#endif
