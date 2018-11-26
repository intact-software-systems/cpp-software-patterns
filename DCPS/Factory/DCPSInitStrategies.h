#ifndef DCPS_Factory_DCPSInitStrategies_h_IsIncluded
#define DCPS_Factory_DCPSInitStrategies_h_IsIncluded

#include"DCPS/Factory/DCPSInit.h"
#include"DCPS/Pub/DataWriter.h"
#include"DCPS/Sub/DataReader.h"

#include"DCPS/Machine/CacheWriter.h"
#include"DCPS/Machine/CacheReader.h"

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Factory
{

// ------------------------------------------------------------------------------
// Example implementations of configuration of RTPS into DCPS entities
// ------------------------------------------------------------------------------

/**
 * @brief The RTPSDomainParticipantInit class
 */
class DLL_STATE RTPSDomainParticipantInit : public DomainParticipantInit
{
public:
    RTPSDomainParticipantInit();
    virtual ~RTPSDomainParticipantInit();

    virtual bool Init(DCPS::Domain::DomainParticipantPtr &participant);
};

/**
 * @brief The RTPSPublisherInit class
 */
class DLL_STATE RTPSPublisherInit : public PublisherInit
{
public:
    RTPSPublisherInit();
    virtual ~RTPSPublisherInit();

    virtual bool Init(Publication::PublisherPtr &);
};

/**
 * @brief The RTPSSubscriberInit class
 */
class DLL_STATE RTPSSubscriberInit : public SubscriberInit
{
public:
    RTPSSubscriberInit();
    virtual ~RTPSSubscriberInit();

    virtual bool Init(Subscription::SubscriberPtr &);
};

/**
 * @brief The RTPSDataReaderInit class
 */
template <typename T>
class DLL_STATE RTPSDataReaderInit : public DataReaderInit<T>
{
public:
    RTPSDataReaderInit() {}
    virtual ~RTPSDataReaderInit() {}

    virtual bool Init(typename DCPS::Subscription::DataReader<T>::Ptr &reader)
    {
        return true;
    }
};

/**
 * @brief The RTPSDataWriterInit class
 */
template <typename T>
class DLL_STATE RTPSDataWriterInit : public DataWriterInit<T>
{
public:
    RTPSDataWriterInit() {}
    virtual ~RTPSDataWriterInit() {}

    virtual bool Init(typename DCPS::Publication::DataWriter<T>::Ptr &writer)
    {
        //RTPS::VirtualMachineWriter::Ptr virtualMachineRtpsWriter = RTPS::RtpsFactory::NewVirtualMachine()
        //TODO: User DomainParticipant to get GUID

        //DCPS::Publication::CacheWriter cache();
        //DCPS::CacheWriter cache();
        //writer->GetConfig().SetCacheWriter();

        return true;
    }
};

}}

#endif

