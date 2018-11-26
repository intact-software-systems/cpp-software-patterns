#include"BaseLib/Status/FutureStatus.h"

namespace BaseLib { namespace Status
{


// ------------------------------------------------------
// Operators
// ------------------------------------------------------

std::ostream& operator<<(std::ostream& ostr, const FutureStatusKind::Type& kind)
{
    switch(kind)
    {
#define CASE(t) case t: ostr << #t; break;
    CASE(FutureStatusKind::Deferred)
    CASE(FutureStatusKind::Ready)
    CASE(FutureStatusKind::Failed)
    CASE(FutureStatusKind::Timeout)
#undef CASE
    default:
        std::cerr << "Missing operator<< case for FutureStatusKind::Type" << std::endl;
        break;
    }
    return ostr;
}

// ------------------------------------------------------
// FutureStatus
// ------------------------------------------------------


FutureStatus::FutureStatus(FutureStatusKind::Type kind, BaseLib::GeneralException exception)
    : Templates::ComparableImmutableType<FutureStatusKind::Type>(kind)
    , exception_(exception)
{}

FutureStatus::~FutureStatus()
{}

const FutureStatusKind::Type &FutureStatus::Kind() const
{
    return this->delegate();
}

// ---------------------------------------
// Convenience functions
// ---------------------------------------

bool FutureStatus::IsDeferred() const
{
    return this->delegate() == FutureStatusKind::Deferred;
}

bool FutureStatus::IsReady() const
{
    return this->delegate() == FutureStatusKind::Ready;
}

bool FutureStatus::IsFailed() const
{
    return this->delegate() == FutureStatusKind::Failed;
}

bool FutureStatus::IsTimeout() const
{
    return this->delegate() == FutureStatusKind::Timeout;
}

BaseLib::GeneralException FutureStatus::exception() const
{
    return exception_;
}

// ---------------------------------------
// Constructor functions
// ---------------------------------------

FutureStatus FutureStatus::Deferred()
{
    return FutureStatus(FutureStatusKind::Deferred);
}

FutureStatus FutureStatus::Ready()
{
    return FutureStatus(FutureStatusKind::Ready);
}

FutureStatus FutureStatus::Failed(BaseLib::GeneralException exception)
{
    return FutureStatus(FutureStatusKind::Failed, exception);
}

FutureStatus FutureStatus::Timeout()
{
    return FutureStatus(FutureStatusKind::Timeout);
}

}}
