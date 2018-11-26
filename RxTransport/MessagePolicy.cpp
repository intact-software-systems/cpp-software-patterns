#include "RxTransport/MessagePolicy.h"

namespace BaseLib { namespace Concurrent {

MessagePolicy::MessagePolicy()
{ }

MessagePolicy::~MessagePolicy()
{ }

MessagePolicy MessagePolicy::Default()
{
    return Concurrent::MessagePolicy();
}

}}