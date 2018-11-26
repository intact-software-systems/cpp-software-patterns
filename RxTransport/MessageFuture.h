#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/MessagePolicy.h"

namespace BaseLib { namespace Concurrent {

/**
 * Future to wait for message delivery.
 *
 * Enables a client to wait for delivery before continuing
 */
template <typename T>
class MessageFuture
    : public Templates::Notifyable<MessageFuture<T>, WaitCondition, Mutex>
      , public Templates::ResultMethod1<std::pair<Status::FutureStatus, ChangeForReaderStatusKind>, int64>
      , public Templates::IsReceivedMethod
{
public:
    MessageFuture()
        : message_()
        , future_()
    { }

    MessageFuture(Message<T> message, std::shared_future<ChangeForReaderStatusKind> future)
        : message_(message)
        , future_(future)
    { }

    virtual ~MessageFuture()
    { }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const
    {
        std::chrono::duration<int64, std::milli> duration(msecs);

        return future_.wait_for(duration) == std::future_status::ready;
    }

    std::pair<Status::FutureStatus, ChangeForReaderStatusKind> Result(int64 msecs = LONG_MAX) const
    {
        std::chrono::duration<int64, std::milli> duration(msecs);

        std::future_status status = future_.wait_for(duration);

        ChangeForReaderStatusKind kind = future_.get();

        switch(status)
        {
            case std::future_status::ready:
            {
                return std::pair<Status::FutureStatus, ChangeForReaderStatusKind>(Status::FutureStatus::Ready(), kind);
            }
            case std::future_status::timeout:
            {
                return std::pair<Status::FutureStatus, ChangeForReaderStatusKind>(Status::FutureStatus::Timeout(), kind);
            }
            case std::future_status::deferred:
            {
                return std::pair<Status::FutureStatus, ChangeForReaderStatusKind>(Status::FutureStatus::Deferred(), kind);
            }
        }
        return std::pair<Status::FutureStatus, ChangeForReaderStatusKind>(Status::FutureStatus::Deferred(), kind);
    }

    virtual bool IsReceived() const
    {
        static std::chrono::duration<int64, std::milli> duration(1);

        return future_.wait_for(duration) == std::future_status::ready;
    }

    virtual bool IsContent() const
    {
        return message_.IsContent();
    }

    const Message<T>& Data() const
    {
        return message_;
    }

    static MessageFuture<T> Empty()
    {
        return Concurrent::MessageFuture<T>();
    }

private:
    Message<T>                                            message_;
    mutable std::shared_future<ChangeForReaderStatusKind> future_;
};

}}
