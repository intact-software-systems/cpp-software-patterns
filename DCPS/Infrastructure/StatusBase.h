/*
 * StatusBase.h
 *
 *  Created on: 10. july 2012
 *      Author: KVik
 */

#ifndef DCPS_Infrastructure_StatusBase_h_IsIncluded
#define DCPS_Infrastructure_StatusBase_h_IsIncluded

namespace DCPS { namespace Infrastructure
{

/**
Status is the abstract root class for all communication status objects. All concrete kinds of Status classes specialize this
class.

Each concrete Entity is associated with a set of Status objects whose value represents the communication status of that
entity. These status values can be accessed with corresponding methods on the Entity. The changes on these status values
are the ones that both cause activation of the corresponding StatusCondition objects and trigger invocation of the proper
Listener objects to asynchronously inform the application.
Status objects and their relationship to Listener and Condition objects are detailed in Section 7.1.4.1, Communication
Status, on page 120.

*/
class StatusBase
{
public:
    StatusBase();
    virtual ~StatusBase();

};

}} // namespace DCPS::Infrastructure

#endif // DCPS_Infrastructure_StatusBase_h_IsIncluded
