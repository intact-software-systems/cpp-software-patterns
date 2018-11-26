/*
 * SubmessageKind.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_SubmessageKind_h_IsIncluded
#define RTPS_Elements_SubmessageKind_h_IsIncluded

namespace RTPS
{

typedef unsigned short SubmessageLength;
/*--------------------------------------------------------------------------------------------
	Mapping of the type SubmessageKind_t

enum SubmessageKind_t
{
	PAD 			= 0x01, 	// Pad
	ACKNACK 		= 0x06, 	// AckNack
	HEARTBEAT 		= 0x07, 	// Heartbeat
	GAP 			= 0x08, 	// Gap
	INFO_TS 		= 0x09, 	// InfoTimestamp
	INFO_SRC 		= 0x0c, 	// InfoSource
	INFO_REPLY_IP4 	= 0x0d, 	// InfoReplyIp4
	INFO_DST 		= 0x0e, 	// InfoDestination
	INFO_REPLY 		= 0x0f, 	// InfoReply
	NACK_FRAG 		= 0x12, 	// NackFrag
	HEARTBEAT_FRAG 	= 0x13,		// HeartbeatFrag
	DATA 			= 0x15, 	// Data
	DATA_FRAG 		= 0x16 		// DataFrag
};
---------------------------------------------------------------------------------------------*/
namespace SubmessageKind
{
enum Type
{
	UNKNOWN			= 0x00,		// Unknown
	PAD 			= 0x01, 	// Pad
	ACKNACK 		= 0x06, 	// AckNack
	HEARTBEAT 		= 0x07, 	// Heartbeat
	GAP 			= 0x08, 	// Gap
	INFO_TS 		= 0x09, 	// InfoTimestamp
	INFO_SRC 		= 0x0c, 	// InfoSource
	INFO_REPLY_IP4 	= 0x0d, 	// InfoReplyIp4
	INFO_DST 		= 0x0e, 	// InfoDestination
	INFO_REPLY 		= 0x0f, 	// InfoReply
	NACK_FRAG 		= 0x12, 	// NackFrag
	HEARTBEAT_FRAG 	= 0x13,		// HeartbeatFrag
	DATA 			= 0x15, 	// Data
	DATA_FRAG 		= 0x16 		// DataFrag
};
} // namespace SubmessageKind

} // namespace RTPS

#endif // RTPS_Elements_SubmessageKind_h_IsIncluded
