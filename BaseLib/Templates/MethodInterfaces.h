#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Templates
{

// -------------------------------------------------------------------
// TODO:
// - Important: Commands are not events. Commands may lead to generation of events.
// - Define all events to be identifiable as events, i.e., "Something" using keys, enums, strings, etc
//      - Autogenerate using class Event<Data> ?
// - Define one interface with one method for every event, i.e., class SomethingMethod { virtual void Something(); };
// - Define one reactive/observer method for every event, i.e., class SomethingObserver { virtual void OnSomething(); }
// - Implement one subject part for every event, i.e., class SomethingSubject { virtual void OnSomething() { somethingSignaller_.Signal(); }
// - Implement one actor/handler for every event, i.e., process incoming events
// - Implement strategies to process events, event-pairs, etc
// - Connect command chains of actors/handlers with conditional triggers
// -------------------------------------------------------------------

/**
  * NOTE!
  * - Method interfaces should not be used directly but rather subclassed.
  * - If CLASS_TRAITS is used then virtual destructors ARE necessary
  * - Only declare virtual destructor if methods can be deleted by client.
  * - Possibly declare pure virtual destructor
  *
  *
  * If object is to be a base class for all objects then inheritance must be virtual to avoid "diamond inheritance"
  *
  * AMethod : public virtual Object
  */
class Object
{
public:
    virtual ~Object() {}
};

// --------------------------------------------
// Copy methods
// --------------------------------------------

template <typename Return>
class CopyMethod0
{
public:
    virtual ~CopyMethod0() {}

    virtual Return Copy() const = 0;
};

template <typename T, typename Arg1>
class CopyMethod1
{
public:
    virtual ~CopyMethod1() {}

    virtual T Copy(Arg1) const = 0;
};

template <typename T, typename Arg1, typename Arg2>
class CopyMethod2
{
public:
    virtual ~CopyMethod2() {}

    virtual T Copy(Arg1, Arg2) const = 0;
};

// --------------------------------------------
// Factory methods
// --------------------------------------------

template <typename Return>
class CreateMethod0
{
public:
    virtual ~CreateMethod0() {}

    virtual Return Create() = 0;
};

template <typename Return, typename Arg1>
class CreateMethod1
{
public:
    virtual ~CreateMethod1() {}

    virtual Return Create(Arg1) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class CreateMethod2
{
public:
    virtual ~CreateMethod2() {}

    virtual Return Create(Arg1, Arg2) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class CreateMethod3
{
public:
    virtual ~CreateMethod3() {}

    virtual Return Create(Arg1, Arg2, Arg3) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class CreateMethod4
{
public:
    virtual ~CreateMethod4() {}

    virtual Return Create(Arg1, Arg2, Arg3, Arg4) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class CreateMethod5
{
public:
    virtual ~CreateMethod5() {}

    virtual Return Create(Arg1, Arg2, Arg3, Arg4, Arg5) = 0;
};

class DLL_STATE IsCreatedMethod
{
public:
    virtual ~IsCreatedMethod() {}

    virtual bool IsCreated() const = 0;
};

// ----------------------------------------
// Object access methods
// ----------------------------------------

class DLL_STATE DeleteMethod
{
public:
    virtual ~DeleteMethod() {}

    virtual void Delete() = 0;
};

class DLL_STATE IsDeletedMethod
{
public:
    virtual ~IsDeletedMethod() {}

    virtual bool IsDeleted() const = 0;
};

class DLL_STATE ModifyMethod
{
public:
    virtual ~ModifyMethod() {}

    virtual void Modify() = 0;
};

class DLL_STATE IsModifiedMethod
{
public:
    virtual ~IsModifiedMethod() {}

    virtual bool IsModified() const = 0;
};

class DLL_STATE NotModifyMethod
{
public:
    virtual ~NotModifyMethod() {}

    virtual void NotModify() = 0;
};

class DLL_STATE IsNotModifiedMethod
{
public:
    virtual ~IsNotModifiedMethod() {}

    virtual bool IsNotModified() const = 0;
};

class DLL_STATE ReadMethod
{
public:
    virtual ~ReadMethod() {}

    virtual void Read() = 0;
};

class DLL_STATE IsReadMethod
{
public:
    virtual ~IsReadMethod() {}

    virtual bool IsRead() const = 0;
};

template <typename Return>
class SubscribeMethod
{
public:
    virtual ~SubscribeMethod() {}

    virtual Return Subscribe() = 0;
};

template <typename Return, typename Arg1>
class SubscribeMethod1
{
public:
    virtual ~SubscribeMethod1() {}

    virtual Return Subscribe(Arg1) = 0;
};

template <typename Return>
class UnsubscribeMethod
{
public:
    virtual ~UnsubscribeMethod() {}

    virtual Return Unsubscribe() = 0;
};

template <typename Return, typename Arg1>
class UnsubscribeMethod1
{
public:
    virtual ~UnsubscribeMethod1() {}

    virtual Return Unsubscribe(Arg1) = 0;
};

class DLL_STATE IsSubscribedMethod
{
public:
    virtual ~IsSubscribedMethod() {}

    virtual bool IsSubscribed() const = 0;
};

class DLL_STATE WriteMethod
{
public:
    virtual ~WriteMethod() {}

    virtual void Write() = 0;
};

class DLL_STATE IsWrittenMethod
{
public:
    virtual ~IsWrittenMethod() {}

    virtual bool IsWritten() const = 0;
};

class DLL_STATE PublishMethod
{
public:
    virtual ~PublishMethod() {}

    virtual void Publish() = 0;
};

class DLL_STATE IsPublishedMethod
{
public:
    virtual ~IsPublishedMethod() {}

    virtual bool IsPublished() const = 0;
};

template <typename Return>
class ResultMethod
{
public:
    virtual ~ResultMethod() {}

    virtual Return Result() const = 0;
};

template <typename Return, typename Arg1>
class ResultMethod1
{
public:
    virtual ~ResultMethod1() {}

    virtual Return Result(Arg1) const = 0;
};

// --------------------------------------------
// Typical interface methods
// --------------------------------------------

template <typename Return>
class LoginMethod
{
public:
    virtual ~LoginMethod() {}

    virtual Return Login() = 0;
};

class DLL_STATE IsLoggedInMethod
{
public:
    virtual ~IsLoggedInMethod() {}

    virtual bool IsLoggedIn() const = 0;
};

template <typename Return>
class ResetMethod
{
public:
    virtual ~ResetMethod() {}

    virtual Return Reset() = 0;
};

class DLL_STATE CloseMethod
{
public:
    virtual ~CloseMethod() {}

    virtual void Close() = 0;
};

class DLL_STATE IsClosedMethod
{
public:
    virtual ~IsClosedMethod() {}

    virtual bool IsClosed() const = 0;
};

class DLL_STATE CancelMethod
{
public:
    virtual ~CancelMethod() {}

    virtual bool Cancel() = 0;
};

class DLL_STATE IsCancelledMethod
{
public:
    virtual ~IsCancelledMethod() {}

    virtual bool IsCancelled() const = 0;
};

class DLL_STATE InterruptMethod
{
public:
    virtual ~InterruptMethod() {}

    virtual bool Interrupt() = 0;
};

class DLL_STATE IsInterruptedMethod
{
public:
    virtual ~IsInterruptedMethod() {}

    virtual bool IsInterrupted() const = 0;
};

class DLL_STATE DoneMethod
{
public:
    virtual ~DoneMethod() {}

    virtual bool Done() = 0;
};

class DLL_STATE IsDoneMethod
{
public:
    virtual ~IsDoneMethod() {}

    virtual bool IsDone() const = 0;
};

class DLL_STATE EmptyMethod
{
public:
    virtual ~EmptyMethod() {}

    virtual bool Empty() = 0;
};

class DLL_STATE IsEmptyMethod
{
public:
    virtual ~IsEmptyMethod() {}

    virtual bool IsEmpty() const = 0;
};

template <typename Return>
class SizeMethod
{
public:
    virtual ~SizeMethod() {}

    virtual Return Size() const = 0;
};

class DLL_STATE NullMethod
{
public:
    virtual ~NullMethod() {}

    virtual bool Null() = 0;
};

class DLL_STATE IsNullMethod
{
public:
    virtual ~IsNullMethod() {}

    virtual bool IsNull() const = 0;
};

class DLL_STATE TimeoutMethod
{
public:
    virtual ~TimeoutMethod() {}

    virtual void Timeout() = 0;
};

class DLL_STATE IsTimeoutMethod
{
public:
    virtual ~IsTimeoutMethod() {}

    virtual bool IsTimeout() const = 0;
};

template <typename Return>
class TimeoutInMethod
{
public:
    virtual ~TimeoutInMethod() {}

    virtual Return TimeoutIn() const = 0;
};

class DLL_STATE FinishMethod
{
public:
    virtual ~FinishMethod() {}

    virtual void Finish() = 0;
};

class DLL_STATE IsFinishedMethod
{
public:
    virtual ~IsFinishedMethod() {}

    virtual bool IsFinished() const = 0;
};

class DLL_STATE BlockMethod
{
public:
    virtual ~BlockMethod() {}

    virtual bool Block() = 0;
};

class DLL_STATE UnblockMethod
{
public:
    virtual ~UnblockMethod() {}

    virtual bool Unblock() = 0;
};

class DLL_STATE IsBlockedMethod
{
public:
    virtual ~IsBlockedMethod() {}

    virtual bool IsBlocked() const = 0;
};

/**
 * @brief The RepeatMethod class, i.e., Again, OnceMore, etc
 */
class DLL_STATE RepeatMethod
{
public:
    virtual ~RepeatMethod() {}

    virtual bool Repeat() = 0;
};

template <typename Return, typename Arg1>
class WaitMethod
{
public:
    virtual ~WaitMethod() {}

    virtual Return Wait(Arg1) const = 0;
};

template <typename Return, typename Arg1>
class WaitForMethod1
{
public:
    virtual ~WaitForMethod1() {}

    virtual Return WaitFor(Arg1) const = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class WaitForMethod2
{
public:
    virtual ~WaitForMethod2() {}

    virtual Return WaitFor(Arg1, Arg2) const = 0;
};

template <typename Return>
class PolicyMethod
{
public:
    virtual ~PolicyMethod() {}

    virtual Return Policy() const = 0;
};

class DLL_STATE IsPolicyViolatedMethod
{
public:
    virtual ~IsPolicyViolatedMethod() {}

    virtual bool IsPolicyViolated() const = 0;
};

template <typename Return>
class HistoryMethod
{
public:
    virtual ~HistoryMethod() {}

    virtual Return History() const = 0;
};

template <typename Return>
class IdMethod
{
public:
    virtual ~IdMethod() {}

    virtual Return Id() const = 0;
};

template <typename Return>
class DescriptionMethod
{
public:
    virtual ~DescriptionMethod() {}

    virtual Return Description() const = 0;
};

// ----------------------------------------
// Actions from track and field
// "On your mark", "Get set", "Start", "Stop"
// Typical race/run/execution methods
// ----------------------------------------

class DLL_STATE TakeYourMarkMethod
{
public:
    virtual ~TakeYourMarkMethod() {}

    virtual void TakeYourMark() = 0;
};

class DLL_STATE GetSetMethod
{
public:
    virtual ~GetSetMethod() {}

    virtual void GetSet() = 0;
};

class DLL_STATE ReadyMethod
{
public:
    virtual ~ReadyMethod() {}

    virtual void Ready() = 0;
};

template <typename Return>
class ReadyInMethod
{
public:
    virtual ~ReadyInMethod() {}

    virtual Return ReadyIn() const = 0;
};

class DLL_STATE IsReadyMethod
{
public:
    virtual ~IsReadyMethod() {}

    virtual bool IsReady() const = 0;
};

class DLL_STATE IsReceivedMethod
{
public:
    virtual ~IsReceivedMethod() {}

    virtual bool IsReceived() const = 0;
};

class DLL_STATE SuspendMethod
{
public:
    virtual ~SuspendMethod() {}

    virtual void Suspend() = 0;
};

class DLL_STATE IsSuspendedMethod
{
public:
    virtual ~IsSuspendedMethod() {}

    virtual bool IsSuspended() const = 0;
};

class DLL_STATE ResumeMethod
{
public:
    virtual ~ResumeMethod() {}

    virtual void Resume() = 0;
};

template <typename Return>
class AttachMethod
{
public:
    virtual ~AttachMethod() {}

    virtual Return Attach() = 0;
};

class DLL_STATE IsAttachedMethod
{
public:
    virtual ~IsAttachedMethod() {}

    virtual bool IsAttached() const = 0;
};

template <typename Return>
class DetachMethod
{
public:
    virtual ~DetachMethod() {}

    virtual Return Detach() = 0;
};

class DLL_STATE IsResumedMethod
{
public:
    virtual ~IsResumedMethod() {}

    virtual bool IsResumed() const = 0;
};

class DLL_STATE PostponeMethod
{
public:
    virtual ~PostponeMethod() {}

    virtual void Postpone() = 0;
};

class DLL_STATE IsPostponedMethod
{
public:
    virtual ~IsPostponedMethod() {}

    virtual bool IsPostponed() const = 0;
};

class DLL_STATE DeferMethod
{
public:
    virtual ~DeferMethod() {}

    virtual void Defer() = 0;
};

class DLL_STATE IsDeferredMethod
{
public:
    virtual ~IsDeferredMethod() {}

    virtual bool IsDeferred() const = 0;
};

class DLL_STATE IsFailureMethod
{
public:
    virtual ~IsFailureMethod() {}

    virtual bool IsFailure() const = 0;
};

class DLL_STATE StartMethod
{
public:
    virtual ~StartMethod() {}

    virtual void Start() = 0;
};

class DLL_STATE IsStartedMethod
{
public:
    virtual ~IsStartedMethod() {}

    virtual bool IsStarted() const = 0;
};

class DLL_STATE IsCurrentMethod
{
public:
    virtual ~IsCurrentMethod() {}

    virtual bool IsCurrent() const = 0;
};

class DLL_STATE IsStartingMethod
{
public:
    virtual ~IsStartingMethod() {}

    virtual bool IsStarting() const = 0;
};

class DLL_STATE FalseStartMethod
{
public:
    virtual ~FalseStartMethod() {}

    virtual void FalseStart() = 0;
};

class DLL_STATE GoMethod
{
public:
    virtual ~GoMethod() {}

    virtual void Go() = 0;
};

class DLL_STATE TriggerMethod
{
public:
    virtual ~TriggerMethod() {}

    virtual void Trigger() = 0;
};

class DLL_STATE IsTriggeredMethod
{
public:
    virtual ~IsTriggeredMethod() {}

    CLASS_TRAITS(IsTriggeredMethod)

    virtual bool IsTriggered() const = 0;
};

class DLL_STATE UntriggerMethod
{
public:
    virtual ~UntriggerMethod() {}

    virtual void Untrigger() = 0;
};

class DLL_STATE StopMethod
{
public:
    virtual ~StopMethod() {}

    virtual void Stop() = 0;
};

class DLL_STATE IsStoppedMethod
{
public:
    virtual ~IsStoppedMethod() {}

    virtual bool IsStopped() const = 0;
};

template <typename Return>
class StopAndWaitMethod
{
public:
    virtual ~StopAndWaitMethod() {}

    virtual Return Stop(int64 msecs) = 0;
};

class DLL_STATE AbortMethod
{
public:
    virtual ~AbortMethod() {}

    virtual void Abort() = 0;
};

class DLL_STATE IsAbortedMethod
{
public:
    virtual ~IsAbortedMethod() {}

    virtual bool IsAborted() const = 0;
};

class DLL_STATE SuccessMethod
{
public:
    virtual ~SuccessMethod() {}

    virtual void Success() = 0;
};

class DLL_STATE IsSuccessMethod
{
public:
    virtual ~IsSuccessMethod() {}

    virtual bool IsSuccess() const = 0;
};

class DLL_STATE FailureMethod
{
public:
    virtual ~FailureMethod() {}

    virtual void Failure() = 0;
};

class DLL_STATE IsFailedMethod
{
public:
    virtual ~IsFailedMethod() {}

    virtual bool IsFailed() const = 0;
};

class DLL_STATE ErrorMethod
{
public:
    virtual ~ErrorMethod() {}

    virtual void Error() = 0;
};

template<typename Return, typename T>
class ErrorMethod1
{
public:
    virtual ~ErrorMethod1() {}

    virtual Return Error(T t) = 0;
};

class DLL_STATE CompleteMethod
{
public:
    virtual ~CompleteMethod() {}

    virtual bool Complete() = 0;
};

class DLL_STATE IsCompletedMethod
{
public:
    virtual ~IsCompletedMethod() {}

    virtual bool IsCompleted() const = 0;
};

template <typename T>
class VoidNextMethod1
{
public:
    virtual ~VoidNextMethod1() {}

    virtual void Next(T) = 0;
};


template <typename Return, typename T>
class NextMethod1
{
public:
    virtual ~NextMethod1() {}

    virtual Return Next(T) = 0;
};


template <typename Return>
class ScheduleMethod
{
public:
    virtual ~ScheduleMethod() {}

    virtual Return Schedule(int64 msecs) = 0;
};

class DLL_STATE IsScheduledMethod
{
public:
    virtual ~IsScheduledMethod() {}

    virtual bool IsScheduled() const = 0;
};

class DLL_STATE RunMethod
{
public:
    virtual ~RunMethod() {}

    virtual void Run() = 0;
};

class DLL_STATE IsRunningMethod
{
public:
    virtual ~IsRunningMethod() {}

    virtual bool IsRunning() const = 0;
};

class DLL_STATE IsIdleMethod
{
public:
    virtual ~IsIdleMethod() {}

    virtual bool IsIdle() const = 0;
};

class DLL_STATE RaceMethods
        : public TakeYourMarkMethod
        , public GetSetMethod
        , public StartMethod
        , public StopMethod
        , public AbortMethod
        , public SuccessMethod
        , public FailureMethod
        , public ErrorMethod
        , public CompleteMethod
{
public:
    virtual ~RaceMethods() {}
};

template <typename Return, typename T, typename Err>
class ReactorMethods
        : public CompleteMethod
        , public ErrorMethod1<bool, Err>
        , public NextMethod1<Return, T>
{
public:
    virtual ~ReactorMethods() {}
};

class DLL_STATE EntryMethod
{
public:
    virtual ~EntryMethod() {}

    virtual void Entry() = 0;
};

template <typename T>
class EntryMethod1
{
public:
    virtual ~EntryMethod1() {}

    virtual void Entry(T &t) = 0;
};

class DLL_STATE ExitMethod
{
public:
    virtual ~ExitMethod() {}

    virtual void Exit() = 0;
};

template <typename T>
class ExitMethod1
{
public:
    virtual ~ExitMethod1() {}

    virtual void Exit(T &t) = 0;
};

template <typename T, typename U>
class ExitMethod2
{
public:
    virtual ~ExitMethod2() {}

    virtual void Exit(T &t, U &u) = 0;
};

// --------------------------------------------
// Various methods
// --------------------------------------------

template <typename LOGGER>
class LogMethod
{
public:
    virtual ~LogMethod() {}

    virtual void Log(LOGGER &) = 0;
};

template <typename T>
class DefaultMethod
{
public:
    virtual ~DefaultMethod() {}

    virtual T Default() const = 0;
};

template <typename T>
class MergeMethod
{
public:
    virtual ~MergeMethod() {}

    virtual bool Merge(const T &) = 0;
};

template <typename Return>
class UniqueMethod
{
public:
    virtual ~UniqueMethod() {}

    virtual Return Unique() = 0;
};

class DLL_STATE IsUniqueMethod
{
public:
    virtual ~IsUniqueMethod() {}

    virtual bool IsUnique() const = 0;
};

template <typename T>
class TransactionMethod
{
public:
    virtual ~TransactionMethod() {}

    virtual bool Transaction(const T &) = 0;
};

template <typename Return>
class TypenameMethod
{
public:
    virtual ~TypenameMethod() {}

    virtual Return Typename() const = 0;
};

template <typename Return, typename Rights>
class AuthorizationMethod
{
public:
    virtual ~AuthorizationMethod() {}

    virtual Return IsAuthorized(const Rights &) const = 0;
};

class DLL_STATE NoOpMethod
{
public:
    virtual ~NoOpMethod() {}

    /**
     * @brief NoOperation may be used as a "NoOp" in FutureTasks, Signal, Commands, etc.
     */
    void NoOp() {}
};

template <typename T>
class DelegateMethod
{
public:
    virtual ~DelegateMethod() {}

    virtual T Delegate() const = 0;
};

template <typename T>
class FunctionMethod
{
public:
    virtual ~FunctionMethod() {}

    virtual T Function() const = 0;
};

template <typename T>
class StatusMethod
{
public:
    virtual ~StatusMethod() {}

    virtual T& Status() = 0;
};

template <typename T>
class StatusConstMethod
{
public:
    virtual ~StatusConstMethod() {}

    virtual const T& StatusConst() const = 0;
};

template <typename T>
class TaskMethod
{
public:
    virtual ~TaskMethod() {}

    virtual T& Task() = 0;
};

template <typename T>
class TaskConstMethod
{
public:
    virtual ~TaskConstMethod() {}

    virtual const T& TaskConst() const = 0;
};

// --------------------------------------------
// Execute methods
// --------------------------------------------

template <typename Return>
class ExecuteMethod0
{
public:
    virtual ~ExecuteMethod0() {}

    virtual Return Execute() = 0;
};

template <typename Return, typename Arg1>
class ExecuteMethod1
{
public:
    virtual ~ExecuteMethod1() {}

    virtual Return Execute(Arg1) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class ExecuteMethod2
{
public:
    virtual ~ExecuteMethod2() {}

    virtual Return Execute(Arg1, Arg2) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class ExecuteMethod3
{
public:
    virtual ~ExecuteMethod3() {}

    virtual Return Execute(Arg1 t, Arg2 u, Arg3 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class ExecuteMethod4
{
public:
    virtual ~ExecuteMethod4() {}

    virtual Return Execute(Arg1 t, Arg2 u, Arg3 v, Arg4 w) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class ExecuteMethod5
{
public:
    virtual ~ExecuteMethod5() {}

    virtual Return Execute(Arg1 t, Arg2 u, Arg3 v, Arg4 w, Arg5 arg5) = 0;
};

class IsExecutingMethod
{
public:
    virtual ~IsExecutingMethod() {}

    virtual bool IsExecuting() const = 0;
};

// --------------------------------------------
// Invoke methods
// --------------------------------------------

template <typename Return>
class InvokeMethod0
{
public:
    virtual ~InvokeMethod0() {}

    virtual Return Invoke() = 0;
};

template <typename Return>
class InvokeConstMethod0
{
public:
    virtual ~InvokeConstMethod0() {}

    virtual Return Invoke() const = 0;
};

class VoidInvokeMethod0 : public InvokeMethod0<void>
{
public:
    virtual ~VoidInvokeMethod0() {}

    CLASS_TRAITS(VoidInvokeMethod0)
};

template <typename Return, typename Arg1>
class InvokeMethod1
{
public:
    virtual ~InvokeMethod1() {}

    virtual Return Invoke(Arg1 t) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class InvokeMethod2
{
public:
    virtual ~InvokeMethod2() {}

    virtual Return Invoke(Arg1 t, Arg2 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class InvokeMethod3
{
public:
    virtual ~InvokeMethod3() {}

    virtual Return Invoke(Arg1 t, Arg2 u, Arg3 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class InvokeMethod4
{
public:
    virtual ~InvokeMethod4() {}

    virtual Return Invoke(Arg1 t, Arg2 u, Arg3 v, Arg4 w) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class InvokeMethod5
{
public:
    virtual ~InvokeMethod5() {}

    virtual Return Invoke(Arg1 t, Arg2 u, Arg3 v, Arg4 w, Arg5 arg5) = 0;
};

class IsInvokingMethod
{
public:
    virtual ~IsInvokingMethod() {}

    virtual bool IsInvoking() const = 0;
};

// --------------------------------------------
// Invoke methods
// --------------------------------------------

template <typename Return>
class OperatorMethod0
{
public:
    virtual ~OperatorMethod0() {}

    virtual Return operator()() = 0;
};

template <typename Return, typename Arg1>
class OperatorMethod1
{
public:
    virtual ~OperatorMethod1() {}

    virtual Return operator()(Arg1 t) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class OperatorMethod2
{
public:
    virtual ~OperatorMethod2() {}

    virtual Return operator()(Arg1 t, Arg2 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class OperatorMethod3
{
public:
    virtual ~OperatorMethod3() {}

    virtual Return operator()(Arg1 t, Arg2 u, Arg3 v) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class OperatorMethod4
{
public:
    virtual ~OperatorMethod4() {}

    virtual Return operator()(Arg1 t, Arg2 u, Arg3 v, Arg4 w) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class OperatorMethod5
{
public:
    virtual ~OperatorMethod5() {}

    virtual Return operator()(Arg1 t, Arg2 u, Arg3 v, Arg4 w, Arg5 x) = 0;
};

// --------------------------------------------
// Event selector methods
// --------------------------------------------

template <typename Return, typename K>
class EventSelectorMethod
{
public:
    virtual ~EventSelectorMethod() {}

    CLASS_TRAITS(EventSelectorMethod)

    virtual Return Select(int64 msecs = LONG_MAX) = 0;
};

template <typename Return, typename K, typename Arg1>
class EventSelectorMethod1
{
public:
    virtual ~EventSelectorMethod1() {}

    CLASS_TRAITS(EventSelectorMethod1)

    virtual Return Select(Arg1 arg1) = 0;
};

template <typename Return, typename K, typename Arg1, typename Arg2>
class EventSelectorMethod2
{
public:
    virtual ~EventSelectorMethod2() {}

    CLASS_TRAITS(EventSelectorMethod2)

    virtual Return Select(Arg1 arg1, Arg2 arg2) = 0;
};

}}
