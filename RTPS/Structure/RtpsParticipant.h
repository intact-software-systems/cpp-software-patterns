/*
 * RtpsParticipant.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#ifndef RTPS_Structure_RtpsParticipant_h_IsIncluded
#define RTPS_Structure_RtpsParticipant_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Structure/RtpsEntity.h"
#include"RTPS/Structure/RtpsEndpoint.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*------------------------------------------------------------------------------------------------------------
---
Every Participant has GUID <prefix, ENTITYID_PARTICIPANT>, where the constant ENTITYID_PARTICIPANT is a
special value defined by the RTPS protocol. Its actual value depends on the PSM.
The implementation is free to chose the prefix, as long as every Participant in the Domain has a unique GUID.
---
---
The Endpoints contained by a Participant with GUID <participantPrefix, ENTITYID_PARTICIPANT> have the GUID
<participantPrefix, entityId>. The entityId is the unique identification of the Endpoint relative to the Participant. This has
several consequences:
• The GUIDs of all the Endpoints within a Participant have the same prefix.
• Once the GUID of an Endpoint is known, the GUID of the Participant that contains the endpoint is also known.
• The GUID of any endpoint can be deduced from the GUID of the Participant to which it belongs and its entityId.

The selection of entityId for each RTPS Entity depends on the PSM.
---

RTPS Participant is the container of RTPS Endpoint entities and maps to a DDS DomainParticipant. In addition, the
RTPS Participant facilitates the fact that the RTPS Endpoint entities within a single RTPS Participant are likely to share
common properties.
------------------------------------------------------------------------------------------------------------*/


/**
 * @brief The RtpsParticipant class
 *
 * TODO:
 *  - Each DDS DomainParticipant holds an RtpsParticipant
 *  - An RtpsParticipant maps to all RtpsEndpoints
 *  - Each RtpsEndpoint is either a reader/writer
 *  - Each reader receives data from one or more remote writers
 *  - Each writer writes to one or more remote readers
 *
 *  The RTPS Simple Participant Discovery Protocol (SPDP) uses a simple approach to announce
 *  and detect the presence of Participants in a domain. For each Participant, the SPDP creates
 *  two RTPS built-in Endpoints: the SPDPbuiltinParticipantWriter and the SPDPbuiltinParticipantReader.
 *
 */
class DLL_STATE RtpsParticipant : public RtpsEntity
{
public:
    typedef std::map<GUID, RtpsEndpoint::Ptr>	MapGUIDEndpoint;

public:
    RtpsParticipant();
    RtpsParticipant(const GUID &guid);
    RtpsParticipant(const GUID &guid, const ProtocolVersion &protocolVersion, const VendorId &vendorId);
    virtual ~RtpsParticipant();

    CLASS_TRAITS(RtpsParticipant)

    bool AddEndpoint(RtpsEndpoint::Ptr endpoint);


public:
    const ProtocolVersion&	GetProtocolVersion()	const { return protocolVersion_; }
    const VendorId			GetVendorId()			const { return vendorId_; }

    void SetProtocolVersion(const ProtocolVersion &protocolVersion)		{ protocolVersion_ = protocolVersion; }
    void SetVendorId(const VendorId &vendorId)							{ vendorId_ = vendorId; }

private:
    ProtocolVersion protocolVersion_;
    VendorId		vendorId_;
    LocatorList		defaultUnicastLocatorList_;
    LocatorList		defaultMulticastLocatorList_;

    MapGUIDEndpoint	mapGUIDEndpoints_;
};

} // namespace RTPS

#endif // RTPS_RtpsParticipant_h_IsIncluded
