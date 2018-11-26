/*
 * DataWriterListener.h
 *
 *  Created on: 25. september 2012
 *      Author: KVik
 */

#ifndef DCPS_Publication_DataWriterListener_h_IsIncluded
#define DCPS_Publication_DataWriterListener_h_IsIncluded

#include"DCPS/Pub/PubFwd.h"
#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Publication
{

/**
 * @brief
 *
 * Since a DataWriter is a kind of Entity, it has the ability to have a listener associated with it. In this case, the associated
 * listener should be of concrete type DataWriterListener. The use of this listener and its relationship to changes in the
 * communication status of the DataWriter is described in Section 7.1.4, "Listeners, Conditions, and Wait-sets," on page 120.
 */
template <typename DATA>
class DataWriterListener : public DCPS::Infrastructure::Listener
{
public:
	virtual ~DataWriterListener() { }

	CLASS_TRAITS(DataWriterListener)

	virtual void OnOfferedIncompatibleQos(DataWriter<DATA> *writer, const Status::OfferedIncompatibleQosStatus &status) = 0;
	virtual void OnOfferedDeadlineMissed(DataWriter<DATA>* writer, const Status::OfferedDeadlineMissedStatus &status) = 0;
	virtual void OnLivelinessLost(DataWriter<DATA>* writer, const Status::LivelinessLostStatus &status) = 0;
	virtual void OnPublicationMatch(DataWriter<DATA>* writer, const Status::PublicationMatchedStatus &status) = 0;
};

template <typename DATA>
class NoOpDataWriterListener : public DataWriterListener<DATA>
{
public:
	virtual ~NoOpDataWriterListener() { }

	CLASS_TRAITS(NoOpDataWriterListener)

	virtual void OnOfferedIncompatibleQos(DataWriter<DATA> *writer, const Status::OfferedIncompatibleQosStatus &status) { }
	virtual void OnOfferedDeadlineMissed(DataWriter<DATA>* writer, const Status::OfferedDeadlineMissedStatus &status) { }
	virtual void OnLivelinessLost(DataWriter<DATA>* writer, const Status::LivelinessLostStatus &status) { }
	virtual void OnPublicationMatch(DataWriter<DATA>* writer, const Status::PublicationMatchedStatus &status) { }
};

}} // namespace DCPS::Publication

#endif // DCPS_Publication_DataWriterListener_h_IsIncluded

