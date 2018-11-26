/*
 * CommonDefines.h
 *
 *  Created on: Mar 11, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_CommonDefines_h_IsIncluded
#define RTPS_CommonDefines_h_IsIncluded

#include"BaseLib/CommonDefines.h"

namespace RTPS
{
/*--------------------------------------------------------------------------------------------
    typedef char octet

    Alternatives:
    typedef int8_t octet
    typedef uint8_t octet
    //typedef std::vector<char> sequence;
---------------------------------------------------------------------------------------------*/
typedef char 					octet;
typedef unsigned short 			SubmessageLength;
typedef std::vector<octet>  	ByteSeq;

#define OMG_CDR_BE		0x0000		// big-endian
#define OMG_CDR_LE		0x0001		// little-endian
#define OMG_PL_CDR_BE	0x0002		// parameter-list big-endian
#define OMG_PL_CDR_LE	0x0003		// parameter-list little-endian

/*-------------------------------------------------------
    Sizes, etc, should be moved to configurable class?
---------------------------------------------------------*/
const unsigned int          DATAGRAM_LENGTH  = 1024;


/*-------------------------------------------------------
    Submessage flags and bit position
---------------------------------------------------------*/
#define SUBMESSAGE_HEADER_ENDIAN_FLAG	0x01	// SubmessageHeader endian flag bit

#define ACKNACK_FINAL_FLAG 				0x02	// AckNack submessage: Final flag bit

#define DATA_INLINE_QOS_FLAG			0x02	// Data submessage: Inline QoS Flag bit
#define DATA_SERIALIZED_DATA_FLAG		0x04	// Data submessage: Has serialized data flag bit
#define DATA_KEY_DATA_FLAG				0x08	// Data submessage: Has serialized data key flag bit

#define DATAFRAG_INLINE_QOS_FLAG		0x02	// DataFrag submessage: Inline QoS Flag bit
#define DATAFRAG_SERIALIZED_KEY_FLAG	0x04	// DataFrag submessage: Has serialized data/key flag bit

#define HEARTBEAT_FINAL_FLAG			0x02	// Heartbeat submessage: Final flag bit
#define HEARTBEAT_LIVELINESS_FLAG		0x04	// Heartbeat submessage: Liveliness flag bit

#define INFO_TIMESTAMP_INVALIDATE_FLAG	0x02	// InfoTimestamp submessage: Liveliness flag bit

#define INFO_REPLY_MULTICAST_FLAG      0x02    // InfoReply submessage: Multicast flag bit

#define STATUS_INFO_DISPOSE_FLAG       0x01    // StatusInfo element: Dispose flag bit
#define STATUS_INFO_UNREGISTER_FLAG    0x02    // StatusInfo element: Unregister flag bit

/*-------------------------------------------------------
Enumeration used to determine ReaderKind.
---------------------------------------------------------*/
namespace ReaderKind
{
enum Type
{
    Undefined,
    Stateless,
    Stateful
};
}
/*-------------------------------------------------------
Enumeration used to determine WriterKind.
---------------------------------------------------------*/
namespace WriterKind
{
enum Type
{
    Undefined,
    Stateless,
    Stateful
};
}

/*-------------------------------------------------------
Enumeration used to indicate the status of a ChangeForReader.
---------------------------------------------------------*/
namespace ChangeForReaderStatusKind
{
enum Type
{
    UNSENT,
    UNACKNOWLEDGED,
    REQUESTED,
    ACKNOWLEDGED,
    UNDERWAY
};
}

/*-------------------------------------------------------
Enumeration used to indicate the status of a ChangeFromWriter.
---------------------------------------------------------*/
namespace ChangeFromWriterStatusKind
{
enum Type
{
    LOST,
    MISSING,
    RECEIVED,
    UNKNOWN
};
}

/*-------------------------------------------------------
Type used to represent the identity of a data-object whose changes in value are
communicated by the RTPS protocol.
---------------------------------------------------------*/
// TODO: Implement InstanceHandle

/*-------------------------------------------------------
Type used to hold data exchanged between Participants. The most notable use
of this type is for the Writer Liveliness Protocol.
---------------------------------------------------------*/
// TODO: Implement ParticipantMessageData

} // namespace RTPS

#endif // RTPS_CommonDefines_h_IsIncluded
