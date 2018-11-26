#ifndef DCPS_Publication_DataWriterStatus_h_IsIncluded
#define DCPS_Publication_DataWriterStatus_h_IsIncluded

#include"DCPS/Status/IncludeLibs.h"

#include"DCPS/Export.h"
namespace DCPS { namespace Publication
{

/**
 * @brief The DataWriterStatus class is a container for all status objects relevant for a DataWriter.
 */
class DLL_STATE DataWriterStatus
{
public:
    DataWriterStatus();
    ~DataWriterStatus();

    template <typename STATUS>
    const STATUS& status() const;

    template <typename STATUS>
    STATUS& status();

    // --------------------------------------------------------------
	//  Status Management
	// --------------------------------------------------------------

	const Status::LivelinessLostStatus&             GetLivelinessLostStatus()           const { return livelinessLostStatus_; }
	const Status::OfferedDeadlineMissedStatus&      GetOfferedDeadlineMissedStatus() 	const { return offeredDeadlineMissedStatus_; }
	const Status::OfferedIncompatibleQosStatus&     GetOfferedIncompatibleQosStatus()   const { return offeredIncompatibleQosStatus_; }
	const Status::PublicationMatchedStatus&         GetPublicationMatchedStatus() 		const { return publicationMatchedStatus_;}

private:
    Status::LivelinessLostStatus				livelinessLostStatus_;
	Status::OfferedDeadlineMissedStatus			offeredDeadlineMissedStatus_;
	Status::OfferedIncompatibleQosStatus		offeredIncompatibleQosStatus_;
	Status::PublicationMatchedStatus			publicationMatchedStatus_;
};

}}

#endif
