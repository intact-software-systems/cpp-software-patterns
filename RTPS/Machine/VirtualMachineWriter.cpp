/*
 * VirtualMachineWriter.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: knuthelv
 */

#include"RTPS/Machine/VirtualMachineWriter.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"
#include"RTPS/Submessages/IncludeLibs.h"
#include"RTPS/Stateful/IncludeLibs.h"
#include"RTPS/Stateless/IncludeLibs.h"

#include <typeinfo>

namespace RTPS
{

/*-------------------------------------------------------------------------
            class VirtualMachineWriter
-------------------------------------------------------------------------*/
VirtualMachineWriter::VirtualMachineWriter(RtpsWriter* rtpsWriter)
    : rtpsWriter_(rtpsWriter)
{
}

VirtualMachineWriter::~VirtualMachineWriter()
{
}

/*-------------------------------------------------------------------------
            main thread
-------------------------------------------------------------------------*/
void VirtualMachineWriter::run()
{
    ASSERT(rtpsWriter_);

    try
    {
        switch(this->rtpsWriter_->GetWriterKind())
        {
			case WriterKind::Stateful:
			{
				//StatefulWriter *statefulWriter = dynamic_cast<StatefulWriter>(this->rtpsWriter_);
                StatefulWriter::Ptr statefulWriter = std::dynamic_pointer_cast<StatefulWriter>(this->rtpsWriter_);

                ASSERT(statefulWriter);

				// -----------------------------------------
				// Main run functions for StatefulWriter
				// -----------------------------------------
				this->run(statefulWriter);

				break;
			}
			case WriterKind::Stateless:
			{
				//StatelessWriter *statelessWriter = dynamic_cast<StatelessWriter*>(this->rtpsWriter_);
                StatelessWriter::Ptr statelessWriter = std::dynamic_pointer_cast<StatelessWriter>(this->rtpsWriter_);

                ASSERT(statelessWriter);

				// -----------------------------------------
				// Main run functions for StatelessWriter
				// -----------------------------------------
				this->run(statelessWriter);

				break;
			}
			default:
				IFATAL() << "Unrecognized WriterKind!";
				break;
		}
    }
    catch(Exception ex)
    {
        ICRITICAL() << "Exception caught: " << ex.msg();
    }
}

/*-------------------------------------------------------------------------
	Called from main thread	-> For StatelessWriter
-------------------------------------------------------------------------*/
void VirtualMachineWriter::run(StatelessWriter::Ptr statelessWriter)
{
    ASSERT(statelessWriter);

    IDEBUG() << "StatelessWriter Started by " << TYPE_NAME(this);

	// FIXME! Don't pass pointer to guard!
	GuardCondition::Ptr newChangeGuard = statelessWriter->GetNewChangeGuard();
    newChangeGuard->SetHandler<StatelessWriter>(statelessWriter.get());

    DCPS::ReturnCode::Type state = DCPS::ReturnCode::Ok;

    while(state == DCPS::ReturnCode::Ok)
    {
        state = statelessWriter->WaitDispatch(1000);
    }

    IWARNING() << "Stateless Writer Stopped!";
}

/*-------------------------------------------------------------------------
	Called from main thread	-> For StatefulWriter
-------------------------------------------------------------------------*/
void VirtualMachineWriter::run(StatefulWriter::Ptr statefulWriter)
{
	ASSERT(statefulWriter);

    IDEBUG() << "StatefulWriter Started by " << TYPE_NAME(this);

    // FIXME! Don't pass pointer to guard!
	GuardCondition::Ptr newChangeGuard = statefulWriter->GetNewChangeGuard();
    newChangeGuard->SetHandler<StatefulWriter>(statefulWriter.get());

	DCPS::ReturnCode::Type state = DCPS::ReturnCode::Ok;

	while(state == DCPS::ReturnCode::Ok)
	{
		state = statefulWriter->WaitDispatch(1000);
	}

	IWARNING() << "StatefulWriter Stopped!";
}

/*-------------------------------------------------------------------------------
Submessages that flow from Reader to Writer.

-- AckNack --

This Submessage is used to communicate the state of a Reader to a Writer.
The Submessage allows the Reader to inform the Writer about the sequence
numbers it has received and which ones it is still missing. This Submessage
can be used to do both positive and negative acknowledgments.

This transition is triggered by the reception of an ACKNACK message destined to the RTPS StatelessWriter
‘the_rtps_writer’ originating from some RTPS Reader.

The transition performs the following logical actions in the virtual machine:

FOREACH reply_locator_t IN { Receiver.unicastReplyLocatorList, Receiver.multicastReplyLocatorList }
	reader_locator := the_rtps_writer.reader_locator_lookup(reply_locator_t);
	reader_locator.requested_changes_set(ACKNACK.readerSNState.set);

Note that the processing of this message uses the reply locators in the RTPS Receiver. This is the only source of
information for the StatelessWriter to determine where to send the reply to. Proper functioning of the protocol requires
that the RTPS Reader inserts an InfoReply Submessage ahead of the AckNack such that these fields are properly set.


-- NackFrag --

The NackFrag Submessage is used to communicate the state of a Reader to a Writer.
When a data change is sent as a series of fragments, the NackFrag Submessage
allows the Reader to inform the Writer about specific fragment numbers
it is still missing.
-------------------------------------------------------------------------------*/
bool VirtualMachineWriter::ReceiveSubmessage(RtpsMessageReceiver::Ptr &messageReceiver, RtpsSubmessageBase::Ptr &submessage)
{
	if(!submessage)
	{
		ICRITICAL() << "Submessage == NULL!";
		return false;
	}

	bool retValue = true;

	switch(submessage->GetSubmessageKind())
	{
		case SubmessageKind::ACKNACK:
		{
			RtpsAckNack *ackNack = dynamic_cast<RtpsAckNack*>(submessage.get());
			ASSERT(ackNack);

			retValue = rtpsWriter_->ReceiveSubmessage(messageReceiver.get(), ackNack);
			break;
		}
		case SubmessageKind::NACK_FRAG:
		{
			RtpsNackFrag *nackFrag = dynamic_cast<RtpsNackFrag*>(submessage.get());
			ASSERT(nackFrag);

			retValue = rtpsWriter_->ReceiveSubmessage(messageReceiver.get(), nackFrag);
			break;
		}
		case SubmessageKind::DATA:
		case SubmessageKind::DATA_FRAG:
		case SubmessageKind::GAP:
		case SubmessageKind::HEARTBEAT:
		case SubmessageKind::HEARTBEAT_FRAG:
		{
			IWARNING() << "Submessage " << (char)submessage->GetSubmessageKind() << " is not supported by the Writer!";
			retValue = false;
			break;
		}
		default:
			ICRITICAL() << "Unknown entity SubmessageKind: " << (char)submessage->GetSubmessageKind();
			retValue = false;
			break;
	}

	return retValue;
}

} // namespace RTPS
