#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DataAccessGroup;

// -----------------------------------------------------------
// class DataAccessGroupChainPolicy
// -----------------------------------------------------------

class DLL_STATE DataAccessGroupChainPolicy
{
    // TODO: success criterion (all, n-success)
    // TODO: failure strategy: fail-fast, continue, etc
};

// -----------------------------------------------------------
// class DataAccessGroupChainState
// -----------------------------------------------------------

class DLL_STATE DataAccessGroupChainState
{
public:
    typedef std::vector<DataAccessGroup> GroupVector;

public:
    DataAccessGroupChainState();
    virtual ~DataAccessGroupChainState();

    GroupVector&        Group();
    const GroupVector&  Group() const;

private:
    std::vector<DataAccessGroup> group_;
};

// -----------------------------------------------------------
// class DataAccessGroupChain
// -----------------------------------------------------------

class DLL_STATE DataAccessGroupChain
        : public Templates::StatusConstMethod<Status::ExecutionStatus>
        , public Templates::StartMethod
        , public Templates::ReactorMethods<bool, DataAccessGroup, BaseLib::GeneralException>
        , public Templates::ContextObjectShared<DataAccessGroupChainPolicy, DataAccessGroupChainState, Status::ExecutionStatus>
{
public:
    DataAccessGroupChain();
    virtual ~DataAccessGroupChain();

    // -----------------------------------------------------------
    // Group managment
    // -----------------------------------------------------------

    DataAccessGroup Sequential();
    DataAccessGroup Parallel();

    std::vector<DataAccessGroup>&       Group();
    const std::vector<DataAccessGroup>& Group() const;

    const Status::ExecutionStatus& StatusConst() const;
    virtual void Start();

    // -----------------------------------------------------------
    // Factory functions
    // -----------------------------------------------------------

    static DataAccessGroupChain Create();

    // -----------------------------------------------------------
    // reactor functions - add dataAccess to current group execution status (similar to TCP acknowledgments, "Execution control protocol")
    // -----------------------------------------------------------

    virtual bool Next(DataAccessGroup group);
    virtual bool Error(BaseLib::GeneralException exception);
    virtual bool Complete();
};

}
