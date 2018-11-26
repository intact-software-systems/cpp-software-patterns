#include "RTPS/Structure/RtpsReaderProxyBase.h"

namespace RTPS
{

RtpsReaderProxyBase::RtpsReaderProxyBase()
    : guardCondition_(new GuardCondition())
{

}

RtpsReaderProxyBase::~RtpsReaderProxyBase()
{

}

/* --------------------------------------------------
 Function call operator is called from GuardCondition
 and the GuardCondition is initialized when calling
 SetHandler.
----------------------------------------------------*/
bool RtpsReaderProxyBase::AttachWaitSet(WaitSet &waitSet)
{
	return waitSet.Attach(guardCondition_);
}

void RtpsReaderProxyBase::SetRequestedChangesSet(const SequenceNumberSet &requestedSeqNumbers)
{
	requestedChangesSet_ = requestedSeqNumbers;
	lowestRequestedChange_ = requestedSeqNumbers.GetBitmapBase();
}

} // namespace RTPS
