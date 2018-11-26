#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/Worker.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

// ------------------------------------------------------
// WorkersPolicy class
// ------------------------------------------------------

class DLL_STATE WorkersPolicy
{
public:
    WorkersPolicy(Policy::MaxLimitULong maxNumWorkers = Policy::MaxLimitULong(Policy::LimitKind::INCLUSIVE, Thread::idealThreadCount()));
    virtual ~WorkersPolicy();

    const Policy::MaxLimitULong& MaxNumWorkers() const;

    static WorkersPolicy Default();

private:
    Policy::MaxLimitULong maxNumWorkers_;
};

// ------------------------------------------------------
// WorkersConfig class
// ------------------------------------------------------

class DLL_STATE WorkersConfig : protected Templates::ImmutableType<WorkersPolicy>
{
public:
    WorkersConfig(WorkersPolicy policy = WorkersPolicy::Default());
    virtual ~WorkersConfig();

    const WorkersPolicy& policy() const;
};

// ------------------------------------------------------
// WorkersState class
// ------------------------------------------------------

class DLL_STATE WorkersState
{
public:
    typedef std::list<Worker::Ptr>  ListWorkers;

public:
    WorkersState();
    ~WorkersState();

    const ListWorkers& Workers() const;
    ListWorkers& Workers();

private:
    ListWorkers workers_;
};

// ------------------------------------------------------
// Workers class
// ------------------------------------------------------

/**
 * Creates and operates on Worker instances.
 *
 * TODO: Use cache implementation with Lease and renew on access to control lifetime of worker instances.
 * TODO: MaxLimit, Remove excess threads under low loads.
 * TODO: If more than a "configurable duration" since worker ran then remove
 */
class DLL_STATE Workers
        : public Templates::Action
        , public Templates::Lockable<Mutex>
        , public Templates::ContextObject<WorkersConfig, WorkersState>
        , public ENABLE_SHARED_FROM_THIS(Workers)
{
private:
    typedef MutexTypeLocker<Workers> Locker;

public:
    Workers();
    virtual ~Workers();

    CLASS_TRAITS(Workers)

    Workers::Ptr GetPtr();

    size_t Size() const;
    bool IsEmpty() const;

    // ----------------------------------------------------
    // Worker management
    // ----------------------------------------------------

    Worker::Ptr Create(const std::string &workerName, WaitSet::Ptr taskQueue, const WorkerPolicy &policy);

    WorkersState::ListWorkers RemoveDone();

    int Restart(int numToRestart);

    bool Clear(WaitSet::Ptr waitSet);

    // ----------------------------------------------------
    // Administrate workers
    // ----------------------------------------------------

    virtual bool Cancel();
    virtual void Start();
    virtual bool IsDone() const;
    virtual bool IsSuccess() const;

    bool IsSpace() const;
    int NumWorkers() const;

    // ----------------------------------------------------
    // Status
    // ----------------------------------------------------

    void Report(std::ostream &ostr) const;

private:
    // ----------------------------------------------------
    // Private implementation
    // ----------------------------------------------------

    Worker::Ptr create(const std::string &workerName, WaitSet::Ptr taskQueue, const WorkerPolicy &policy);

    WorkersState::ListWorkers removeDone();

    bool clear(WaitSet::Ptr waitSet);
    bool cancel();
    void start();

    bool isSpace() const;
    bool isDone() const;
    bool isSuccess() const;

    int numWorkers() const;
    int restart(int numToRestart);
};

}}
