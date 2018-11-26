#ifndef BaseLib_Status_ExecutionTime_h_Included
#define BaseLib_Status_ExecutionTime_h_Included

#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Templates/Synchronization.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

/**
 * Keeps track of execution times
 *
 * TODO: Accessed by multiple threads. Review needed. Thread safety needed?
 */
class DLL_STATE ExecutionTime
        : public ::BaseLib::Templates::StartMethod
        , public ::BaseLib::Templates::SuccessMethod
        , public ::BaseLib::Templates::FailureMethod
        , public ::BaseLib::Templates::FalseStartMethod
        , public ::BaseLib::Templates::SuspendMethod
        , public ::BaseLib::Templates::ResumeMethod
        , public Templates::LockableType<ExecutionTime>
{
public:
    ExecutionTime();
    virtual ~ExecutionTime();

    CLASS_TRAITS(ExecutionTime)

    // --------------------------------------
    // Access time
    // --------------------------------------

    Timestamp LastSuccessfulExecutionTime() const;
    Timestamp LastFailedExecutionTime() const;
    Timestamp CurrentExecutionTime() const;

    Duration TimeSinceLastExecutionTime() const;
    Duration TimeSinceLastFalseStart() const;

    // --------------------------------------
    // Access booleans
    // --------------------------------------

    bool DidLastExecutionFail() const;

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

    friend std::ostream &operator<<(std::ostream &ostr, const ExecutionTime &status)
    {
        ostr << "["
             << status.LastSuccessfulExecutionTime()
             << ","
             << status.LastFailedExecutionTime()
             << ","
             << status.TimeSinceLastExecutionTime()
             << ","
             << status.CurrentExecutionTime()
             << "]";

        return ostr;
    }

private:
    /**
      * Last successful execution time point
      */
    Timestamp lastExecutionTime_;

    /**
      * Last failed execution time point
      */
    Timestamp lastFailedExecutionTime_;

    /**
      * Current execution start time point
      */
    Timestamp currentExecutionTime_;

    /**
     * Last false start
     */
    Timestamp lastFalseStart_;

    /**
     * Last time suspended
     */
    Timestamp suspendTime_;

    /**
     * Last time interrupted
     */
    Timestamp interruptedTime_;
};

}}

#endif
