#pragma once

namespace BaseLib { namespace Observer
{

class TakeYourMarkObserver
{
public:
    virtual ~TakeYourMarkObserver() {}

    virtual void OnTakeYourMark() = 0;
};

class GetSetObserver
{
public:
    virtual ~GetSetObserver() {}

    virtual void OnGetSet() = 0;
};

class StartObserver
{
public:
    virtual ~StartObserver() {}

    virtual void OnStart() = 0;
};

class StopObserver
{
public:
    virtual ~StopObserver() {}

    virtual void OnStop() = 0;
};

class AbortObserver
{
public:
    virtual ~AbortObserver() {}

    virtual void OnAbort() = 0;
};

class SuccessObserver
{
public:
    virtual ~SuccessObserver() {}

    virtual void OnSuccess() = 0;
};

class FailureObserver
{
public:
    virtual ~FailureObserver() {}

    virtual void OnFailure() = 0;
};

class ErrorObserver
{
public:
    virtual ~ErrorObserver() {}

    virtual void OnError() = 0;
};

template <typename T>
class ErrorObserver1
{
public:
    virtual ~ErrorObserver1() {}

    virtual void OnError(T) = 0;
};

class CompleteObserver
{
public:
    virtual ~CompleteObserver() {}

    virtual void OnComplete() = 0;
};

template <typename T>
class OneArgCompleteObserver
{
public:
    virtual ~OneArgCompleteObserver() {}

    virtual void OnComplete(T) = 0;
};

template <typename T>
class NextObserver
{
public:
    virtual ~NextObserver() {}

    virtual void OnNext(T) = 0;
};

/**
 * @brief The RaceObserver class
 */
class RaceObserver
        : public TakeYourMarkObserver
        , public GetSetObserver
        , public StartObserver
        , public StopObserver
        , public AbortObserver
        , public SuccessObserver
        , public FailureObserver
        , public ErrorObserver
        , public CompleteObserver
{
public:
    virtual ~RaceObserver() {}
};

/**
 * @brief The ReactiveObserver class
 *
 * TODO: typename Error should give receiver access to source
 */
template <typename T, typename Error>
class ReactiveObserver
        : public NextObserver<T>
        , public CompleteObserver
        , public ErrorObserver1<Error>
{
public:
    virtual ~ReactiveObserver() {}
};

}}
