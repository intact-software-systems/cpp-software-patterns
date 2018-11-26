/*
 * RtpsEndpoint.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */
#ifndef RTPS_Structure_RtpsEndpoint_h_IsIncluded
#define RTPS_Structure_RtpsEndpoint_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Structure/RtpsEntity.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*------------------------------------------------------------------------------------------------------------
RTPS Endpoint represents the possible communication endpoints from the point of view of the RTPS protocol. There are
two kinds of RTPS Endpoint entities: Writer endpoints and Reader endpoints.

- RTPS Writer endpoints send CacheChange messages to RTPS Reader endpoints and potentially receive
acknowledgments for the changes they send.
- RTPS Reader endpoints receive CacheChange and change-availability
announcements from Writer endpoints and potentially acknowledge the changes and/or request missed changes.
------------------------------------------------------------------------------------------------------------*/
class DLL_STATE RtpsEndpoint : public RtpsEntity
{
public:
    RtpsEndpoint();
    RtpsEndpoint(const GUID &guid);
    RtpsEndpoint(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind);
    virtual ~RtpsEndpoint();

    CLASS_TRAITS(RtpsEndpoint)

public:
    const TopicKind&				GetTopicKind() 				const { return topicKind_; }
    ReliabilityKindId::Type	        GetReliabilityKind() 		const { return reliabilityKind_.GetReliabilityKind(); }
    const LocatorList&				GetUnicastLocatorList()		const { return unicastLocatorList_; }
    const LocatorList&				GetMulticastLocatorList() 	const { return multicastLocatorList_; }

    void SetTopicKind(const TopicKind &topicKind)							{ topicKind_ = topicKind; }
    void SetReliabilityKind(const ReliabilityKind &reliabilityKind)			{ reliabilityKind_ = reliabilityKind; }
    void SetReliabilityKind(const ReliabilityKindId::Type &reliabilityKind)	{ reliabilityKind_.SetReliabilityKind(reliabilityKind); }
    void SetUnicastLocatorList(const LocatorList &l)						{ unicastLocatorList_ = l; }
    void SetMulticastLocatorList(const LocatorList &l)						{ multicastLocatorList_ = l; }

private:
    TopicKind			topicKind_;
    ReliabilityKind 	reliabilityKind_;
    LocatorList			unicastLocatorList_;
    LocatorList			multicastLocatorList_;
};

} // namespace RTPS

#endif // RTSP_Structure_RtpsEndpoint_h_IsIncluded
