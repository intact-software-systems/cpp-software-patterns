#ifndef DDS_QosPolicy_PolicyKind_h_Included
#define DDS_QosPolicy_PolicyKind_h_Included

namespace DDS { namespace Policy {

namespace OwnershipKind {
enum Type {
	SHARED,
	EXCLUSIVE
}; }

namespace DurabilityKind {
enum Type {
	VOLATILE,
	TRANSIENT_LOCAL,
	TRANSIENT,
	PERSISTENT
}; }

namespace PresentationAccessScopeKind  {
enum Type {
	INSTANCE,
	TOPIC,
	GROUP
}; }


namespace ReliabilityKind {
enum Type {
	BEST_EFFORT,
	RELIABLE
}; }

namespace DestinationOrderKind {
enum Type {
	BY_RECEPTION_TIMESTAMP,
	BY_SOURCE_TIMESTAMP
}; }

namespace HistoryKind {
enum Type {
	KEEP_LAST,
	KEEP_ALL
};}

namespace LivelinessKind {
enum Type {
	AUTOMATIC,
	MANUAL_BY_PARTICIPANT,
	MANUAL_BY_TOPIC
}; }

namespace TypeConsistencyEnforcementKind {
enum Type {
	EXACT_TYPE_TYPE_CONSISTENCY,
	EXACT_NAME_TYPE_CONSISTENCY,
	DECLARED_TYPE_CONSISTENCY,
	ASSIGNABLE_TYPE_CONSISTENCY
}; }

/**
  * @brief
  *
  * @knuthelv Defines the DDS RxO property for QoS policies
  */
namespace RequestedOfferedKind {
enum Type {
	COMPATIBLE,
	INDEPENDENT,
	NOT_APPLICABLE
}; }

}} // namespace DDS::Policy

#endif // DDS_QosPolicy_PolicyKind_h_Included

