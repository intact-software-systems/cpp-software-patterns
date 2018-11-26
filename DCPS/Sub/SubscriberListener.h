/*
 * SubscriberListener.h
 *
 *  Created on: 27. october 2012
 *      Author: KVik
 */

#ifndef DCPS_Subscriber_SubscriberListener_h_IsIncluded
#define DCPS_Subscriber_SubscriberListener_h_IsIncluded

#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{
class Subscriber;

/**
@description

Since a Subscriber is a kind of Entity, it has the ability to have a listener associated with it. In this case, the associated
listener should be of concrete type SubscriberListener. The use of this listener and its relationship to changes in the
communication status of the Subscriber is described in Section 7.1.4, “Listeners, Conditions, and Wait-sets,” on page 120.
*/
class DLL_STATE SubscriberListener : public DCPS::Infrastructure::Listener
{
public:
	virtual ~SubscriberListener() { }

	CLASS_TRAITS(SubscriberListener)

	virtual void OnDataOnReaders(Subscriber *subscriber) = 0;
};

class DLL_STATE NoOpSubscriberListener : public SubscriberListener
{
public:
	virtual ~NoOpSubscriberListener() { }

	CLASS_TRAITS(NoOpSubscriberListener)

	virtual void OnDataOnReaders(Subscriber *subscriber) { }
};

}} // namespace DCPS::Subscription

#endif // DCPS_Subscriber_SubscriberListener_h_IsIncluded


