/*
 * WriterProxy.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateful_WriterProxy_h_IsIncluded
#define RTPS_Stateful_WriterProxy_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Stateful/ChangeFromWriter.h"
#include"RTPS/Export.h"
namespace RTPS
{
/* --------------------------------------------------------------------------------------------------
The RTPS WriterProxy represents the information an RTPS StatefulReader maintains on each matched RTPS Writer. The
attributes of the RTPS WriterProxy are described in Table 8.67.
The association is a consequence of the matching of the corresponding DDS Entities as defined by the DDS specification,
that is the DDS DataReader matching a DDS DataWriter by Topic, having compatible QoS, belonging to a common
partition, and not being explicitly ignored by the application that uses DDS.

The newly-created writer proxy ‘this’ is initialized as follows:

this.attributes := <as specified in the constructor>;
this.changes_from_writer := <all past and future samples from the writer>;

The changes_from_writer of the newly-created WriterProxy is initialized to contain all past and future samples from the
Writer represented by the WriterProxy. This is a conceptual representation only, used to describe the Stateful Reference
Implementation. The ChangeFromWriter status of each CacheChange in changes_from_writer is initialized to
UNKNOWN, indicating the StatefulReader initially does not know whether any of these changes actually already exist.
As discussed in Section 8.4.12.3, the status will change to RECEIVED or MISSING as the StatefulReader receives the
actual changes or is informed about their existence via a HEARTBEAT message.
--------------------------------------------------------------------------------------------------*/
class DLL_STATE WriterProxy
{
public:
    typedef std::map<SequenceNumber, ChangeFromWriter>	MapChangeForWriter;

public:
    WriterProxy();
    WriterProxy(const GUID &guid);
    virtual ~WriterProxy();

    CLASS_TRAITS(WriterProxy)

public:
    // from RTPS specifications
    SequenceNumber	AvailableChangesMax();
    void			IrrelevantChangeSet(const SequenceNumber &sequenceNumber);
    void			LostChangesUpdate(const SequenceNumber &firstAvailableSeqNum);
    SequenceSet		MissingChanges() const;
    void			MissingChangesUpdate(const SequenceNumber &lastAvailableSeqNum);
    void			ReceivedChangeSet(const SequenceNumber &aSeqNum);

public:
    // Own functions
    bool HasMissingChanges() const;

public:
    // -- Simple Get/Set Functions --
    const GUID&			GetWriterGUID()				const { return remoteWriterGuid_; }
    const LocatorList& 	GetUnicastLocatorList() 	const { return unicastLocatorList_; }
    const LocatorList& 	GetMulticastLocatorList() 	const { return multicastLocatorList_; }

    void				SetWriterGUID(const GUID &guid)						{ remoteWriterGuid_ = guid; }
    void				SetUnicastLocatorList(const LocatorList &uni)		{ unicastLocatorList_ = uni; }
    void				SetMulticastLocatorList(const LocatorList &multi) 	{ multicastLocatorList_ = multi; }

private:
    GUID					remoteWriterGuid_; 		// Identifies the remote matched RTPS Writer that is represented by the WriterProxy.
    LocatorList				unicastLocatorList_;
    LocatorList				multicastLocatorList_;
    MapChangeForWriter		mapChangesFromWriter_; 	// List of CacheChange changes as they relate to the matched RTPS Writer.
};

} // namespace RTPS

#endif // RTPS_Stateful_ReaderProxy_h_IsIncluded
