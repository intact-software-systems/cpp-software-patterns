/*
 * RtpsParticipant.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Structure/RtpsParticipant.h"

namespace RTPS
{

RtpsParticipant::RtpsParticipant()
    : RtpsEntity()
    , protocolVersion_(ProtocolVersion::PROTOCOLVERSION())
    , vendorId_(VendorId::UNKNOWN())
{ }

RtpsParticipant::RtpsParticipant(const GUID &guid)
    : RtpsEntity(guid)
    , protocolVersion_(ProtocolVersion::PROTOCOLVERSION())
    , vendorId_(VendorId::UNKNOWN())
{ }

RtpsParticipant::RtpsParticipant(const GUID &guid, const ProtocolVersion &protocolVersion, const VendorId &vendorId)
    : RtpsEntity(guid)
    , protocolVersion_(protocolVersion)
    , vendorId_(vendorId)
{ }

RtpsParticipant::~RtpsParticipant()
{ }

} // namespace RTPS
