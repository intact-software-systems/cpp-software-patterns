#pragma once

#include "RxTransport/Message.h"
#include "RxTransport/MessageBuilder.h"

#include "MessageWriterPolicy.h"
#include "MessageReaderProxies.h"
#include "MessageReaderProxy.h"

#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageWriterPolicyAlgorithms
{
    template <typename T>
    static void PolicyViolations(
        const MessageReaderProxies& proxies,
        const MessageWriterPolicy& policy,
        const MessageStatus& messageStatus,
        MessageCache<T>& cache,
        MessageFeedbackSubjectForWriter& feedback
    )
    {
        // Detect: timed out, deadline passed, latency budget violated, acked

        std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, SequenceNumber>> dataStatus = messageStatus.State(MessageKind::Data);

        for(auto item : dataStatus->Instances())
        {
            SequenceNumber sequenceNumber = item.second;

            std::shared_ptr<DataMessage<T>> message = cache.Find(sequenceNumber);
            ASSERT(message);

            proxies.ForEach(
                [&sequenceNumber, &message, &policy, &feedback](std::shared_ptr<MessageReaderProxy> reader)
                {
                    MessageReaderProxyStatusPtr status = reader->Find(sequenceNumber);

                    ASSERT(status);

                    if(status->CurrentState() == ChangeForReaderStatusKind::FILTERED)
                    {
                        feedback.OnMessageFeedback(MessageWriterFeedbackKind::Filtered, sequenceNumber, status.operator*());
                        return;
                    }

                    ChangeForReaderStatusKind initialStatus = policy.commDirection == Policy::CommDirection::Pull
                                                              ? ChangeForReaderStatusKind::UNACKNOWLEDGED
                                                              : ChangeForReaderStatusKind::UNSENT;

                    // ------------------------------------------------------
                    // Is acked?
                    // ------------------------------------------------------
                    bool acked = status->CurrentState() == ChangeForReaderStatusKind::ACKNOWLEDGED;
                    if(acked)
                    {
                        feedback.OnMessageFeedback(MessageWriterFeedbackKind::ValueAcked, sequenceNumber, status.operator*());
                        return;
                    }

                    // ------------------------------------------------------
                    // Check for deadline violations.
                    // ------------------------------------------------------
                    Duration timeFromCreatedToAcked = status->TimeSinceFirstAccessFromTo(initialStatus, ChangeForReaderStatusKind::ACKNOWLEDGED);

                    if(timeFromCreatedToAcked > policy.writerPolicy.Deadline().delegate())
                    {
                        feedback.OnMessageFeedback(MessageWriterFeedbackKind::OfferedDeadlineMissed, sequenceNumber, status.operator*());
                    }
                }
            );
        }
    }

    template <typename T>
    static void CheckPolicy(const MessageWriterStatus& status, const MessageWriterPolicy& policy, ChannelFeedbackSubjectForWriter& feedback)
    {
        std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> dataStatus = status.State(MessageKind::Data);

        if(dataStatus->TimeSinceMostRecentAccess() > policy.writerPolicy.Deadline().delegate())
        {
            feedback.OnChannelFeedback(MessageWriterFeedbackKind::OfferedDeadlineMissed, dataStatus);
        }
    }
};

}}
