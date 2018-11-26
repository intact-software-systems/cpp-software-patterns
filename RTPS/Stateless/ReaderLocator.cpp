/*
 * ReaderLocator.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include "RTPS/Stateless/ReaderLocator.h"

namespace RTPS
{
/* --------------------------------------------------
	ReaderLocator is created from StatelessWriter
----------------------------------------------------*/
ReaderLocator::ReaderLocator(const Locator &locator, bool expectsInlineQos)
	: locator_(locator)
	, expectsInlineQos_(expectsInlineQos)
{

}

ReaderLocator::ReaderLocator(bool expectsInlineQos)
	: expectsInlineQos_(expectsInlineQos)
{

}

ReaderLocator::~ReaderLocator()
{

}

/* --------------------------------------------------
 Function call operator is called from GuardCondition
 and the GuardCondition is initialized when calling
 SetHandler.
----------------------------------------------------*/
/*template<>
void ReaderLocator::operator()<DataCache>(const DataCache &dataCache)
{
	IDEBUG() << "Callback after new cache object added!";

	SequenceNumber highestSeqNum = dataCache.GetSequenceNumberMax();

	if(highestSeqNum > this->highestSeqNumSent_)
	{
		// TODO: How to notify to send to Locator?

		// SendOut(locator_, SequenceNumberRange(this->highestSeqNumSent_, highestSeqNum));
	}
}*/

/* --------------------------------------------------
	TODO: Implement
----------------------------------------------------*/
/*ChangeForReader ReaderLocator::NextRequestedChange()
{
	// TODO: Implement
	return ChangeForReader();
}

ChangeForReader ReaderLocator::NextUnsentChange()
{
	// TODO: Implement
	return ChangeForReader();
}

ReaderLocator::MapCacheChange ReaderLocator::RequestedChanges()
{
	// TODO: Implement
	return ReaderLocator::MapCacheChange();
}

ReaderLocator::MapCacheChange ReaderLocator::UnsentChanges()
{
	// TODO: Implement
	return ReaderLocator::MapCacheChange();
}*/

} // namespace RTPS
