#ifndef RTPS_Discovery_DiscoveredWriterData_h_IsIncluded
#define RTPS_Discovery_DiscoveredWriterData_h_IsIncluded

#include"RTPS/Stateful/WriterProxy.h"
#include"RTPS/Export.h"

namespace RTPS
{

/*
 struct DiscoveredWriterData {
       struct DDS::PublicationBuiltinTopicData ddsPublicationData;
       struct WriterProxy writerProxy;
};
*/

/**
 * @brief The DiscoveredWriterData class
 */
class DLL_STATE DiscoveredWriterData
{
public:
    DiscoveredWriterData();

private:
    // TODO: struct DDS::PublicationBuiltinTopicData ddsPublicationData;
    //WriterProxy writerProxy_;
};

}

#endif
