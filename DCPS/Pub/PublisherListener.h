/*
 * PublisherListener.h
 *
 *  Created on: 25. september 2012
 *      Author: KVik
 */

#ifndef DCPS_Publication_PublisherListener_h_IsIncluded
#define DCPS_Publication_PublisherListener_h_IsIncluded

#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Pub/DataWriterListener.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Publication
{

/**
@description

Since a DataWriter is a kind of Entity, it has the ability to have a listener associated with it. In this case, the associated
listener should be of concrete type PublisherListener. The use of this listener and its relationship to changes in the
communication status of the DataWriter is described in Section 7.1.4, “Listeners, Conditions, and Wait-sets,” on page 120.
*/
class DLL_STATE PublisherListener : public virtual DCPS::Infrastructure::Listener //AnyDataWriterListener
{
public:
	PublisherListener() { }
	virtual ~PublisherListener() { }

	CLASS_TRAITS(PublisherListener)
};

/*
class DLL_STATE NoOpPublisherListener : public virtual PublisherListener,
										public virtual NoOpAnyDataWriterListener
{
public:
	PublisherListener() { }
	virtual ~PublisherListener() { }

	CLASS_TRAITS(PublisherListener)
};
*/

}} // namespace DCPS::Publication

#endif // DCPS_Publication_PublisherListener_h_IsIncluded

