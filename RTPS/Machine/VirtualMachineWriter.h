/*
 * VirtualMachineWriter.h
 *
 *  Created on: Apr 7, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Machine_VirtualMachineWriter_h_IsIncluded
#define RTPS_Machine_VirtualMachineWriter_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"

#include"RTPS/Export.h"

namespace RTPS
{

class RtpsWriter;
class StatelessWriter;
class StatefulWriter;

FORWARD_CLASS_TRAITS(RtpsWriter)

FORWARD_CLASS_TRAITS(StatelessWriter)

FORWARD_CLASS_TRAITS(StatefulWriter)

/*-------------------------------------------------------------------------
			class VirtualMachineWriter
-------------------------------------------------------------------------*/
class DLL_STATE VirtualMachineWriter : public BaseLib::Thread
{
public:
	VirtualMachineWriter(RtpsWriter* rtpsWriter);
	virtual ~VirtualMachineWriter();

    CLASS_TRAITS(VirtualMachineWriter)

	bool ReceiveSubmessage(RtpsMessageReceiver::Ptr &messageReceiver, RtpsSubmessageBase::Ptr &submessage);

public:
	RtpsWriterPtr     GetWriter() const { return rtpsWriter_; }

private:
	virtual void run();

private:
	void run(StatelessWriterPtr statelessWriter);
	void run(StatefulWriterPtr statefulWriter);

private:
	RtpsWriterPtr		rtpsWriter_;
};

} // namespace RTPS

#endif // RTPS_Machine_VirtualMachineWriter_h_IsIncluded
