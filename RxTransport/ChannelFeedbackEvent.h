#pragma once

#include "CommonDefines.h"

namespace BaseLib { namespace Concurrent {

template <typename T, typename U>
class ChannelFeedbackEvent
{
public:
    virtual ~ChannelFeedbackEvent()
    { }

    virtual void OnChannelFeedback(T t, U u) = 0;
};

template <typename T, typename U, typename V>
class MessageFeedbackEvent
{
public:
    virtual ~MessageFeedbackEvent()
    { }

    virtual void OnMessageFeedback(T t, U u, V v) = 0;
};


template <typename T, typename U, typename V>
class MessageFeedbackObserver
    : public MessageFeedbackEvent<T, U, V>
      , public Concurrent::ObserverSlot<MessageFeedbackObserver<T, U, V>>
{
};

template <typename T, typename U>
class ChannelFeedbackObserver
    : public ChannelFeedbackEvent<T, U>
      , public Concurrent::ObserverSlot<ChannelFeedbackObserver<T, U>>
{
};


template <typename T, typename U>
class ChannelFeedbackSubject
    : public ChannelFeedbackEvent<T, U>
      , public Concurrent::Observable<ChannelFeedbackObserver<T, U>>
{
    typedef ChannelFeedbackObserver<T, U>      TheObserver;
    typedef Concurrent::Signaller2<void, T, U> TheSignaller;
    typedef std::shared_ptr<TheSignaller>      TheSignallerPtr;

public:
    ChannelFeedbackSubject()
        : signaller_(new TheSignaller())
    { }

    virtual ~ChannelFeedbackSubject()
    { }

    virtual void OnChannelFeedback(T t, U u)
    {
        signaller_->Signal(t, u);
    }

    virtual std::shared_ptr<SlotHolder> Connect(TheObserver* observer)
    {
        auto slot = signaller_->template Connect<TheObserver>(observer, &ChannelFeedbackEvent<T, U>::OnChannelFeedback);
        observer->AddSlot(observer, slot);
        return slot;
    }

    virtual bool Disconnect(TheObserver* observer)
    {
        return signaller_->template Disconnect<TheObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    TheSignallerPtr signaller_;
};

template <typename T, typename U, typename V>
class MessageFeedbackSubject
    : public MessageFeedbackEvent<T, U, V>
      , public Concurrent::Observable<MessageFeedbackObserver<T, U, V>>
{
    typedef MessageFeedbackObserver<T, U, V>      TheObserver;
    typedef Concurrent::Signaller3<void, T, U, V> TheSignaller;
    typedef std::shared_ptr<TheSignaller>         TheSignallerPtr;

public:
    MessageFeedbackSubject()
        : signaller_(new TheSignaller())
    { }

    virtual ~MessageFeedbackSubject()
    { }

    virtual void OnMessageFeedback(T t, U u, V v)
    {
        signaller_->Signal(t, u, v);
    }

    virtual std::shared_ptr<SlotHolder> Connect(TheObserver* observer)
    {
        auto slot = signaller_->template Connect<TheObserver>(observer, &MessageFeedbackEvent<T, U, V>::OnMessageFeedback);
        observer->AddSlot(observer, slot);
        return slot;
    }

    virtual bool Disconnect(TheObserver* observer)
    {
        return signaller_->template Disconnect<TheObserver>(observer);
    }

    virtual void DisconnectAll()
    {
        signaller_->DisconnectAll();
    }

private:
    TheSignallerPtr signaller_;
};

}}
