#include "StdThread.h"

namespace BaseLib
{

StdThread::StdThread(const std::string& name) : Thread(name)
{ }

StdThread::StdThread(const std::string& name, bool debug) : Thread(name, debug)
{ }

StdThread::~StdThread()
{ }

bool StdThread::start(ThreadDetails::Priority priority)
{
    thread_ = std::make_shared<std::thread>( [this] () { run(); } );

    std::thread::id id;


    getThreadDetails().threadState(ThreadDetails::RunningState);
    //getThreadDetails().threadId(thread_->get_id());
    getThreadDetails().threadRunning(true);
    getThreadDetails().threadPriority(priority);

    return Thread::start(priority);
}

}
