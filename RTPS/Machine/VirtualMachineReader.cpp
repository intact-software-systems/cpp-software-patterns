/*
 * VirtualMachineReader.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: knuthelv
 */

#include"RTPS/Machine/VirtualMachineReader.h"
#include"RTPS/Structure/IncludeLibs.h"
#include"RTPS/Stateful/IncludeLibs.h"
#include"RTPS/Stateless/IncludeLibs.h"

namespace RTPS
{
/*-------------------------------------------------------------------------
			class VirtualMachineReader

    TODO:
    - Use a private class.
    - Initialize by copying what is in run() to Initialize()
    -
-------------------------------------------------------------------------*/
VirtualMachineReader::VirtualMachineReader(RtpsReader* rtpsReader)
	: rtpsReader_(rtpsReader)
{

}

VirtualMachineReader::~VirtualMachineReader()
{

}

/*-------------------------------------------------------------------------
			main thread
-------------------------------------------------------------------------*/
void VirtualMachineReader::run()
{
	try
	{
		switch(this->rtpsReader_->GetReaderKind())
		{
			case ReaderKind::Stateful:
			{
				StatefulReader *statefulReader = dynamic_cast<StatefulReader*>(this->rtpsReader_);
				ASSERT(statefulReader);

				// -----------------------------------------
				// Main run functions for StatefulReader
				// -----------------------------------------
				this->run(statefulReader);

				break;
			}
			case ReaderKind::Stateless:
			{
				StatelessReader *statelessReader = dynamic_cast<StatelessReader*>(this->rtpsReader_);
				ASSERT(statelessReader);

				// -----------------------------------------
				// Main run functions for StatelessReader
				// -----------------------------------------
				this->run(statelessReader);

				break;
			}
			default:
				IFATAL() << "Unrecognized ReaderKind!";
				break;
		}
	}
	catch(Exception ex)
	{
		ICRITICAL() << "Reader stopped! Exception: " << ex.msg();
	}
}

/*-------------------------------------------------------------------------
	Called from main thread	-> For StatelessReader
-------------------------------------------------------------------------*/
void VirtualMachineReader::run(StatelessReader *statelessReader)
{
	ASSERT(statelessReader);

	IDEBUG() << "StatelessReader Started by " << typeid(this).name();

	DCPS::ReturnCode::Type state = DCPS::ReturnCode::Ok;

	while(state == DCPS::ReturnCode::Ok)
	{
		Thread::sleep(2);
		//state = statelessReader->WaitDispatch(1000);
	}
}

/*-------------------------------------------------------------------------
	Called from main thread	-> For StatefulReader
-------------------------------------------------------------------------*/
void VirtualMachineReader::run(StatefulReader *statefulReader)
{
	ASSERT(statefulReader);

	IDEBUG() << "StatefulReader Started by " << typeid(this).name();

	DCPS::ReturnCode::Type state = DCPS::ReturnCode::Ok;

	while(state == DCPS::ReturnCode::Ok)
	{
		state = statefulReader->WaitDispatch(1000);
	}
}

/*-------------------------------------------------------------------------
Submessages that flow from Writer to Reader
-------------------------------------------------------------------------*/
bool VirtualMachineReader::ReceiveSubmessage(RtpsMessageReceiver::Ptr &messageReceiver, RtpsSubmessageBase::Ptr &submessage)
{
	if(!submessage)
	{
		ICRITICAL() << "Submessage == NULL!";
		return false;
	}

	bool retValue = true;

	switch(submessage->GetSubmessageKind())
	{
		case SubmessageKind::DATA:
		{
			RtpsData *data = dynamic_cast<RtpsData*>(submessage.get());
			ASSERT(data);

			retValue = rtpsReader_->ReceiveSubmessage(messageReceiver.get(), data);
			break;
		}
		case SubmessageKind::DATA_FRAG:
		{
			RtpsDataFrag *dataFrag = dynamic_cast<RtpsDataFrag*>(submessage.get());
			ASSERT(dataFrag);

			//retValue = rtpsReader_->ReceiveSubmessage(messageReceiver.get(), dataFrag);
			break;
		}
		case SubmessageKind::HEARTBEAT:
		{
			RtpsHeartbeat *heartbeat = dynamic_cast<RtpsHeartbeat*>(submessage.get());
			ASSERT(heartbeat);

			retValue = rtpsReader_->ReceiveSubmessage(messageReceiver.get(), heartbeat);
			break;
		}
		case SubmessageKind::HEARTBEAT_FRAG:
		{
			RtpsHeartbeatFrag *heartbeatFrag = dynamic_cast<RtpsHeartbeatFrag*>(submessage.get());
			ASSERT(heartbeatFrag);

			//retValue = rtpsReader_->ReceiveSubmessage(messageReceiver.get(), heartbeatFrag);
			break;
		}
		case SubmessageKind::GAP:
		{
			RtpsGap *gap = dynamic_cast<RtpsGap*>(submessage.get());
			ASSERT(gap);

			retValue = rtpsReader_->ReceiveSubmessage(messageReceiver.get(), gap);
			break;
		}
		case SubmessageKind::ACKNACK:
		case SubmessageKind::NACK_FRAG:
		{
			IWARNING() << "Submessage " << (char)submessage->GetSubmessageKind() << " is not supported by the Reader!";
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
