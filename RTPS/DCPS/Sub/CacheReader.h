#ifndef RTPS_DCPS_Sub_CacheReader_h_IsIncluded
#define RTPS_DCPS_Sub_CacheReader_h_IsIncluded

#include"RTPS/DCPS/Interfaces/CacheReader.h"
#include"RTPS/Machine/VirtualMachine.h"
#include"RTPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
* @brief The DDS writer communicates with RTPS using these functions
*
* TODO: Maybe rename to CacheReaderToRTPS, CacheReaderRtps
*
* TODO: Use RxData cache approaches instead
* - This CacheReader is a wrapper around RxData's object reader and
* abstracts low level object managing details out of data-reader
*
* @author KVik
*/
class DLL_STATE CacheReader : public DCPS::Interfaces::CacheReader
{
public:
    CacheReader();
    CacheReader(const RTPS::VirtualMachineReader::Ptr &virtualMachineRtpsReader);
    virtual ~CacheReader();

public:
    /*------------------------------------------------------------------------------------
    This transition is triggered by the act of reading data from the DDS DataReader "the_dds_reader" by means of the "read"
    operation. Changes returned to the application remain in the RTPS Reader’s HistoryCache such that subsequent read or
    take operations can find them again.
    The transition performs the following logical actions in the virtual machine:

    the_rtps_reader := the_dds_reader.related_rtps_reader;
    a_change_list := new();

    FOREACH change IN the_rtps_reader.reader_cache.changes
    {
        if DDS_FILTER(the_rtps_reader, change) ADD change TO a_change_list;
    }

    RETURN a_change_list;

    The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on
    CacheChange::kind, QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the
    behavior and not necessarily the actual implementation of the protocol.
    ------------------------------------------------------------------------------------*/
    virtual std::vector<SerializeReader::Ptr> Read();

    /*------------------------------------------------------------------------------------
    This transition is triggered by the act of reading data from the DDS DataReader ‘the_dds_reader’ by means of the ‘take’
    operation. Changes returned to the application are removed from the RTPS Reader’s HistoryCache such that subsequent
    read or take operations do not find the same change.
    The transition performs the following logical actions in the virtual machine:

    the_rtps_reader := the_dds_reader.related_rtps_reader;
    a_change_list := new();
    FOREACH change IN the_rtps_reader.reader_cache.changes
    {
        if DDS_FILTER(the_rtps_reader, change)
        {
            ADD change TO a_change_list
        }
        the_rtps_reader.reader_cache.remove_change(a_change);
    }
    RETURN a_change_list;

    The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on
    CacheChange::kind, QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the
    behavior and not necessarily the actual implementation of the protocol.

    After the transition the following post-conditions hold:

    FOREACH change IN a_change_list
        change BELONGS_TO the_rtps_reader.reader_cache.changes == FALSE
    ------------------------------------------------------------------------------------*/
    virtual std::vector<SerializeReader::Ptr> Take();

    /*------------------------------------------------------------------------------------
    This transition is triggered by the destruction of a DDS DataReader "the_dds_reader".
    The transition performs the following logical actions in the virtual machine:
    delete the_dds_reader.related_rtps_reader;
    ------------------------------------------------------------------------------------*/
    virtual bool Delete();

private:
    /*------------------------------------------------------------------------------------
      Apply some "filter" conditions to remove unwanted cache-changes
    ------------------------------------------------------------------------------------*/
    bool DoFilterOut(const RTPS::CacheChange::Ptr &cacheChange);

private:
    RTPS::VirtualMachineReader::Ptr virtualMachineRtpsReader_;
};

}}

#endif // RTPS_DCPS_Sub_CacheReader_h_IsIncluded
