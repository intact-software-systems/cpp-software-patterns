/*
 * CommonDefines.h
 *
 *  Created on: Nov 4, 2012
 *      Author: knuthelv
 */

#ifndef DDS_CommonDefines_h_IsIncluded
#define DDS_CommonDefines_h_IsIncluded

#include"DDS/IncludeExtLibs.h"

#define DEFAULT_STATE_BIT_COUNT 		32
#define DEFAULT_STATUS_COUNT    		32

namespace DDS
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

namespace DCPS
{

//struct BuiltinTopicKey_t
//{
//	BUILTIN_TOPIC_KEY_TYPE_NATIVE value[3];
//};

// ----------------------------------------------------------------------
// Return codes
// ----------------------------------------------------------------------
namespace ReturnCode
{
enum Type
{
    Ok 						= 0,
    Error 					= 1,
    UNSUPPORTED 			= 2,
    BAD_PARAMETER 			= 3,
    PRECONDITION_NOT_MET 	= 4,
    OUT_OF_RESOURCES 		= 5,
    NOT_ENABLED 			= 6,
    IMMUTABLE_POLICY 		= 7,
    INCONSISTENT_POLICY 	= 8,
    ALREADY_DELETED 		= 9,
    TIMEOUT 				= 10,
    NODATA 					= 11,
    ILLEGAL_OPERATION 		= 12
};
}

}

#endif
