/*
 * ReaderProxy.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include "RTPS/Stateful/ReaderProxy.h"

namespace RTPS
{
/* ------------------------------------------------------------
	class ReaderProxy
-------------------------------------------------------------*/
ReaderProxy::ReaderProxy(bool expectsInlineQos)
	: expectsInlineQos_(expectsInlineQos)
	, isActive_(false)
{
}

ReaderProxy::ReaderProxy(const GUID &guid, bool expectsInlineQos)
	: remoteReaderGuid_(guid)
	, expectsInlineQos_(expectsInlineQos)
{
}

ReaderProxy::~ReaderProxy()
{
}

bool ReaderProxy::AddChangeForReader(const SequenceNumber &seqNum, const ChangeForReader::Ptr &changeForReader)
{
	if(mapChangesForReader_.count(seqNum))
	{
		ICRITICAL() << "Already added sequence number " << seqNum.GetSequenceNumber();
		mapChangesForReader_[seqNum] = changeForReader;
		return true;
	}

	std::pair<MapChangeForReader::iterator, bool> inserted;
	inserted = mapChangesForReader_.insert(std::pair<SequenceNumber, ChangeForReader::Ptr>(seqNum, changeForReader));

	ASSERT(inserted.second);

	return inserted.second;
}

/*---------------------------------------------------------------------------------------------------
This operation takes a SequenceNumber as a parameter and determines whether all ReaderProxy has
acknowledged the CacheChange. The operation will return true if the ReaderProxy has acknowledged
the corresponding CacheChange and false otherwise.

return true IF and only IF
	change IN proxy.changes_for_reader THEN
		change.is_relevant == TRUE AND change.status == ACKNOWLEDGED
---------------------------------------------------------------------------------------------------*/
bool ReaderProxy::IsAckedChange(const SequenceNumber &ackedSeq) const
{
	if(mapChangesForReader_.count(ackedSeq))
	{
		const ChangeForReader::Ptr &changeForReader = mapChangesForReader_.at(ackedSeq);
		if(changeForReader->IsRelevant() && changeForReader->GetStatus() == ChangeForReaderStatusKind::ACKNOWLEDGED)
			return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------------------------
This operation modifies the ChangeForReader status of a set of changes for the RTPS Reader represented by
ReaderProxy ‘this.’ The set of changes with sequence numbers ‘req_seq_num_set’ have their status changed to
REQUESTED.

FOR_EACH seq_num IN req_seq_num_set DO
	FIND change_for_reader IN this.changes_for_reader
			SUCH-THAT (change_for_reader.sequenceNumber==seq_num)
	change_for_reader.status := REQUESTED;
END
----------------------------------------------------------------------------------------------------*/
void ReaderProxy::RequestedChangesSet(const SequenceNumberSet &reqSeqNumSet)
{
	// Check sequence number base
	SequenceNumber seqNumBase = reqSeqNumSet.GetBitmapBase();

	// -- debug --
	ASSERT(seqNumBase != SequenceNumber::UNKNOWN());
	ASSERT(mapChangesForReader_.count(seqNumBase.GetSequenceNumber()));
	// -- debug --

	ChangeForReader::Ptr &changeForReader = mapChangesForReader_.at(seqNumBase.GetSequenceNumber());
	ASSERT(changeForReader);

	changeForReader->SetStatus(ChangeForReaderStatusKind::REQUESTED);

	// Check remaining bits
	for(int64_t i = seqNumBase.GetSequenceNumber(); i < (seqNumBase.GetSequenceNumber() + 1 + reqSeqNumSet.GetNumBits()); i++)
	{
		int position = i - seqNumBase.GetSequenceNumber();

		if(!reqSeqNumSet.TestBitAtPosition(position)) continue;

		SequenceNumber seqNum(seqNumBase.GetSequenceNumber() + 1 + i);

		if(mapChangesForReader_.count(seqNum) <= 0)
		{
			ICRITICAL() << "Requested SequenceNumber " << seqNum.GetSequenceNumber() << " not available in ReaderProxy!";
			continue;
		}

		ChangeForReader::Ptr &changeForReader = mapChangesForReader_.at(seqNum);
		changeForReader->SetStatus(ChangeForReaderStatusKind::REQUESTED);
	}
}

/* --------------------------------------------------------------------------------------------------
This operation changes the ChangeForReader status of a set of changes for the reader represented by ReaderProxy
‘the_reader_proxy.’ The set of changes with sequence number smaller than or equal to the value ‘committed_seq_num’
have their status changed to ACKNOWLEDGED.

FOR_EACH change in this.changes_for_reader
		SUCH-THAT (change.sequenceNumber <= committed_seq_num) DO
change.status := ACKNOWLEDGED;
----------------------------------------------------------------------------------------------------*/
void ReaderProxy::AckedChangesSet(const SequenceNumber &committedSeqNum)
{
	ASSERT(committedSeqNum != SequenceNumber::UNKNOWN());

	for(MapChangeForReader::iterator it = mapChangesForReader_.begin(), it_end = mapChangesForReader_.end(); it != it_end; ++it)
	{
		const SequenceNumber& seqNumber = it->first;
		if(seqNumber > committedSeqNum) break;

		ChangeForReader::Ptr &changeForReader = it->second;
		changeForReader->SetStatus(ChangeForReaderStatusKind::ACKNOWLEDGED);
	}
}

/* --------------------------------------------------------------------------------------------------
This operation returns the ChangeForReader for the ReaderProxy that has the lowest sequence number among the
changes with status ‘REQUESTED.’ This represents the next repair packet that should be sent to the RTPS Reader
represented by the ReaderProxy in response to a previous AckNack message (see Section 8.3.7.1) from the Reader.

next_seq_num := MIN {change.sequenceNumber SUCH-THAT change IN this.requested_changes()}
return change IN this.requested_changes() SUCH-THAT (change.sequenceNumber == next_seq_num);
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::NextRequestedChange(ChangeForReader::Ptr &changeForReader)
{
	return findChange(changeForReader, ChangeForReaderStatusKind::REQUESTED);
}

/* --------------------------------------------------------------------------------------------------
This operation returns the CacheChange for the ReaderProxy that has the lowest sequence number among the changes
with status ‘UNSENT.’ This represents the next change that should be sent to the RTPS Reader represented by the
ReaderProxy.
next_seq_num := MIN { change.sequenceNumber SUCH-THAT change IN this.unsent_changes() };
return change IN this.unsent_changes() SUCH-THAT (change.sequenceNumber == next_seq_num);
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::NextUnsentChange(ChangeForReader::Ptr &changeForReader)
{
	return findChange(changeForReader, ChangeForReaderStatusKind::UNSENT);
}

/* --------------------------------------------------------------------------------------------------
This operation returns the subset of changes for the ReaderProxy that have status ‘REQUESTED.’ This represents the set
of changes that were requested by the RTPS Reader represented by the ReaderProxy using an ACKNACK Message.
return change IN this.changes_for_reader SUCH-THAT (change.status == REQUESTED);
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::RequestedChanges(MapChangeForReader &requestedChanges)
{
	return findChanges(requestedChanges, ChangeForReaderStatusKind::REQUESTED);
}

bool ReaderProxy::HasRequestedChanges() const
{
	return hasChangeState(ChangeForReaderStatusKind::REQUESTED);
}

/* --------------------------------------------------------------------------------------------------
This operation returns the subset of changes for the ReaderProxy the have status ‘UNSENT.’ This represents the set of
changes that have not been sent to the RTPS Reader represented by the ReaderProxy.
return change IN this.changes_for_reader SUCH-THAT (change.status == UNSENT);
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::UnsentChanges(MapChangeForReader &unsentChanges) const
{
	return findChanges(unsentChanges, ChangeForReaderStatusKind::UNSENT);
}

bool ReaderProxy::HasUnsentChanges() const
{
	return hasChangeState(ChangeForReaderStatusKind::UNSENT);
}

/* --------------------------------------------------------------------------------------------------
This operation returns the subset of changes for the ReaderProxy that have status ‘UNACKNOWLEDGED.’ This
represents the set of changes that have not been acknowledged yet by the RTPS Reader represented by the ReaderProxy.
return change IN this.changes_for_reader SUCH-THAT (change.status == UNACKNOWLEDGED);
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::UnackedChanges(MapChangeForReader &unackedChanges)
{
	return findChanges(unackedChanges, ChangeForReaderStatusKind::UNACKNOWLEDGED);
}

bool ReaderProxy::HasUnackedChanges() const
{
	return hasChangeState(ChangeForReaderStatusKind::UNACKNOWLEDGED);
}

/* --------------------------------------------------------------------------------------------------
private functions used to search for ChangeForReader with state
----------------------------------------------------------------------------------------------------*/
bool ReaderProxy::findChanges(MapChangeForReader &requestedChanges, ChangeForReaderStatusKind::Type changeForReaderKind) const
{
	bool found = false;

	// Assumption: SequenceNumber are ordered by increasing numbers
	for(MapChangeForReader::const_iterator it = mapChangesForReader_.begin(), it_end = mapChangesForReader_.end(); it != it_end; ++it)
	{
		const ChangeForReader::Ptr &changeForReader = it->second;
		if(changeForReader->GetStatus() == changeForReaderKind)
		{
			requestedChanges.insert(std::pair<SequenceNumber, ChangeForReader::Ptr>(it->first, it->second));
			found = true;
		}
	}
	return found;
}

bool ReaderProxy::findChange(ChangeForReader::Ptr &change, ChangeForReaderStatusKind::Type changeForReaderKind) const
{
	bool found = false;

	// Assumption: SequenceNumber are ordered by increasing numbers
	for(MapChangeForReader::const_iterator it = mapChangesForReader_.begin(), it_end = mapChangesForReader_.end(); it != it_end; ++it)
	{
		const ChangeForReader::Ptr &changeForReader = it->second;
		if(changeForReader->GetStatus() == changeForReaderKind)
		{
			change = changeForReader;
			found = true;
			break;
		}
	}
	return found;
}

bool ReaderProxy::hasChangeState(ChangeForReaderStatusKind::Type changeForReaderKind) const
{
	for(MapChangeForReader::const_iterator it = mapChangesForReader_.begin(), it_end = mapChangesForReader_.end(); it != it_end; ++it)
	{
		const ChangeForReader::Ptr &changeForReader = it->second;
		if(changeForReader->GetStatus() == changeForReaderKind)
		{
			return true;
		}
	}

	return false;
}

}
