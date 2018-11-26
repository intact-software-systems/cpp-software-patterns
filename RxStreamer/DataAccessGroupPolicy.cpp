#include "RxStreamer/DataAccessGroupPolicy.h"

namespace Reactor
{

DataAccessGroupPolicy::DataAccessGroupPolicy(Policy::Computation computation)
    : computation_(computation)
{}

DataAccessGroupPolicy::~DataAccessGroupPolicy()
{}

const Policy::Computation &DataAccessGroupPolicy::Computation() const
{
    return computation_;
}

DataAccessGroupPolicy DataAccessGroupPolicy::Sequential()
{
    return DataAccessGroupPolicy(Policy::Computation::Sequential());
}

DataAccessGroupPolicy DataAccessGroupPolicy::Parallel()
{
    return DataAccessGroupPolicy(Policy::Computation::Parallel());
}

}
