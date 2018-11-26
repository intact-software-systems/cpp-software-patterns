#ifndef DCPS_Factory_DCPSInit_h_IsIncluded
#define DCPS_Factory_DCPSInit_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Factory
{

// ------------------------------------------------------------------------------
// Interfaces to be implemented by individual configuration setups: RTPS, etc.
// ------------------------------------------------------------------------------

class DLL_STATE DomainParticipantInit : public BaseLib::Templates::InitTypeMethod<DCPS::Domain::DomainParticipantPtr>
{
public:
    virtual ~DomainParticipantInit() {}

    CLASS_TRAITS(DomainParticipantInit)
};

class DLL_STATE PublisherInit : public BaseLib::Templates::InitTypeMethod<DCPS::Publication::PublisherPtr>
{
public:
    virtual ~PublisherInit() { }

    CLASS_TRAITS(PublisherInit)
};

class DLL_STATE SubscriberInit : public BaseLib::Templates::InitTypeMethod<DCPS::Subscription::SubscriberPtr>
{
public:
    virtual ~SubscriberInit() {}

    CLASS_TRAITS(SubscriberInit)
};

template <typename T>
class DataReaderInit : public BaseLib::Templates::InitTypeMethod<typename DCPS::Subscription::DataReader<T>::Ptr>
{
public:
    virtual ~DataReaderInit() {}

    CLASS_TRAITS(DataReaderInit)
};

template <typename T>
class DataWriterInit : public BaseLib::Templates::InitTypeMethod<typename DCPS::Publication::DataWriter<T>::Ptr>
{
public:
    virtual ~DataWriterInit() {}

    CLASS_TRAITS(DataWriterInit)
};

}}

#endif
