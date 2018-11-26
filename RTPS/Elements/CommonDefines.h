/*
 * CommonDefines.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_CommonDefines_h_IsIncluded
#define RTPS_Elements_CommonDefines_h_IsIncluded

/** ------------------------------------------------------------------
@description

Table 9.12 summarizes the list of ParameterIds used to encapsulate the
data for the built-in Entities. Table 9.13 lists the Entities to which
each parameterID applies and its default value.
--------------------------------------------------------------------*/

#define PID_PAD									0x0000
#define PID_SENTINEL							0x0001
#define PID_USER_DATA							0x002c
#define PID_TOPIC_NAME							0x0005
#define PID_TYPE_NAME							0x0007
#define PID_GROUP_DATA							0x002d
#define PID_TOPIC_DATA							0x002e
#define PID_DURABILITY							0x001d
#define PID_DURABILITY_SERVICE					0x001e
#define PID_DEADLINE							0x0023
#define PID_LATENCY_BUDGET						0x0027
#define PID_LIVELINESS							0x001b
#define PID_RELIABILITY							0x001A
#define PID_LIFESPAN							0x002b
#define PID_DESTINATION_ORDER					0x0025
#define PID_HISTORY								0x0040
#define PID_RESOURCE_LIMITS						0x0041
#define PID_OWNERSHIP							0x001f
#define PID_OWNERSHIP_STRENGTH					0x0006
#define PID_PRESENTATION						0x0021
#define PID_PARTITION							0x0029
#define PID_TIME_BASED_FILTER					0x0004
#define PID_TRANSPORT_PRIORITY					0x0049
#define PID_PROTOCOL_VERSION					0x0015
#define PID_VENDORID							0x0016
#define PID_UNICAST_LOCATOR						0x002f
#define PID_MULTICAST_LOCATOR					0x0030
#define PID_MULTICAST_IPADDRESS					0x0011
#define PID_DEFAULT_UNICAST_LOCATOR				0x0031
#define PID_DEFAULT_MULTICAST_LOCATOR			0x0048
#define PID_METATRAFFIC_UNICAST_LOCATOR			0x0032
#define PID_METATRAFFIC_MULTICAST_LOCATOR		0x0033
#define PID_DEFAULT_UNICAST_IPADDRESS			0x000c
#define PID_DEFAULT_UNICAST_PORT				0x000e
#define PID_METATRAFFIC_UNICAST_IPADDRESS		0x0045
#define PID_METATRAFFIC_UNICAST_PORT 			0x000d
#define PID_METATRAFFIC_MULTICAST_IPADDRESS 	0x000b
#define PID_METATRAFFIC_MULTICAST_PORT 			0x0046
#define PID_EXPECTS_INLINE_QOS 					0x0043
#define PID_PARTICIPANT_MANUAL_LIVELINESS_COUNT 0x0034
#define PID_PARTICIPANT_BUILTIN_ENDPOINTS 		0x0044
#define PID_PARTICIPANT_LEASE_DURATION 			0x0002
#define PID_CONTENT_FILTER_PROPERTY 			0x0035
#define PID_PARTICIPANT_GUID 					0x0050
#define PID_PARTICIPANT_ENTITYID 				0x0051
#define PID_GROUP_GUID 							0x0052
#define PID_GROUP_ENTITYID 						0x0053
#define PID_BUILTIN_ENDPOINT_SET 				0x0058
#define PID_PROPERTY_LIST 						0x0059
#define PID_TYPE_MAX_SIZE_SERIALIZED 			0x0060
#define PID_ENTITY_NAME 						0x0062
//#define PID_KEY_HASH 							0x0070
//#define PID_STATUS_INFO 						0x0071

// Inline QoS
#define PID_CONTENT_FILTER_INFO 				0x0055
#define PID_COHERENT_SET 						0x0056
#define PID_DIRECTED_WRITE 						0x0057
#define PID_ORIGINAL_WRITER_INFO 				0x0061
#define PID_KEY_HASH 							0x0070
#define PID_STATUS_INFO 						0x0071


/**
 * EntityId_t values fully predefined by the RTPS Protocol
 */
#define ENTITYID_UNKNOWN                                    {0x00,0x00,0x00,0x00}

// participant
#define ENTITYID_PARTICIPANT                                {0x00,0x00,0x01,0xc1}

// SEDPbuiltinTopicWriter
#define ENTITYID_SEDP_BUILTIN_TOPIC_WRITER                  {0x00,0x00,0x02,0xc2}

// SEDPbuiltinTopicReader
#define ENTITYID_SEDP_BUILTIN_TOPIC_READER                  {0x00,0x00,0x02,0xc7}

// SEDPbuiltinPublicationsWriter
#define ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER           {0x00,0x00,0x03,0xc2}

// SEDPbuiltinPublicationsReader
#define ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER           {0x00,0x00,0x03,0xc7}

// SEDPbuiltinSubscriptionsWriter
#define ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER          {0x00,0x00,0x04,0xc2}

// SEDPbuiltinSubscriptionsReader
#define ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER          {0x00,0x00,0x04,0xc7}

// SPDPbuiltinParticipantWriter
#define ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER            {0x00,0x01,0x00,0xc2}

// SPDPbuiltinSdpParticipantReader
#define ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER            {0x00,0x01,0x00,0xc7}

// BuiltinParticipantMessageWriter
#define ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER     {0x00,0x02,0x00,0xC2}

// BuiltinParticipantMessageReader
#define ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER     {0x00,0x02,0x00,0xC7}

/**
 * Table 9.1 - entityKind octet of an EntityId_t
 */
#define UNKNOWN_USERDEFINED_ENTITY          0x00
#define UNKOWN_BULTIN_ENTITY                0xc0
//#define USERDEFINED_PARTICIPANT            N/A

#define BUILTIN_PARTICIPANT                 0xc1

#define USERDEFINED_WRITER_WITHKEY          0x02
#define BUILTIN_WRITER_WITHKEY              0xc2

#define USERDEFINED_WRITER_NOKEY            0x03
#define BUILTIN_WRITER_NOKEY                0xc3

#define USERDEFINED_READER_WITHKEY          0x07
#define BULTIN_READER_WITHKEY               0xc7

#define USERDEFINED_READER_NOKEY            0x04
#define BUILTIN_READER_NOKEY                0xc4


/**
  * GUID
  */
#define GUIDPREFIX_UNKNOWN {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}


/*--------------------------------------------------------------------------------------------
    Mapping of the type BuiltinEndpointSet_t

typedef unsigned long BuiltinEndpointSet_t;
//where
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER 0x00000001 << 0;
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR 0x00000001 << 1;
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER 0x00000001 << 2;
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR 0x00000001 << 3;
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER 0x00000001 << 4;
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR 0x00000001 << 5;
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_ANNOUNCER 0x00000001 << 6;
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_DETECTOR 0x00000001 << 7;
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_ANNOUNCER 0x00000001 << 8;
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_DETECTOR 0x00000001 << 9;
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER 0x00000001 << 10;
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER 0x00000001 << 11;

typedef BuiltinEndpointSet_t BuiltinEndpointSet;
---------------------------------------------------------------------------------------------*/


#endif // RTPS_Elements_CommonDefines_h_IsIncluded
