#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/DataAccessGroup.h"

namespace Reactor
{

// -----------------------------------------------------------
// class DataAccessGroupChainState
// -----------------------------------------------------------

DataAccessGroupChainState::DataAccessGroupChainState()
{}

DataAccessGroupChainState::~DataAccessGroupChainState()
{ }

DataAccessGroupChainState::GroupVector &DataAccessGroupChainState::Group()
{
    return group_;
}

const DataAccessGroupChainState::GroupVector &DataAccessGroupChainState::Group() const
{
    return group_;
}

// -----------------------------------------------------------
// class DataAccessGroupChain
// -----------------------------------------------------------

DataAccessGroupChain::DataAccessGroupChain()
    : Templates::ContextObjectShared<DataAccessGroupChainPolicy, DataAccessGroupChainState, Status::ExecutionStatus>
      (
          new DataAccessGroupChainPolicy(), new DataAccessGroupChainState(), new Status::ExecutionStatus()
      )
{}

DataAccessGroupChain::~DataAccessGroupChain()
{}

// -----------------------------------------------------------
// Group managment
// -----------------------------------------------------------

DataAccessGroup DataAccessGroupChain::Sequential()
{
    DataAccessGroup dataAccessGroup = DataAccessGroup::Sequential();
    this->data()->Group().push_back(dataAccessGroup);

    return dataAccessGroup;
}

DataAccessGroup DataAccessGroupChain::Parallel()
{
    DataAccessGroup dataAccessGroup = DataAccessGroup::Parallel();
    this->data()->Group().push_back(dataAccessGroup);

    return dataAccessGroup;
}

std::vector<DataAccessGroup> &DataAccessGroupChain::Group()
{
    return this->data()->Group();
}

const std::vector<DataAccessGroup> &DataAccessGroupChain::Group() const
{
    return this->data()->Group();
}

const Status::ExecutionStatus &DataAccessGroupChain::StatusConst() const
{
    return this->status().delegate();
}

void DataAccessGroupChain::Start()
{
    this->status()->Start();
}

// -----------------------------------------------------------
// Factory functions
// -----------------------------------------------------------

DataAccessGroupChain DataAccessGroupChain::Create()
{
    return DataAccessGroupChain();
}

// -----------------------------------------------------------
// reactor functions - add dataAccess to current group execution status (similar to TCP acknowledgments, "Execution control protocol")
// -----------------------------------------------------------

bool DataAccessGroupChain::Next(DataAccessGroup )
{
    // TODO: Anything to do?
    return true;
}

bool DataAccessGroupChain::Error(GeneralException )
{
    this->status()->Failure();
    return true;
}

bool DataAccessGroupChain::Complete()
{
    this->status()->Success();
    return true;
}

}
