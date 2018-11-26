/*
 * ReaderProxy.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateful_ReaderProxy_h_IsIncluded
#define RTPS_Stateful_ReaderProxy_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Stateful/ChangeForReader.h"
#include"RTPS/Structure/RtpsReaderProxyBase.h"
#include"RTPS/Export.h"
namespace RTPS
{
/* --------------------------------------------------------------------------------------------------
Operation: new ReaderProxy

This operation creates a new RTPS ReaderProxy. The newly-created reader proxy ‘this’ is initialized as follows:

this.attributes := <as specified in the constructor>;
this.changes_for_reader := RTPS::Writer.writer_cache.changes;
FOR_EACH change IN (this.changes_for_reader) DO
{
    IF ( DDS_FILTER(this, change) THEN change.is_relevant := FALSE;
    ELSE change.is_relevant := TRUE;
    IF ( RTPS::Writer.pushMode == true) THEN change.status := UNSENT;
    ELSE change.status := UNACKNOWLEDGED;
}

The above logic indicates that the newly-created ReaderProxy initializes its set of ‘changes_for_reader’ to contain all the
CacheChanges in the Writer’s HistoryCache.
The change is marked as ‘irrelevant’ if the application of any of the DDS-DataReader filters indicates the change is not
relevant to that particular reader. The DDS specification indicates that a DataReader may provide a time-based filter as
well as a content-based filter. These filters should be applied in a manner consistent with the DDS specification to select
any changes that are irrelevant to the DataReader.
The status is set depending on the value of the RTPS Writer attribute ‘pushMode.’
 --------------------------------------------------------------------------------------------------*/
class DLL_STATE ReaderProxy : public RtpsReaderProxyBase
{
public:
    typedef std::map<SequenceNumber, ChangeForReader::Ptr>	MapChangeForReader;

public:
    ReaderProxy(bool expectsInlineQos = false);
    ReaderProxy(const GUID &guid, bool expectsInlineQos = false);
    virtual ~ReaderProxy();

    CLASS_TRAITS(ReaderProxy)

public:
    // -- Core Functions --
    void RequestedChangesSet(const SequenceNumberSet &reqSeqNumSet);
    void AckedChangesSet(const SequenceNumber &committedSeqNum);

    bool NextRequestedChange(ChangeForReader::Ptr &changeForReader);
    bool NextUnsentChange(ChangeForReader::Ptr &changeForReader);
    bool RequestedChanges(MapChangeForReader &requestedChanges);
    bool UnsentChanges(MapChangeForReader &unsentChanges) const;
    bool UnackedChanges(MapChangeForReader &unackedChanges);

public:
    // Own implementation
    bool IsAckedChange(const SequenceNumber &ackedSeq) const;
    bool HasUnsentChanges() const;
    bool HasUnackedChanges() const;
    bool HasRequestedChanges() const;
    bool AddChangeForReader(const SequenceNumber &seqNum, const ChangeForReader::Ptr &changeForReader);

public:
    // -- Simple Get/Set Functions --
    bool IsActive()				const { return isActive_; }
    bool ExpectsInlineQos()		const { return expectsInlineQos_; }

    void SetActive(bool active)				{ isActive_ = active; }
    void SetExcpectsInlineQos(bool qos)		{ expectsInlineQos_ = qos; }

public:
    // -- Simple Get/Set Functions --
    const GUID&			GetReaderGUID()				const { return remoteReaderGuid_; }
    const LocatorList& 	GetUnicastLocatorList() 	const { return unicastLocatorList_; }
    const LocatorList& 	GetMulticastLocatorList() 	const { return multicastLocatorList_; }

    void				SetUnicastLocatorList(const LocatorList &uni)		{ unicastLocatorList_ = uni; }
    void				SetMulticastLocatorList(const LocatorList &multi) 	{ multicastLocatorList_ = multi; }

private:
    bool			    findChanges(MapChangeForReader &changes, ChangeForReaderStatusKind::Type type) const;
    bool			    findChange(ChangeForReader::Ptr &change, ChangeForReaderStatusKind::Type type) const;

    bool                hasChangeState(ChangeForReaderStatusKind::Type changeForReaderKind) const;

private:
    GUID			    remoteReaderGuid_; 			//Identifies the remote matched RTPS Reader that is represented by the ReaderProxy.
    bool			    expectsInlineQos_; 			// = false. Specifies whether the remote matched RTPS Reader expects in-line QoS to be sent along with any data.
    bool			    isActive_;					// Specifies whether the remote Reader is responsive to the Writer.
    LocatorList		    unicastLocatorList_;
    LocatorList		    multicastLocatorList_;

private:
    MapChangeForReader	mapChangesForReader_;       // List of CacheChange changes as they relate to the matched RTPS Reader.
};

}

#endif
