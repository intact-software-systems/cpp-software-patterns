/*
 * ProtocolId.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ProtocolId_h_IsIncluded
#define RTPS_Elements_ProtocolId_h_IsIncluded

namespace RTPS
{

/*--------------------------------------------------------------------------------------------
	Mapping of the type ProtocolId_t

enum ProtocolId_t
{
	NoProtocol = 0
	IntactProtocol,
	RTPSProtocol
};
typedef ProtocolId_t ProtocolId;
---------------------------------------------------------------------------------------------*/
namespace ProtocolId
{
enum Type
{
	NoProtocol = 0,
	IntactProtocol,
	RTPSProtocol
};
} // namespace ProtocolId

} // namespace RTPS

#endif // RTPS_Elements_ProtocolId_h_IsIncluded
