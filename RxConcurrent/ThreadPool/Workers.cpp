#include"RxConcurrent/ThreadPool/Workers.h"

namespace BaseLib { namespace Concurrent
{

// ----------------------------------------------------
// WorkersPolicy class
// ----------------------------------------------------

WorkersPolicy::WorkersPolicy(Policy::MaxLimitULong maxNumWorkers)
    : maxNumWorkers_(maxNumWorkers)
{}

WorkersPolicy::~WorkersPolicy()
{}

const Policy::MaxLimitULong &WorkersPolicy::MaxNumWorkers() const
{
    return maxNumWorkers_;
}

WorkersPolicy WorkersPolicy::Default()
{
    return WorkersPolicy(Policy::MaxLimitULong(Policy::LimitKind::INCLUSIVE, Thread::idealThreadCount()));
}

// ----------------------------------------------------
// WorkersConfig class
// ----------------------------------------------------

WorkersConfig::WorkersConfig(WorkersPolicy policy)
    : Templates::ImmutableType<WorkersPolicy>(policy)
{}

WorkersConfig::~WorkersConfig()
{}

const WorkersPolicy &WorkersConfig::policy() const
{
    return this->delegate();
}

// ----------------------------------------------------
// WorkersState class
// ----------------------------------------------------

WorkersState::WorkersState()
{}

WorkersState::~WorkersState()
{}

const WorkersState::ListWorkers &WorkersState::Workers() const
{
    return workers_;
}

WorkersState::ListWorkers &WorkersState::Workers()
{
    return workers_;
}

// ----------------------------------------------------
// Workers class
// ----------------------------------------------------

Workers::Workers()
    : Templates::ContextObject<WorkersConfig, WorkersState>
      (
          WorkersConfig(WorkersPolicy()),
          WorkersState()
      )
{}

Workers::~Workers()
{}

Workers::Ptr Workers::GetPtr()
{
    return this->shared_from_this();
}

size_t Workers::Size() const
{
    Locker lock(this);
    return this->state().Workers().size();
}

bool Workers::IsEmpty() const
{
    Locker lock(this);
    return this->state().Workers().empty();
}

// ----------------------------------------------------
// Worker management
// ----------------------------------------------------

Worker::Ptr Workers::Create(const std::string &workerName, WaitSet::Ptr taskQueue, const WorkerPolicy &policy)
{
    Locker lock(this);
    return create(workerName, taskQueue, policy);
}

WorkersState::ListWorkers Workers::RemoveDone()
{
    Locker lock(this);
    return removeDone();
}

int Workers::Restart(int numToRestart)
{
    Locker lock(this);
    return restart(numToRestart);
}

bool Workers::Clear(WaitSet::Ptr waitSet)
{
    Locker lock(this);
    return clear(waitSet);
}

// ----------------------------------------------------
// Administrate workers
// ----------------------------------------------------

bool Workers::Cancel()
{
    Locker lock(this);
    return cancel();
}

void Workers::Start()
{
    Locker lock(this);
    start();
}

bool Workers::IsDone() const
{
    Locker lock(this);
    return isDone();
}

bool Workers::IsSuccess() const
{
    Locker lock(this);
    return isSuccess();
}

bool Workers::IsSpace() const
{
    Locker lock(this);
    return this->isSpace();
}

int Workers::NumWorkers() const
{
    Locker lock(this);
    return this->numWorkers();
}

// ----------------------------------------------------
// Status
// ----------------------------------------------------

void Workers::Report(std::ostream &ostr) const
{
    for(const Worker::Ptr& worker : this->state().Workers())
    {
        ostr << "Worker: " << worker->state().Status() << std::endl;
    }
}


// ----------------------------------------------------
// Private implementation
// ----------------------------------------------------

Worker::Ptr Workers::create(const std::string &workerName, WaitSet::Ptr taskQueue, const WorkerPolicy &policy)
{
    if(!isSpace()) return Worker::Ptr();

    GuardCondition::Ptr    guard(new GuardCondition());
    Worker::Ptr            worker(new Worker(workerName, taskQueue, guard->GetPtr(), policy));

    this->state().Workers().push_back(worker->GetPtr());

    return worker->GetPtr();
}

WorkersState::ListWorkers Workers::removeDone()
{
    WorkersState::ListWorkers doneWorkers;

    for(Worker::Ptr &worker : state().Workers())
    {
        if(worker->IsDone() && !worker->IsInLifespan())
        {
            doneWorkers.push_back(worker->GetPtr());
        }
    }

    for(Worker::Ptr &worker : doneWorkers)
    {
        this->state().Workers().remove(worker);
    }

    return doneWorkers;
}

bool Workers::clear(WaitSet::Ptr waitSet)
{
    for(Worker::Ptr &worker : state().Workers())
    {
        worker->Cancel();

        bool isDetached = waitSet->Detach(worker->GetExecutionGuard());
        ASSERT(isDetached);
    }

    size_t sz = state().Workers().size();

    state().Workers().clear();

    return sz != 0;
}

bool Workers::isSpace() const
{
    return this->config().policy().MaxNumWorkers().IsWithin(numWorkers());
}

bool Workers::cancel()
{
    for(Worker::Ptr &worker : this->state().Workers())
    {
        worker->Cancel();
    }

    return true;
}

void Workers::start()
{
    for(Worker::Ptr &worker : this->state().Workers())
    {
        worker->Start();
    }
}

bool Workers::isDone() const
{
    for(const Worker::Ptr &worker : this->state().Workers())
    {
        if(!worker->IsDone())
            return false;
    }

    return true;
}

bool Workers::isSuccess() const
{
    for(const Worker::Ptr &worker : this->state().Workers())
    {
        if(!worker->IsSuccess())
            return false;
    }
    return true;
}

int Workers::numWorkers() const
{
    int numOfWorkers = 0;
    for(const Worker::Ptr &worker : this->state().Workers())
    {
        if(!worker->IsDone())
            numOfWorkers++;
    }

    return numOfWorkers;
}

int Workers::restart(int numToRestart)
{
    int numRestarted = 0;
    for(const Worker::Ptr &worker : this->state().Workers())
    {
        if(numRestarted >= numToRestart) break;

        if(worker->IsDone())
        {
            worker->Start();
            numRestarted++;
        }
    }
    return numRestarted;
}

}}
