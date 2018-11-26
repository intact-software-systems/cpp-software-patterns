/*
 * WriterProxy.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include "RTPS/Stateful/WriterProxy.h"

namespace RTPS
{
/* -----------------------------------------------
		class WriterProxy
------------------------------------------------*/
WriterProxy::WriterProxy()
{
}

WriterProxy::WriterProxy(const GUID &guid)
	: remoteWriterGuid_(guid)
{
}


WriterProxy::~WriterProxy()
{
}

/* --------------------------------------------------------------------------------------------------
This operation returns the maximum SequenceNumber_t among the changes_from_writer changes in the RTPS
WriterProxy that are available for access by the DDS DataReader

The condition to make any CacheChange ‘a_change’ available for ‘access’ by the DDS DataReader is that there are no
changes from the RTPS Writer with SequenceNumber_t smaller than or equal to a_change.sequenceNumber that have
status MISSING or UNKNOWN. In other words, the available_changes_max and all previous changes are either
RECEIVED or LOST.

Logical action in the virtual machine:
seq_num := MAX { change.sequenceNumber SUCH-THAT
				( change IN this.changes_from_writer
					AND ( change.status == RECEIVED
						OR change.status == LOST) ) };
return seq_num;
---------------------------------------------------------------------------------------------------*/
SequenceNumber WriterProxy::AvailableChangesMax()
{
	for(MapChangeForWriter::reverse_iterator it = mapChangesFromWriter_.rbegin(), it_end = mapChangesFromWriter_.rend(); it != it_end; ++it)
	{
		ChangeFromWriter &changeFromWriter = it->second;
		if(changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::RECEIVED ||
			changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::LOST)
		{
			return it->first;
		}
	}

	IWARNING() << "No available changes found!";

	return SequenceNumber::UNKNOWN();
}

/* --------------------------------------------------------------------------------------------------
This operation modifies the status of a ChangeFromWriter to indicate that the CacheChange with the
SequenceNumber_t ‘a_seq_num’ is irrelevant to the RTPS Reader.
Logical action in the virtual machine:

FIND change FROM this.changes_from_writer SUCH-THAT
						(change.sequenceNumber == a_seq_num);
change.status := RECEIVED;
change.is_relevant := FALSE;
---------------------------------------------------------------------------------------------------*/
void WriterProxy::IrrelevantChangeSet(const SequenceNumber &sequenceNumber)
{
	if(mapChangesFromWriter_.count(sequenceNumber))
	{
		ChangeFromWriter &changeFromWriter = mapChangesFromWriter_[sequenceNumber];
		changeFromWriter.SetStatus(ChangeFromWriterStatusKind::RECEIVED);
		changeFromWriter.SetRelevant(false);
	}
	else
	{
		ICRITICAL() << "Sequence number " << sequenceNumber.GetSequenceNumber() << " not found!";
	}
}

/* --------------------------------------------------------------------------------------------------
This operation modifies the status stored in ChangeFromWriter for any changes in the WriterProxy whose status is
MISSING or UNKNOWN and have sequence numbers lower than ‘first_available_seq_num.’ The status of those changes
is modified to LOST indicating that the changes are no longer available in the WriterHistoryCache of the RTPS Writer
represented by the RTPS WriterProxy.
Logical action in the virtual machine:

FOREACH change IN this.changes_from_writer
	SUCH-THAT ( change.status == UNKNOWN OR change.status == MISSING
		AND seq_num < first_available_seq_num )	DO
{
	change.status := LOST;
}
---------------------------------------------------------------------------------------------------*/
void WriterProxy::LostChangesUpdate(const SequenceNumber &firstAvailableSeqNum)
{
	for(MapChangeForWriter::iterator it = mapChangesFromWriter_.begin(), it_end = mapChangesFromWriter_.end(); it != it_end; ++it)
	{
		const SequenceNumber &seqNum = it->first;
		if(seqNum < firstAvailableSeqNum) continue;

		ChangeFromWriter &changeFromWriter = it->second;
		if(changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::UNKNOWN ||
			changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::MISSING)
		{
			changeFromWriter.SetStatus(ChangeFromWriterStatusKind::LOST);
		}
	}
}

/* --------------------------------------------------------------------------------------------------
This operation returns the subset of changes for the WriterProxy that have status ‘MISSING.’ The changes with status
‘MISSING’ represent the set of changes available in the HistoryCache of the RTPS Writer represented by the RTPS
WriterProxy that have not been received by the RTPS Reader.

return { change IN this.changes_from_writer SUCH-THAT change.status == MISSING };
---------------------------------------------------------------------------------------------------*/
SequenceSet WriterProxy::MissingChanges() const
{
	SequenceSet sequenceNumberSet;

	for(MapChangeForWriter::const_iterator it = mapChangesFromWriter_.begin(), it_end = mapChangesFromWriter_.end(); it != it_end; ++it)
	{
		const ChangeFromWriter &changeFromWriter = it->second;
		if(changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::MISSING)
		{
			sequenceNumberSet.insert(it->first);
		}
	}

	// -- debug --
	if(sequenceNumberSet.empty())
		ICRITICAL() << "Sequence number set is empty!";
	// -- end debug --

	return sequenceNumberSet;
}

// ----------------------------------------------------------
// Convenience function to check if there are missing changes
// ----------------------------------------------------------
bool WriterProxy::HasMissingChanges() const
{
	for(MapChangeForWriter::const_iterator it = mapChangesFromWriter_.begin(), it_end = mapChangesFromWriter_.end(); it != it_end; ++it)
	{
		const ChangeFromWriter &changeFromWriter = it->second;
		if(changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::MISSING)
			return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------------------------
This operation modifies the status stored in ChangeFromWriter for any changes in the WriterProxy whose status is
UNKNOWN and have sequence numbers smaller or equal to ‘last_available_seq_num.’ The status of those changes is
modified from UNKNOWN to MISSING indicating that the changes are available at the WriterHistoryCache of the RTPS
Writer represented by the RTPS WriterProxy but have not been received by the RTPS Reader.

Logical action in the virtual machine:
FOREACH change IN this.changes_from_writer
		SUCH-THAT ( change.status == UNKNOWN
			AND seq_num <= last_available_seq_num ) DO
{
	change.status := MISSING;
}
---------------------------------------------------------------------------------------------------*/
void WriterProxy::MissingChangesUpdate(const SequenceNumber &lastAvailableSeqNum)
{
	for(MapChangeForWriter::iterator it = mapChangesFromWriter_.begin(), it_end = mapChangesFromWriter_.end(); it != it_end; ++it)
	{
		const SequenceNumber &seqNum = it->first;
		if(lastAvailableSeqNum <= seqNum)
		{
			ChangeFromWriter &changeFromWriter = it->second;
			if(changeFromWriter.GetStatus() == ChangeFromWriterStatusKind::UNKNOWN)
			{
				changeFromWriter.SetStatus(ChangeFromWriterStatusKind::MISSING);
			}
		}
		else
		{
			// ASSUMPTION: sequence numbers are in increasing order
			break;
		}
	}
}

/* --------------------------------------------------------------------------------------------------
This operation modifies the status of the ChangeFromWriter that refers to the CacheChange with the
SequenceNumber_t ‘a_seq_num.’ The status of the change is set to ‘RECEIVED,’ indicating it has been received.
Logical action in the virtual machine:

FIND change FROM this.changes_from_writer SUCH-THAT change.sequenceNumber == a_seq_num;
change.status := RECEIVED
---------------------------------------------------------------------------------------------------*/
void WriterProxy::ReceivedChangeSet(const SequenceNumber &seqNum)
{
	if(mapChangesFromWriter_.count(seqNum))
	{
		ChangeFromWriter &changeFromWriter = mapChangesFromWriter_[seqNum];
		changeFromWriter.SetStatus(ChangeFromWriterStatusKind::RECEIVED);
	}
	else
	{
		ICRITICAL() << "Sequence number " << seqNum.GetSequenceNumber() << " not found!";
	}
}

} // namespace RTPS
