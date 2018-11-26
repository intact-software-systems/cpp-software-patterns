#ifndef DCPS_Machine_VirtualMachineDCPS_h_IsIncluded
#define DCPS_Machine_VirtualMachineDCPS_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS
{

/**
 * @brief The VirtualMachineDCPS class
 *
 * TODO:
 *  - Check and update data-reader and writer states according to qos policies.
 *  - Create a cache with name "DCPS" and access all object-homes in that cache, which contains all the entities.
 *  - Access the DCPS entities and enforce their QoS policies
 *  - Similarly for RTPS a cache with name "RTPS"
 *
 */
class DLL_STATE VirtualMachineDCPS
{
public:
    VirtualMachineDCPS();
};

/**
 * @brief The QosPolicyChecker class
 *
 * Qos Policies has to be enforced by QoS Policy Checker Strategies. Perhaps implement the call to the strategies using the Interceptor pattern?
 */
class DLL_STATE QosPolicyStrategy
{
public:


};

}

#endif
