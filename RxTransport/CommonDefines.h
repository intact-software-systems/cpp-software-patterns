#pragma once

#include <future>

#include "RxTransport/IncludeExtLibs.h"
#include "RxTransport/Export.h"
#include "ChannelFeedbackEvent.h"

namespace BaseLib { namespace Concurrent {

/**
 * - MessageFactory (monitoring)
 * - Move multicast strategies from observer to message implementation
 *
 * Rules (policy, status) = action implemented by specific strategy
 *
 * RxTransport must support:
 *  - Unicast - one to one
 *  - Multicast - one to many
 *  - Request/Reply (Request with UUID and Reply with same UUID, correlation id)
 *  - best effort and reliable
 *  - Provide Return Address
 *  - Message expiration time (message itself times out), support by RxTransport via MessagePolicy.
 */

// ----------------------------------------------------------------
// Forward declarations
// ----------------------------------------------------------------

class MessageControllerBase;

template <typename T>
class DataMessage;

template <typename T>
class Message;

template <typename T>
class MessageController;

template <typename T>
class MessageReader;

template <typename T>
class MessageWriter;

template <typename T>
class MessageWriterData;

class MessageWriterProxy;

class MessageReaderProxy;

template <typename T>
class MessageChannel;

template <typename T>
class MessageCache;

class ChannelFactory;

// ----------------------------------------------------------------
// Enum classes
// ----------------------------------------------------------------

// Reader, writer and controller share exact same base classes, use this to distinguish
enum class DLL_STATE ChannelAccessorKind : char
{
    READER     = 'R',
    WRITER     = 'W',
    PUBLISHER  = 'P',
    SUBSCRIBER = 'S',
    NO         = '-'
};

enum class DLL_STATE MessageKind : char
{
    Data      = 'D',
    AckNack   = 'A',
    Heartbeat = 'H',
    Request   = 'R',
    Gap       = 'G'
};

enum class DLL_STATE ChangeFromWriterStatusKind : char
{
    LOST     = 'L',
    MISSING  = 'M',
    RECEIVED = 'R',
    UNKNOWN  = 'U',
    FILTERED = 'F',     // TODO: Applicable if RxTransport support filters in reader/writer proxies
    NO       = '-'
};

enum class DLL_STATE ChangeForReaderStatusKind : char
{
    UNSENT         = 'U',
    UNACKNOWLEDGED = 'N',
    REQUESTED      = 'R',
    ACKNOWLEDGED   = 'A',
    UNDERWAY       = 'W',
    FILTERED       = 'F',     // TODO: Applicable if RxTransport support filters in reader/writer proxies
    NO             = '-'
};

enum class DLL_STATE TransmissionStatusKind : char
{
    CREATED      = 'C',
    ROUTED       = 'T',
    UNDERWAY     = 'W',
    RECEIVED     = 'R',
    ACKNOWLEDGED = 'A',
    FILTERED     = 'F',
    NO           = '-'
};

enum class DLL_STATE MessageChangeKind : char
{
    ALIVE                  = 'A',
    NOT_ALIVE_DISPOSED     = 'D',
    NOT_ALIVE_UNREGISTERED = 'U'
};

enum class DLL_STATE MessageWriterFeedbackKind : char
{
    LivelinessLost            = 'L',
    OfferedDeadlineMissed     = 'D',
    OfferedIncompatiblePolicy = 'I',
    PublicationMatched        = 'M',          // ReaderId
    ValueAcked                = 'A',
    Timeout                   = 'T',
    Filtered                  = 'F'
};

enum class DLL_STATE MessageReaderFeedbackKind : char
{
    RequestedDeadlineMissed     = 'M',
    RequestedIncompatiblePolicy = 'I',
    SampleRejected              = 'R',
    LivelinessChanged           = 'L',
    DataAvailable               = 'D',
    SubscriptionMatched         = 'M',        // WriterId
    SampleLost                  = 'S',
    Filtered                    = 'F'
};

// ----------------------------------------------------------------
// Ids used in RxTransport
// ----------------------------------------------------------------

typedef Templates::NameDescription ChannelId;
typedef Templates::NameDescription ScopeId;
typedef Templates::NameDescription DomainId;
typedef BaseLib::GUUID             WriterId;
typedef BaseLib::GUUID             ReaderId;

namespace MessageAddress {
static const BaseLib::GUUID multicast = BaseLib::GUUID("multicast");
}

// Distributed: Domain<DomainId, Scope<ScopeId, Channel<ChannelId, MessageChannel<T> >>>>
// Local: Scope<ScopeId, Channel<ChannelId, MessageChannel<T>

typedef Templates::Key2<DomainId, ScopeId> DomainScopeHandle;

// DomainId is the third level Domain<DomainId, Scope<ScopeId, Channel<ChannelId, MessageChannel<T> >>>>
// ScopeId   == MasterCacheId (or ScopeId).
// ChannelId == DataCacheId

typedef Templates::Key3<DomainId, ScopeId, ChannelId> ChannelHandle;


// ----------------------------------------------------------------
// Base classes
// ----------------------------------------------------------------

class DLL_STATE MessageWriterBase
    : public Templates::InitializeMethods
      , public Templates::Action0<bool>
      , public Status::ActivationStatus
      , public Templates::Key1<WriterId>
{
public:
    MessageWriterBase(const WriterId& id)
        : Templates::Key1<WriterId>(id)
    { }

    virtual ~MessageWriterBase()
    { }
};

class DLL_STATE MessageReaderBase
    : public Templates::InitializeMethods
      , public Templates::Action0<bool>
      , public Status::ActivationStatus
      , public Templates::Key1<ReaderId>
{
public:
    MessageReaderBase(const ReaderId& id)
        : Templates::Key1<ReaderId>(id)
    { }

    virtual ~MessageReaderBase()
    { }
};

class DLL_STATE MessageControllerBase
    : public Templates::InitializeMethods
      , public Templates::Action0<bool>
      , public Status::ActivationStatus
      , public Templates::Key1<ChannelHandle>
{
public:
    MessageControllerBase(const ChannelHandle& id)
        : Templates::Key1<ChannelHandle>(id)
    { }

    virtual ~MessageControllerBase()
    { }
};

class DLL_STATE MessageChannelBase
    : public Templates::InitializeMethods
      , public Status::ActivationStatus
      , public Templates::Key1<ChannelHandle>
{
public:
    MessageChannelBase(const ChannelHandle& id)
        : Templates::Key1<ChannelHandle>(id)
    { }

    virtual ~MessageChannelBase()
    { }
};

// ----------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------

typedef std::shared_ptr<MessageChannelBase> MessageChannelBasePtr;
typedef std::shared_ptr<MessageWriterProxy> MessageWriterProxyPtr;
typedef std::shared_ptr<MessageReaderProxy> MessageReaderProxyPtr;

typedef InstanceHandle                   SequenceNumber;
typedef Collection::ISet<InstanceHandle> SequenceNumberSet;

// TODO: Someone must take care of the back-pressure - the individual proxies monitor statuses?

typedef Status::EventStatusTracker<TransmissionStatusKind, int> DataMessageStatus;
typedef Status::EventStatusTracker<MessageKind, SequenceNumber> MessageStatus;
typedef Status::EventStatusTracker<MessageKind, int>            MessageReaderStatus;
typedef Status::EventStatusTracker<MessageKind, int>            MessageWriterStatus;
typedef Status::EventStatusTracker<SequenceNumber, int>         SequenceNumberCounter;

typedef Status::EventStatusTracker<ChangeFromWriterStatusKind, SequenceNumber> MessageWriterProxyStatus;
typedef std::shared_ptr<MessageWriterProxyStatus>                              MessageWriterProxyStatusPtr;

typedef Status::EventStatusTracker<ChangeForReaderStatusKind, SequenceNumber> MessageReaderProxyStatus;
typedef std::shared_ptr<MessageReaderProxyStatus>                             MessageReaderProxyStatusPtr;

typedef Status::StatusTrackerTimestamped<int> RequestedStatus;

typedef Collection::details::StdMapCollectionType<SequenceNumber, MessageReaderProxyStatusPtr> MapChangeForReader;
typedef Collection::details::StdMapCollectionType<SequenceNumber, MessageWriterProxyStatusPtr> MapChangeForWriter;

typedef std::promise<ChangeForReaderStatusKind> ChangeForReaderPromise;
typedef std::shared_ptr<ChangeForReaderPromise> ChangeForReaderPromisePtr;

// ----------------------------------------------------------------
// Lookup resource in charge of writing and reading to channel id
// TODO: Replace these with observable cache
// ----------------------------------------------------------------

typedef Templates::ObjectsManager<ChannelId, MessageChannelBasePtr, DomainScopeHandle>       ChannelScopeManager;
typedef std::shared_ptr<ChannelScopeManager>                                                 ChannelScopeManagerPtr;
typedef Collection::details::StdMapCollectionType<DomainScopeHandle, ChannelScopeManagerPtr> LookupScopeManager;

typedef ChannelFeedbackObserver<MessageWriterFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>> ChannelFeedbackForPublisher;
typedef MessageFeedbackObserver<MessageWriterFeedbackKind, SequenceNumber, MessageReaderProxyStatus>                              MessageFeedbackForPublisher;

typedef ChannelFeedbackObserver<MessageReaderFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>> ChannelFeedbackForSubscriber;
typedef MessageFeedbackObserver<MessageReaderFeedbackKind, SequenceNumber, MessageWriterProxyStatus>                              MessageFeedbackForSubscriber;

typedef ChannelFeedbackSubject<MessageWriterFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>> ChannelFeedbackSubjectForWriter;
typedef MessageFeedbackSubject<MessageWriterFeedbackKind, SequenceNumber, MessageReaderProxyStatus>                              MessageFeedbackSubjectForWriter;

typedef ChannelFeedbackSubject<MessageReaderFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>> ChannelFeedbackSubjectForReader;
typedef MessageFeedbackSubject<MessageReaderFeedbackKind, SequenceNumber, MessageWriterProxyStatus>                              MessageFeedbackSubjectForReader;

typedef std::function<void(MessageReaderFeedbackKind, SequenceNumber, MessageWriterProxyStatus)>                                 MessageFeedbackFunctionForSubscriber;
typedef std::function<void(MessageReaderFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>)>    ChannelFeedbackFunctionForSubscriber;

typedef std::function<void(MessageWriterFeedbackKind, SequenceNumber, MessageReaderProxyStatus)>                                 MessageFeedbackFunctionForPublisher;
typedef std::function<void(MessageWriterFeedbackKind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>>)>    ChannelFeedbackFunctionForPublisher;

}}
