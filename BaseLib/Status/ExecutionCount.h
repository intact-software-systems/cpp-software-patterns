#ifndef BaseLib_Status_ExecutionCount_h_Included
#define BaseLib_Status_ExecutionCount_h_Included

#include"BaseLib/Count.h"

#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Templates/Synchronization.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

/**
 * Keeps track of execution counts
 */
class DLL_STATE ExecutionCount
        : public ::BaseLib::Templates::StartMethod
        , public ::BaseLib::Templates::SuccessMethod
        , public ::BaseLib::Templates::FailureMethod
        , public ::BaseLib::Templates::FalseStartMethod
        , public ::BaseLib::Templates::SuspendMethod
        , public ::BaseLib::Templates::ResumeMethod
        , public Templates::LockableType<ExecutionCount>
{
public:
    ExecutionCount();
    virtual ~ExecutionCount();

    CLASS_TRAITS(ExecutionCount)

    Count NumFailures() const;

    Count NumSuccesses() const;
    Count TotalNumAttempts() const;

    Count NumConsecutiveSuccesses() const;
    Count NumConsecutiveFailures() const;

    Count NumFalseStarts() const;

    // --------------------------------------
    // Using interfaces
    // --------------------------------------

    virtual void Start();
    virtual void Suspend();
    virtual void Resume();
    virtual void Success();
    virtual void Failure();
    virtual void FalseStart();
    virtual void Interrupt();

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const ExecutionCount &status)
    {
        ostr << "["
             << status.NumSuccesses()
             << ","
             << status.NumConsecutiveSuccesses()
             << ","
             << status.NumFailures()
             << ","
             << status.NumConsecutiveFailures()
             << ","
             << status.TotalNumAttempts()
             << ","
             << status.NumFalseStarts()
             << "]";

        return ostr;
    }

private:
    /**
     * Includes successful executions
     */
    Count executionCount_;

    /**
     * Includes successful + failed executions
     */
    Count totalExecutionCount_;

    Count numConsecutiveSuccesses_;
    Count numConsecutiveFailures_;

    Count numFalseStarts_;

    Count numSuspended_;

    Count totalInterruptedCount_;
};

}}


#endif
