#include "RxStreamer/RxDataAccessGroup.h"

namespace Reactor
{

RxDataAccessGroup::RxDataAccessGroup(Templates::ActionContextPtr actionContext, DataAccessGroup group)
    : actionContext_(actionContext)
    , group_(group)
{ }

RxDataAccessGroup::~RxDataAccessGroup()
{ }

// --------------------------------------------------
// Factory functions
// --------------------------------------------------

RxDataAccessGroup RxDataAccessGroup::Sequential(Templates::ActionContextPtr actionContext)
{
    return RxDataAccessGroup(actionContext, DataAccessGroup::Sequential());
}

RxDataAccessGroup RxDataAccessGroup::Parallel(Templates::ActionContextPtr actionContext)
{
    return RxDataAccessGroup(actionContext, DataAccessGroup::Parallel());
}

// --------------------------------------------------
// Access to group
// --------------------------------------------------

DataAccessGroup RxDataAccessGroup::Group()
{
    return group_;
}

}
