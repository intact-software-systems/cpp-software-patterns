#include"RxSignal/Signaller.h"

namespace BaseLib { namespace Concurrent
{

Signaller::Signaller()
    : handle_(0)
    , block_(false)
{ }

Signaller::~Signaller()
{ }

void Signaller::Block()
{
    block_.set(true);
}

void Signaller::Unblock()
{
    block_.set(false);
}

bool Signaller::IsBlocked() const
{
    return block_.get();
}

InstanceHandle Signaller::Handle() const
{
    return handle_;
}

void Signaller::incHandle()
{
    ++handle_;
}

}}

