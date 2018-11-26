#pragma once

#include <thread>
#include "Thread.h"
#include "BaseLib/Export.h"

namespace BaseLib
{

class DLL_STATE StdThread : public Thread
{
public:
    StdThread(const std::string& name);
    StdThread(const std::string& name, bool debug);
    virtual ~StdThread();

    virtual bool start(ThreadDetails::Priority priority = ThreadDetails::InheritPriority);

private:
    std::shared_ptr<std::thread> thread_;
};

}


