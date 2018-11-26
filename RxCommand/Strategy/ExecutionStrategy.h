#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/Export.h"

namespace Reactor {

struct DLL_STATE ExecutionStrategy
{
    // --------------------------------------------------------
    // TODO: throw(FatalException, CancelCommandException)
    // --------------------------------------------------------

    template <typename Return>
    static Return CallAction(
        KeyValueRxEvents<Templates::Action0<Return>*, Return>* command,
        Templates::Action0<Return>* action,
        Status::ExecutionStatus& status
    )
    {
        Return           value;
        GeneralException actionException("Dummy");
        bool             isError = false;

        // -----------------------------------------
        // Update status and call action
        // -----------------------------------------

        try
        {
            status.Start();

            action->Entry();

            value = action->Invoke();
        }
        catch(GeneralException e)
        {
            actionException = e;
            isError         = true;
        }
        catch(...)
        {
            isError = true;
        }

        action->Exit();

        // -----------------------------------------
        // Callbacks and status update after action execution
        // -----------------------------------------

        if(isError)
        {
            // -----------------------------------------
            // Action threw exception
            // -----------------------------------------

            command->OnError(action, actionException);

            if(Utility::instanceof<CancelException, GeneralException>(&actionException))
            {
                status.Cancel();
                throw CancelException(actionException.msg());
            }
            else if(Utility::instanceof<FatalException, GeneralException>(&actionException))
            {
                status.Cancel();
                throw FatalException(actionException.msg());
            }
            else
            {
                status.Failure();
            }
        }
        else
        {
            // -----------------------------------------
            // Action completed without throwing exception
            // -----------------------------------------

            command->OnNext(action, value);
            status.Success();
        }

        return value;
    }

    template <typename Return>
    static Concurrent::ScheduledFuture<Return> CallAsyncAction(Templates::Action0<Concurrent::ScheduledFuture<Return>>* action, Status::ExecutionStatus& status)
    {
        Concurrent::ScheduledFuture<Return> value(nullptr);
        GeneralException                    actionException("Dummy");
        bool                                isError = false;

        // -----------------------------------------
        // Update status and call action
        // -----------------------------------------

        try
        {
            status.Start();

            action->Entry();

            value = action->Invoke();
        }
        catch(GeneralException e)
        {
            actionException = e;
            isError         = true;
        }
        catch(...)
        {
            isError = true;
        }

        action->Exit();

        // -----------------------------------------
        // Callbacks and status update after action execution
        // -----------------------------------------

        if(isError)
        {
            // -----------------------------------------
            // Action threw exception
            // -----------------------------------------

            //command->OnError(action, actionException);

            if(Utility::instanceof<CancelException, GeneralException>(&actionException))
            {
                status.Cancel();
                throw CancelException(actionException.msg());
            }
            else if(Utility::instanceof<FatalException, GeneralException>(&actionException))
            {
                status.Cancel();
                throw FatalException(actionException.msg());
            }
            else
            {
                status.Failure();
            }
        }
        else
        {
            // -----------------------------------------
            // Action completed without throwing exception
            // -----------------------------------------

            //command->OnNext(action, value);
            status.Success();
        }

        return value;
    }
};

}
