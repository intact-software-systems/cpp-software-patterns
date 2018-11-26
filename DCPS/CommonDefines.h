/*
 * CommonDefines.h
 *
 *  Created on: Nov 4, 2012
 *      Author: knuthelv
 */

#ifndef DCPS_CommonDefines_h_IsIncluded
#define DCPS_CommonDefines_h_IsIncluded

#include"DCPS/Domain/DomainFwd.h"
#include"DCPS/Pub/PubFwd.h"
#include"DCPS/Sub/SubFwd.h"
#include"DCPS/Topic/TopicFwd.h"

#define DEFAULT_STATE_BIT_COUNT 		32
#define DEFAULT_STATUS_COUNT    		32

namespace DCPS
{

namespace Core
{
    typedef uint16_t DataRepresentationId;
    typedef std::vector<DataRepresentationId> DataRepresentationIdSeq;

    typedef std::vector<std::string> StringSeq;
    typedef std::vector<uint8_t> ByteSeq;
    const int32_t LENGTH_UNLIMITED = -1;
}

}

#endif // DCPS_CommonDefines_h_IsIncluded
