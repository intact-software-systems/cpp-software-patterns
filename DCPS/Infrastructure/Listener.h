/*
 * Listener.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Infrastructure_Listener_h_IsIncluded
#define DCPS_Infrastructure_Listener_h_IsIncluded

#include"DCPS/Export.h"

namespace DCPS { namespace Infrastructure
{

/**
 * @brief
 *
 * Listener is the abstract root for all Listener interfaces. All the supported kinds of concrete Listener interfaces (one per
 * concrete Entity: DomainParticipant, Topic, Publisher, DataWriter, Subscriber, and DataReader) derive from this root
 * and add methods whose prototype depends on the concrete Listener.
 *
 * See Section 7.1.4.3, Access through Listeners, on page 128 for the list of defined listener interfaces. Listener interfaces
 * provide a mechanism for the Service to asynchronously inform the application of relevant changes in the communication
 * status.
 */
class DLL_STATE Listener
{
public:
	Listener();
	virtual ~Listener();
};

}}

#endif
