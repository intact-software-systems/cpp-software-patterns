/*
 * StatelessReader.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateless_StatelessReader_h_IsIncluded
#define RTPS_Stateless_StatelessReader_h_IsIncluded

#include"RTPS/Structure/RtpsReader.h"
#include"RTPS/Export.h"
namespace RTPS
{

class RtpsMessageReceiver;
class RtpsData;

/*-------------------------------------------------------------------------
Specialization of RTPS Reader. The RTPS StatelessReader has no knowledge of the number of matched writers, nor does
it maintain any state for each matched RTPS Writer.

In the current Reference Implementation, the StatelessReader does not add any configuration attributes or operations to
those inherited from the Reader super class. Both classes are therefore identical. The virtual machine interacts with the
StatelessReader using the operations in Table 8.64.
-------------------------------------------------------------------------*/
class DLL_STATE StatelessReader: public RtpsReader
{
public:
	StatelessReader();
	StatelessReader(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
	virtual ~StatelessReader();

public:
	virtual ReaderKind::Type	GetReaderKind()	const { return ReaderKind::Stateless; }

	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsData *data);
	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsGap *gap);
	virtual bool ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsHeartbeat *heartbeat);

private:
	// No state in reference implementation
};

} // namespace RTPS

#endif // RTPS_Stateless_StatelessReader_h_IsIncluded
