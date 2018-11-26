#pragma once

#include"BaseLib/Exception.h"
#include"BaseLib/Runnable.h"

#include"BaseLib/Policy/IncludeLibs.h"
#include"BaseLib/Templates/StrategyMethods.h"
#include"BaseLib/Templates/ActionHolder.h"
#include"BaseLib/Status/ExecutionStatus.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Strategy {

struct DLL_STATE DefaultRunnableExecution
{
    static bool Perform(Runnable* runnable, Status::ExecutionStatus& status);
};

template <typename Return>
struct DefaultActionExecution
{
    static Return Perform(Templates::Action0<Return>& action, Status::ExecutionStatus& status)
    {
        Return value;

        try
        {
            action.Entry();

            status.Start();

            value = action.Invoke();

            status.Success();
        }
        catch(BaseLib::GeneralException e)
        {
            IWARNING() << "Caught exception " << e;

            status.Failure();
        }
        catch(std::runtime_error error)
        {
            IWARNING() << "Caught exception " << error.what();

            status.Failure();
        }
        catch(...)
        {
            ICRITICAL() << "Caught undefined exception!";

            status.Failure();
        }

        action.Exit();

        return value;
    }
};

template <typename Return, typename Action>
struct RunnableFutureExecution
{
    static Return Perform(Action& action)
    {
        Return result;
        try
        {
            action.Entry();

            result = action.Invoke();

            action.Exit();

            action.Next(result);
            action.Complete();

            MutexTypeLocker<Action> lock(&action);
            action.wakeAll();
        }
        catch(GeneralException e)
        {
            action.Exit();
            action.Error(e);

            MutexTypeLocker<Action> lock(&action);
            action.wakeAll();

            throw e;
        }
        catch(...)
        {
            action.Exit();
            action.Error(Exception("Unhandled exception caught"));

            MutexTypeLocker<Action> lock(&action);
            action.wakeAll();
        }

        return result;
    }
};

}}
