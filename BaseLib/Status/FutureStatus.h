#ifndef BaseLib_Status_FutureStatus_h_Included
#define BaseLib_Status_FutureStatus_h_Included

#include <BaseLib/Exception.h>
#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/MethodInterfaces.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

namespace FutureStatusKind
{
    enum Type
    {
        Deferred,
        Ready,
        Failed,
        Timeout
    };
}

DLL_STATE std::ostream &operator<<(std::ostream& ostr, const FutureStatusKind::Type& kind);

class DLL_STATE FutureStatus
        : Templates::ComparableImmutableType<FutureStatusKind::Type>
        , Templates::IsDeferredMethod
        , Templates::IsReadyMethod
        , Templates::IsFailedMethod
        , Templates::IsTimeoutMethod
{
public:
    FutureStatus(FutureStatusKind::Type kind, BaseLib::GeneralException exception = BaseLib::GeneralException(""));
    virtual ~FutureStatus();

    const FutureStatusKind::Type& Kind() const;

    // ---------------------------------------
    // Convenience functions
    // ---------------------------------------

    virtual bool IsDeferred() const;
    virtual bool IsReady() const;
    virtual bool IsFailed() const;
    virtual bool IsTimeout() const;

    BaseLib::GeneralException exception() const;

    // ---------------------------------------
    // Constructor functions
    // ---------------------------------------

    static FutureStatus Deferred();

    static FutureStatus Ready();

    static FutureStatus Failed(BaseLib::GeneralException exception = BaseLib::GeneralException(""));

    static FutureStatus Timeout();


    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const FutureStatus &status)
    {
        ostr << status.Clone();
        return ostr;
    }

private:
    BaseLib::GeneralException exception_;
};

}}

#endif
