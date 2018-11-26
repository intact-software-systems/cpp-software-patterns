#include"RTPS/Machine/VirtualMachine.h"
#include"RTPS/Factory/RtpsFactory.h"

namespace RTPS
{
/*-------------------------------------------------------------------------
			class VirtualMachine
-------------------------------------------------------------------------*/
VirtualMachine::VirtualMachine(const GUID &guid, RtpsMessageSender::Ptr messageSender)
	: endpointGuid_(guid)
	, messageReceiver_(new RtpsMessageReceiver(guid.GetGuidPrefix()))
	, messageSender_(messageSender)
{

}

VirtualMachine::~VirtualMachine()
{

}

/*-------------------------------------------------------------------------
 Parse raw datagram in NetworkLib::SerializeReader
-------------------------------------------------------------------------*/
bool VirtualMachine::ReceiveDatagram(NetworkLib::SerializeReader::Ptr reader, const NetAddress &address)
{
	DataEncapsulation dataEncapsulation;
	dataEncapsulation.Read(reader.get());
	// TODO: Set endian type on NetworkLib::SerializeReader

	// ----------------------------------------------------------
	// 1. Call RptsMessageReceiver.ParseDatagram(reader);
	// ----------------------------------------------------------
	this->messageReceiver_->ParseDatagram(reader, endpointGuid_.GetGuidPrefix());

	// ----------------------------------------------------------
	// 2. Insert list of submessages to virtual reader and writer
	// ----------------------------------------------------------
	RtpsMessageReceiver::ListSubmessages &listSubmessages = this->messageReceiver_->GetListSubmessages();
	for(RtpsMessageReceiver::ListSubmessages::iterator it = listSubmessages.begin(), it_end = listSubmessages.end(); it != it_end; ++it)
	{
		RtpsSubmessageBase::Ptr submessage = *it;

		// ------------------------
		// try to process in reader
		// ------------------------
		bool isProcessed = this->virtualMachineReader_->ReceiveSubmessage(this->messageReceiver_, submessage);

		// ------------------------
		// if not processed
		// ------------------------
		if(isProcessed == false)
		{
			isProcessed = this->virtualMachineWriter_->ReceiveSubmessage(this->messageReceiver_, submessage);
		}

		// -- debug --
		ASSERT(isProcessed);
		// -- debug --
	}

	// TODO: Implement a listener interface to notify when data is received?
	// Listener interface?
	// SOLUTION: Share DataCache between this and RTPS virtual machine
	//		-> Implement WaitSet/WaitCondition or producer/consumer to DataCache and use it in PopMessage

	return true;
}

/*-------------------------------------------------------------------------
 Initialize RTPS Writer and VirtualMachineWriter
-------------------------------------------------------------------------*/
bool VirtualMachine::InitializeWriter(WriterKind::Type writerType
									  , TopicKindId::Type topicKind
									  , ReliabilityKindId::Type reliabilityKind)
{
	RtpsWriter *rtpsWriter = RtpsFactory::NewDefaultRtpsWriter(writerType, endpointGuid_ , topicKind, reliabilityKind);
	ASSERT(rtpsWriter);

	rtpsWriter->SetMessageSender(messageSender_);

	virtualMachineWriter_ = VirtualMachineWriter::Ptr( new VirtualMachineWriter(rtpsWriter));
	if(virtualMachineWriter_->isRunning() == false)
		virtualMachineWriter_->start();

	return true;
}

/*-------------------------------------------------------------------------
 Initialize RTPS Reader and VirtualMachineReader
-------------------------------------------------------------------------*/
bool VirtualMachine::InitializeReader(ReaderKind::Type readerType
									  , TopicKindId::Type topicKind
									  , ReliabilityKindId::Type reliabilityKind)
{
	RtpsReader* rtpsReader = RtpsFactory::NewDefaultRtpsReader(readerType, endpointGuid_, topicKind, reliabilityKind);
	ASSERT(rtpsReader);

	rtpsReader->SetMessageSender(messageSender_);

	virtualMachineReader_ = VirtualMachineReader::Ptr( new VirtualMachineReader(rtpsReader));
	if(virtualMachineReader_->isRunning() == false)
		virtualMachineReader_->start();

	return true;
}

} // namespace RTPS
