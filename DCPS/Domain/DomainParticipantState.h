#ifndef DCPS_Domain_DomainParticipantState_h_IsIncluded
#define DCPS_Domain_DomainParticipantState_h_IsIncluded

#include"DCPS/Factory/DCPSConfig.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Domain {

/**
 * @brief The DomainParticipantState class gives access to data writers/reader/locators
 *
 * Note: This is kind of the DomainParticipant "Object Manager".
 */
class DLL_STATE DomainParticipantState
{
public:
    DomainParticipantState();
    DomainParticipantState(RxData::CacheDescription description);
    virtual ~DomainParticipantState();

    DCPSConfig::PublisherAccess  publishers();
    DCPSConfig::SubscriberAccess subscribers();
    DCPSConfig::TopicAccess      topics();

private:
    RxData::Cache::Ptr cache_;
};

}}

#endif
