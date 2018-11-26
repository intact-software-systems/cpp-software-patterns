#pragma once

#include <RxTransport/CommonDefines.h>
#include "MessageReaderPolicy.h"
#include "MessageReaderData.h"
#include "RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageReaderPolicyAlgorithms
{
    template <typename T>
    static void PolicyViolations(const MessageWriterProxies& proxies, const MessageStatus& messageStatus, MessageCache<T>& cache, MessageFeedbackSubjectForReader& feedback)
    {
        std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, SequenceNumber>> dataStatus = messageStatus.State(MessageKind::Data);

        for(auto item : dataStatus->Instances())
        {
            SequenceNumber sequenceNumber = item.second;

            std::shared_ptr<DataMessage<T>> message = cache.Find(sequenceNumber);
            ASSERT(message);

            proxies.ForEach(
                [&sequenceNumber, &message, &feedback](std::shared_ptr<MessageWriterProxy> writerProxy)
                {
                    MessageWriterProxyStatusPtr status = writerProxy->Find(sequenceNumber);
                    ASSERT(status);

                    if(status->CurrentState() == ChangeFromWriterStatusKind::RECEIVED)
                    {
                        feedback.OnMessageFeedback(MessageReaderFeedbackKind::DataAvailable, sequenceNumber, status.operator*());
                    }

                    // ------------------------------------------------------
                    // Check for deadline violations.
                    // ------------------------------------------------------
                    //Duration timeFromCreatedToAcked = status->TimeSinceFirstAccessFromTo(ChangeFromWriterStatusKind::);
                    //if(timeFromCreatedToAcked > policy.readerPolicy.Deadline().delegate()) { }
                }
            );
        }
    }

    template <typename T>
    static void CheckPolicy(const MessageReaderStatus& status, const MessageReaderPolicy& policy, ChannelFeedbackSubjectForReader& feedback)
    {
        std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> dataStatus = status.State(MessageKind::Data);

        // Deadline violation: a message not received in interval [a,b]
        if(dataStatus->TimeSinceMostRecentAccess() > policy.readerPolicy.Deadline().delegate())
        {
            feedback.OnChannelFeedback(MessageReaderFeedbackKind::RequestedDeadlineMissed, dataStatus);
        }
    }
};

}}
