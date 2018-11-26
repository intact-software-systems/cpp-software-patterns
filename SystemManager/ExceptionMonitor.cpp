#include"SystemManager/ExceptionMonitor.h"

namespace SystemManagerSpace
{
// -------------------------------------------------------
//	constructor/destructor ExceptionMonitor
//		- Constructor: Initializes the ExceptionMonitor
// -------------------------------------------------------
ExceptionMonitor::ExceptionMonitor(string name) 
	: Thread(name)
	, exceptionSubscriber(NULL)
	, stopExceptionMonitor(false)
{
	exceptionSubscriber = new ExceptionMulticastSubscriber(ExceptionMulticast::GetMulticastHandle(), this, true);
}

ExceptionMonitor::~ExceptionMonitor()
{ 
	stopExceptionMonitor = true;
	
	// -> Is selfdestructing! 
	//if(exceptionSubscriber != NULL)
	//{
	//	exceptionSubscriber->Shutdown();
	//	delete exceptionSubscriber;
	//	exceptionSubscriber = NULL;
	//}
}

// NB! Thread is never started
// TODO: Anything to do in this thread?
void ExceptionMonitor::run()
{
	while(!stopExceptionMonitor)
	{
		msleep(1000);
		if(stopExceptionMonitor) break;
	}
}

void ExceptionMonitor::PrintAll(std::ostream &ostr)
{
	MutexLocker lock(&updateLock);

	ostr << "---------------------------- Exceptions Received ----------------------------" << endl;
	for(size_t i = 0; i < vectorTimeStampExceptionMessage.size(); i++)
	{
		ostr << vectorTimeStampExceptionMessage[i] << endl;
	}
}

void ExceptionMonitor::ClearAll()
{
	MutexLocker lock(&updateLock);
	vectorTimeStampExceptionMessage.clear();
}

void ExceptionMonitor::PostException(ExceptionMessage &exceptionMessage)
{
	MutexLocker lock(&updateLock);
	vectorTimeStampExceptionMessage.push_back(exceptionMessage);
}

map<long long, ExceptionMessage> ExceptionMonitor::GetMiddlewareExceptions(long long &lastTimestamp)
{
	MutexLocker lock(&updateLock);

	if(lastTimestamp < 0) lastTimestamp = 0;
	if(lastTimestamp > vectorTimeStampExceptionMessage.size()) // fix silently
	{
		lastTimestamp = vectorTimeStampExceptionMessage.size();
		//cout << "ExceptionMonitor::GetMiddlewareExceptions(" << lastTimestamp << "): WARNING! timestamp is too large! Current timestamp: " << vectorTimeStampExceptionMessage.size() << endl;
	}

	MapTimeStampExceptionMessage mapTimestampExceptionMessage;
	for( ; lastTimestamp < vectorTimeStampExceptionMessage.size(); lastTimestamp++)
	{
		ExceptionMessage &ex = vectorTimeStampExceptionMessage[(unsigned int)lastTimestamp];
		mapTimestampExceptionMessage.insert(pair<long long, ExceptionMessage>(lastTimestamp, ex));
	}

	return mapTimestampExceptionMessage;
}


} // namespace SystemManagerSpace

