#ifndef RTPS_Structure_RtpsReaderProxyBase_h_IsIncluded
#define RTPS_Structure_RtpsReaderProxyBase_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE RtpsReaderProxyBase
{
public:
    RtpsReaderProxyBase();
	virtual ~RtpsReaderProxyBase();

public:
	// ----------------------------------------------------
	//   Callback
	// ----------------------------------------------------
	//template <typename ARG>
	//void operator()(const ARG &dataCache);

	// ----------------------------------------------------
	//  Initialize GuardCondition and set callback
	// ----------------------------------------------------
	template <typename Functor, typename ARG>
	bool SetHandler(Functor *func, const ARG &arg);

	template <typename ARG>
	bool SetHandler(const ARG &arg);

	// ----------------------------------------------------
	//  Attach GuardCondition to WaitSet
	// ----------------------------------------------------
	bool AttachWaitSet(WaitSet &waitSet);

public:
	virtual void SetRequestedChangesSet(const SequenceNumberSet &requestedSeqNumbers);

public:
	// based on recommended implementation
	SequenceNumber 		GetHighestSeqNumSent() 		const { return highestSeqNumSent_; }
	SequenceNumber      GetLowestRequestedChange()  const { return lowestRequestedChange_; }
	SequenceNumberSet   GetRequestedChangesSet()    const { return requestedChangesSet_; }

	GuardCondition::Ptr GetGuardCondition()             { return guardCondition_; }

	void				SetHighestSeqNumSent(const SequenceNumber &s) { highestSeqNumSent_ = s; }

	/*------------------------------------------------------------------------------------------------------------------------
		The PIM models the ReaderProxy as maintaining an association with each CacheChange in the Writer’s HistoryCache.
		This association is modeled as being mediated by the association class ChangeForReader. The direct implementation of
		this model would result in a lot of information being maintained for each ReaderProxy. In practice, what is required is
		that the ReaderProxy is able to implement the operations used by the protocol and this does not require the use of explicit
		associations.

		For example, the operations unsent_changes() and next_unsent_change() can be implemented by having the ReaderProxy
		maintain a single sequence number ‘highestSeqNumSent.’ The highestSeqNumSent would record the highest value of the
		sequence number of any CacheChange sent to the ReaderProxy. Using this the operation unsent_changes() could be
		implemented by looking up all changes in the HistoryCache and selecting the ones with sequenceNumber greater than
		highestSeqNumSent. The implementation of next_unsent_change() would also look at the HistoryCache and return the
		CacheChange that has the next-highest sequence number greater than highestSeqNumSent. These operations could be
		done efficiently if the HistoryCache maintains an index by sequenceNumber.

		The same techniques can be used to implement, requested_changes(), requested_changes_set(), and
		next_requested_change(). In this case, the implementation can maintain a sliding window of sequence numbers (which can
		be efficiently represented by a SequenceNumber_t lowestRequestedChange and a fixed-length bitmap) to store whether a
		particular sequence number is currently requested. Requests that do not fit in the window can be ignored as they
		correspond to sequence numbers higher than the ones in the window and the reader can be relied on re-sending the
		request later if it is still missing the change.

		Similar techniques can be used to implement acked_changes_set() and unacked_changes().
	 ------------------------------------------------------------------------------------------------------------------------*/
protected:
	SequenceNumber      highestSeqNumSent_;         // Keeps track of unsent changes

	SequenceNumber      lowestRequestedChange_;     // Keeps track of requested changes
	SequenceNumberSet   requestedChangesSet_;

protected:
	GuardCondition::Ptr guardCondition_;
};

/* --------------------------------------------------
 GuardCondition is initialized when calling SetHandler.
----------------------------------------------------*/
template <typename Functor, typename ARG>
bool RtpsReaderProxyBase::SetHandler(Functor *func, const ARG &arg)
{
	if(this->guardCondition_->HasHandler() == true)
	{
		IWARNING() << "Already initialized GuardCondition handler!";
		return false;
	}

	this->guardCondition_->SetHandler<Functor, ARG>(func, arg);

	return true;
}

template <typename ARG>
bool RtpsReaderProxyBase::SetHandler(const ARG &arg)
{
	if(this->guardCondition_->HasHandler() == true)
	{
		IWARNING() << "Already initialized GuardCondition handler!";
		return false;
	}

	this->guardCondition_->SetHandler(this, arg);

	return true;
}

} // namespace RTPS

#endif // RTPS_Structure_RtpsReaderProxyBase_h_IsIncluded
