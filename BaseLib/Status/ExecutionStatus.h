#pragma once

#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Templates/Synchronization.h"

#include"BaseLib/Status/ExecutionCount.h"
#include"BaseLib/Status/ExecutionTime.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

namespace ExecutionStatusKind
{
    enum Type
    {
        /**
         * If command has not been touched in any way.
         */
        NO,

        /**
          * If command is currently about to be executed.
          */
        STARTING,

        /**
         * If command is currently being executed.
         */
        STARTED,

        /**
         * If command is currently suspended
         */
        SUSPENDED,

        /**
         * If command has been suspended and then now running
         */
        RESUMED,

        /**
         * if command has just been interrupted
         */
        INTERRUPTED,

        /**
         * If command was cancelled during execution.
         */
        CANCELLED,

        /**
         * If command was cancelled during execution.
         */
        FAILURE,

        /**
         * If command was successfully executed.
         */
        STOPPED
    };
}

DLL_STATE std::ostream& operator<<(std::ostream& ostr, const ExecutionStatusKind::Type& kind);

/**
 * @brief The ExecutionStatus class
 *
 * TODO: isSuccess doesn't make sense if policy indicates multiple executions.
 */
class DLL_STATE ExecutionStatus
        : public BaseLib::Templates::IsExecutingMethod
        , public BaseLib::Templates::IsSuccessMethod
        , public BaseLib::Templates::IsCancelledMethod
        , public BaseLib::Templates::IsInterruptedMethod
        , public BaseLib::Templates::IsStartedMethod
        , public BaseLib::Templates::IsStartingMethod
        , public BaseLib::Templates::IsSuspendedMethod
        , public BaseLib::Templates::IsResumedMethod
        , public BaseLib::Templates::IsFailedMethod
        , public BaseLib::Templates::IsDoneMethod
        , public BaseLib::Templates::StartMethod
        , public BaseLib::Templates::SuspendMethod
        , public BaseLib::Templates::ResumeMethod
        , public BaseLib::Templates::SuccessMethod
        , public BaseLib::Templates::FailureMethod
        , public BaseLib::Templates::CancelMethod
        , public BaseLib::Templates::InterruptMethod
        , public BaseLib::Templates::FalseStartMethod
        , public Templates::LockableType<ExecutionStatus>
{
public:
    ExecutionStatus();
    virtual ~ExecutionStatus();

    CLASS_TRAITS(ExecutionStatus)

    // ------------------------------------------------------
    // Simple getters
    // ------------------------------------------------------

    const ExecutionCount& Count() const;
    const ExecutionTime& Time() const;

    ExecutionStatusKind::Type State() const;

    void Reset();

    // ------------------------------------------------------
    // Using interfaces: Check status
    // ------------------------------------------------------

    virtual bool IsExecuting() const;
    virtual bool IsStarting() const;
    virtual bool IsStarted() const;
    virtual bool IsSuccess() const;
    virtual bool IsCancelled() const;
    virtual bool IsInterrupted() const;
    virtual bool IsEverStarted() const;
    virtual bool IsSuspended() const;
    virtual bool IsResumed() const;
    virtual bool IsFailed() const;
    virtual bool IsDone() const;

    bool IsLastExecutionFailure() const;
    bool IsLastExecutionSuccess() const;
    bool IsNeverExecuted() const;

    bool IsFalseStart() const;

    // ------------------------------------------------------
    // Using interfaces: Called when execution starts and stops
    // ------------------------------------------------------

    void Starting();
    bool SetIffNotExecuting(ExecutionStatusKind::Type kind);

    virtual void Start();
    virtual void Suspend();
    virtual void Resume();
    virtual void Success();
    virtual void Failure();
    virtual bool Cancel();
    virtual bool Interrupt();
    virtual void FalseStart();

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const ExecutionStatus &status)
    {
        ostr << "["
             << status.State()
             << ", "
             << status.Count()
             << ", "
             << status.Time()
             << "]";

        return ostr;
    }

private:

    bool isExecuting() const;

private:
    /**
     * Counts executions
     */
    ExecutionCount executionCount_;

    /**
     * Tracks execution times
     */
    ExecutionTime executionTime_;

    /**
     * Represents the state of execution
     */
    ExecutionStatusKind::Type state_;
};

}}
