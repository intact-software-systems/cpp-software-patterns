#ifndef DDS_Policy_CoreFwd_h_IsIncluded
#define DDS_Policy_CoreFwd_h_IsIncluded

#include"DDS/CommonDefines.h"

namespace DDS { namespace Policy
{
    class DataRepresentation;

    class Deadline;

    class DestinationOrder;

    class Durability;

    class DurabilityService;

    class EntityFactory;

    class GroupData;

    class History;

    class LatencyBudget;

    class Lifespan;

    class Liveliness;

    class Ownership;

    class OwnershipStrength;

    class Partition;

    class Presentation;

    class QosPolicyCount;

    class ReaderDataLifecycle;

    class Reliability;

    class ResourceLimits;

    class TimeBasedFilter;

    class TopicData;

    class TransportPriority;

    class TypeConsistencyEnforcement;

    class UserData;

    class WriterDataLifecycle;

    typedef uint16_t DataRepresentationId;
    typedef std::vector<DataRepresentationId> DataRepresentationIdSeq;

    typedef std::vector<QosPolicyCount> QosPolicyCountSeq;

    typedef uint32_t QosPolicyId;

}}

#endif

