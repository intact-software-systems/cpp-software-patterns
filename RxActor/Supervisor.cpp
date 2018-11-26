#include "Supervisor.h"

namespace RxActor {

Supervisor::Supervisor(DeciderAlgorithm decider, SupervisorPolicy* policy)
    : Templates::ContextObjectShared<SupervisorPolicy, SupervisorData, SupervisorStatus>
    (
        policy, new SupervisorData{decider}, new SupervisorStatus()
    )
{ }

Supervisor::~Supervisor()
{ }

const SupervisorPolicy& Supervisor::Policy() const
{
    return this->config().delegate();
}

DeciderAlgorithm Supervisor::Decider() const
{
    return this->data()->decider_;
}

Policy::ReactionKind Supervisor::Decide(BaseLib::Exception exception)
{
    return this->data()->decider_(exception);
}

//void Supervisor::ProcessFailure(ActorBasePtr context, ActorProxy child, BaseLib::Exception exception)
//{
//    //SA::ProcessFailure(this->config().delegate(), this->status().delegate(), context, child, exception);
//}

}