#include"BaseLib/Strategy/ExecutionStrategy.h"

namespace BaseLib { namespace Strategy {

bool DefaultRunnableExecution::Perform(Runnable* runnable, Status::ExecutionStatus& status)
{
    ASSERT(runnable != nullptr);

    try
    {
        status.Start();
        runnable->run();
        status.Success();

        return true;
    }
    catch(BaseLib::GeneralException e)
    {
        IWARNING() << "Caught exception " << e;

        status.Failure();
    }
    catch(...)
    {
        ICRITICAL() << "Caught undefined exception!";

        status.Failure();
    }

    return false;
}

}}
