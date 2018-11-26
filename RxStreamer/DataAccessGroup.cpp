#include "RxStreamer/DataAccessGroup.h"

namespace Reactor
{

// -----------------------------------------------------------
// class DataAccessGroupState
// -----------------------------------------------------------

DataAccessGroupState::DataAccessGroupState()
{ }

DataAccessGroupState::~DataAccessGroupState()
{ }

void DataAccessGroupState::Chain(DataAccessBase::Ptr access)
{
    chain.push_back(access);
}

const DataAccessGroupState::DataAccessList &DataAccessGroupState::accesses() const
{
    return chain;
}

DataAccessGroupState::DataAccessList &DataAccessGroupState::accesses()
{
    return chain;
}

const DataAccessGroupState::DataAccessList &DataAccessGroupState::CurrentFinishedExecution() const
{
    return currentFinishedExecution;
}

DataAccessGroupState::DataAccessList &DataAccessGroupState::CurrentFinishedExecution()
{
    return currentFinishedExecution;
}

// -----------------------------------------------------------
// class DataAccessGroup
// -----------------------------------------------------------

DataAccessGroup::DataAccessGroup(DataAccessGroupPolicy policy)
    : Templates::ContextObjectShared<DataAccessGroupPolicy, DataAccessGroupState, Status::ExecutionStatus>
      (
            new DataAccessGroupPolicy(policy.Computation()), new DataAccessGroupState(), new Status::ExecutionStatus()
      )
{ }

DataAccessGroup::~DataAccessGroup()
{ }

// -----------------------------------------------------------
// Factory functions
// -----------------------------------------------------------

DataAccessGroup DataAccessGroup::Sequential()
{
    return DataAccessGroup(DataAccessGroupPolicy::Sequential());
}

DataAccessGroup DataAccessGroup::Parallel()
{
    return DataAccessGroup(DataAccessGroupPolicy::Parallel());
}

// -----------------------------------------------------------
// Access statuses and state
// -----------------------------------------------------------

std::vector<DataAccessBase::Ptr> DataAccessGroup::getList()
{
    Locker lock(this);
    return this->data()->accesses();
}

bool DataAccessGroup::IsExecuting() const
{
    return this->status()->IsExecuting();
}

bool DataAccessGroup::IsDone() const
{
    Locker lock(this);

    return isDone();
}

bool DataAccessGroup::IsReady() const
{
    Locker lock(this);

    return !isDone() && !this->data()->accesses().empty();
}

bool DataAccessGroup::IsFailure() const
{
    return this->status()->IsLastExecutionFailure();
}

size_t DataAccessGroup::Size() const
{
    Locker lock(this);
    return this->data()->accesses().size();
}

void DataAccessGroup::Start()
{
    Locker lock(this);

    this->data()->CurrentFinishedExecution().clear();
    this->status()->Start();
}

// -----------------------------------------------------------
// reactor functions - add dataAccess to current group execution status (similar to TCP acknowledgments, "Execution control protocol")
// -----------------------------------------------------------

bool DataAccessGroup::Next(DataAccessBase::Ptr dataAccess)
{
    Locker lock(this);

    IINFO() << "Time spent to complete " << dataAccess->CacheId() << ": " << dataAccess->StatusConst().executionStatus().Time().TimeSinceLastExecutionTime();

    this->data()->CurrentFinishedExecution().push_back(dataAccess);

    if(this->data()->CurrentFinishedExecution().size() == this->data()->accesses().size())
    {
        IINFO() << "Time elapsed to group finish: " << this->status()->Time().TimeSinceLastExecutionTime();
        this->status()->Success();
    }

    return true;
}

bool DataAccessGroup::Error(BaseLib::GeneralException exception)
{
    Locker lock(this);

    IINFO() << "Time spent to error " << this->status()->Time().TimeSinceLastExecutionTime() << " details: " << exception;
    //this->data()->CurrentFinishedExecution().push_back(dataAccess);
    this->status()->Failure();

    return true;
}

bool DataAccessGroup::Complete()
{
    Locker lock(this);
    this->status()->Success();

    return true;
}

bool DataAccessGroup::isDone() const
{
    return this->status()->IsSuccess() || this->data()->CurrentFinishedExecution().size() == this->data()->accesses().size();
}

}
