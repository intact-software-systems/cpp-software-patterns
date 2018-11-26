#ifndef DCPS_Status_StatusFwd_h_Included
#define DCPS_Status_StatusFwd_h_Included

#include "DCPS/Status/StatusImpl.h"

namespace DCPS { namespace Status
{

/*namespace Impl
{
    class InconsistentTopicStatusImpl;
    class LivelinessChangedStatusImpl;
    class LivelinessLostStatusImpl;
    class OfferedDeadlineMissedStatusImpl;
    class OfferedIncompatibleQosStatusImpl;
    class PublicationMatchedStatusImpl;
    class RequestedDeadlineMissedStatusImpl;
    class RequestedIncompatibleQosStatusImpl;
    class SampleLostStatusImpl;
    class SampleRejectedStatusImpl;
    class SubscriptionMatchedStatusImpl;
}

namespace Template
{
    template <typename D>
    class InconsistentTopicStatus;

    template <typename D>
    class SampleLostStatus;

    template <typename D>
    class SampleRejectedStatus;

    template <typename D>
    class LivelinessLostStatus;

    template <typename D>
    class LivelinessChangedStatus;

    template <typename D>
    class OfferedDeadlineMissedStatus;

    template <typename D>
    class RequestedDeadlineMissedStatus;

    template <typename D>
    class OfferedIncompatibleQosStatus;

    template <typename D>
    class RequestedIncompatibleQosStatus;

    template <typename D>
    class PublicationMatchedStatus;

    template <typename D>
    class SubscriptionMatchedStatus;
}*/

/*namespace detail
{
    typedef DCPS::Status::Template::InconsistentTopicStatus< DCPS::Status::Impl::InconsistentTopicStatusImpl >
    InconsistentTopicStatus;

    typedef DCPS::Status::Template::SampleLostStatus< DCPS::Status::Impl::SampleLostStatusImpl >
    SampleLostStatus;

    typedef DCPS::Status::Template::SampleRejectedStatus< DCPS::Status::Impl::SampleRejectedStatusImpl >
    SampleRejectedStatus;

    typedef DCPS::Status::Template::LivelinessLostStatus< DCPS::Status::Impl::LivelinessLostStatusImpl >
    LivelinessLostStatus;

    typedef DCPS::Status::Template::LivelinessChangedStatus< DCPS::Status::Impl::LivelinessChangedStatusImpl >
    LivelinessChangedStatus;

    typedef DCPS::Status::Template::OfferedDeadlineMissedStatus< DCPS::Status::Impl::OfferedDeadlineMissedStatusImpl >
    OfferedDeadlineMissedStatus;

    typedef DCPS::Status::Template::RequestedDeadlineMissedStatus< DCPS::Status::Impl::RequestedDeadlineMissedStatusImpl >
    RequestedDeadlineMissedStatus;

    typedef DCPS::Status::Template::OfferedIncompatibleQosStatus< DCPS::Status::Impl::OfferedIncompatibleQosStatusImpl >
    OfferedIncompatibleQosStatus;

    typedef DCPS::Status::Template::RequestedIncompatibleQosStatus< DCPS::Status::Impl::RequestedIncompatibleQosStatusImpl >
    RequestedIncompatibleQosStatus;

    typedef DCPS::Status::Template::PublicationMatchedStatus< DCPS::Status::Impl::PublicationMatchedStatusImpl >
    PublicationMatchedStatus;

    typedef DCPS::Status::Template::SubscriptionMatchedStatus< DCPS::Status::Impl::SubscriptionMatchedStatusImpl >
    SubscriptionMatchedStatus;

} // DCPS::Status::detail*/

namespace detail
{
    typedef DCPS::Status::Impl::InconsistentTopicStatusImpl        InconsistentTopicStatus;
    typedef DCPS::Status::Impl::SampleLostStatusImpl               SampleLostStatus;
    typedef DCPS::Status::Impl::SampleRejectedStatusImpl           SampleRejectedStatus;
    typedef DCPS::Status::Impl::LivelinessLostStatusImpl           LivelinessLostStatus;
    typedef DCPS::Status::Impl::LivelinessChangedStatusImpl        LivelinessChangedStatus;
    typedef DCPS::Status::Impl::OfferedDeadlineMissedStatusImpl    OfferedDeadlineMissedStatus;
    typedef DCPS::Status::Impl::RequestedDeadlineMissedStatusImpl  RequestedDeadlineMissedStatus;
    typedef DCPS::Status::Impl::OfferedIncompatibleQosStatusImpl   OfferedIncompatibleQosStatus;
    typedef DCPS::Status::Impl::RequestedIncompatibleQosStatusImpl RequestedIncompatibleQosStatus;
    typedef DCPS::Status::Impl::PublicationMatchedStatusImpl       PublicationMatchedStatus;
    typedef DCPS::Status::Impl::SubscriptionMatchedStatusImpl      SubscriptionMatchedStatus;

} // DCPS::Status::detail

class DataAvailableStatus;

class DataOnReadersStatus;

class StatusMask;

class SampleRejectedState;

typedef DCPS::Status::detail::InconsistentTopicStatus
InconsistentTopicStatus;

typedef DCPS::Status::detail::LivelinessChangedStatus
LivelinessChangedStatus;

typedef DCPS::Status::detail::LivelinessLostStatus
LivelinessLostStatus;

typedef DCPS::Status::detail::OfferedDeadlineMissedStatus
OfferedDeadlineMissedStatus;

typedef DCPS::Status::detail::OfferedIncompatibleQosStatus
OfferedIncompatibleQosStatus;

typedef DCPS::Status::detail::PublicationMatchedStatus
PublicationMatchedStatus;

typedef DCPS::Status::detail::SampleRejectedStatus
SampleRejectedStatus;

typedef DCPS::Status::detail::RequestedDeadlineMissedStatus
RequestedDeadlineMissedStatus;

typedef DCPS::Status::detail::RequestedIncompatibleQosStatus
RequestedIncompatibleQosStatus;

typedef DCPS::Status::detail::SampleLostStatus
SampleLostStatus;

typedef DCPS::Status::detail::SubscriptionMatchedStatus
SubscriptionMatchedStatus;

}} // namespace DCPS::Status

#endif // DCPS_Status_StatusFwd_h_Included

