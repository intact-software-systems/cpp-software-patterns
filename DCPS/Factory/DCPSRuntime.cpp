#include "DCPS/Factory/DCPSRuntime.h"
#include "DCPS/Factory/DCPSFactory.h"
#include "DCPS/Domain/DomainFwd.h"
#include "DCPS/Sub/SubFwd.h"

#include "DCPS/Domain/DomainParticipantFactory.h"

namespace DCPS
{

Singleton<DCPSRuntime> DCPSRuntime::instance_;

/**
 * @brief DCPSRuntime::DCPSRuntime
 */
DCPSRuntime::DCPSRuntime()
    : isInitialized_(false)
{ }

/**
 * @brief DCPSRuntime::~DCPSRuntime
 */
DCPSRuntime::~DCPSRuntime()
{ }

/**
 * @brief DCPSRuntime::Instance
 * @return
 */
DCPSRuntime &DCPSRuntime::Instance()
{
    return instance_.GetRef();
}

/**
 * @brief DCPSRuntime::Initialize
 * @return
 */
bool DCPSRuntime::Initialize()
{
    Domain::DomainParticipant::Ptr domainParticipant = Domain::DomainParticipantFactory::Instance().CreateParticipant();

    Factory::DCPSFactory::CreateBuiltinSubscriber(domainParticipant);

    return true;
}

}
