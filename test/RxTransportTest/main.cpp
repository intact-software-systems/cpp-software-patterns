#include <RxTransport/ChannelFactory.h>

using namespace BaseLib;
using namespace BaseLib::Collection;
using namespace BaseLib::Concurrent;
using namespace BaseLib::Templates;

void setPublisherCallback(std::shared_ptr<PublisherChannel<std::string>> publisher)
{
    publisher->OnMessageFeedbackDo(
        [](MessageWriterFeedbackKind kind, SequenceNumber number, MessageReaderProxyStatus status)
        {
            IINFO() << "Feedback " << (char) kind << " SeqNum: " << number << " Status: " << (char) status.CurrentState();
        }
    );

    publisher->OnChannelFeedbackDo(
        [](MessageWriterFeedbackKind kind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> status)
        {
            IINFO() << "Feedback " << (char) kind << " status for " << (char) status->State();
        }
    );
}

void setSubscriberCallback(std::shared_ptr<SubscriberChannel<std::string>> subscriber)
{
    subscriber->OnDataDo(
        [](std::string value)
        {
            IINFO() << "Got value" << value;
        }
    );

    subscriber->OnMessageFeedbackDo(
        [](MessageReaderFeedbackKind kind, SequenceNumber number, MessageWriterProxyStatus status)
        {
            IINFO() << "Feedback " << (char) kind << " SeqNum: " << number << " Status: " << (char) status.CurrentState();
        }
    );

    subscriber->OnChannelFeedbackDo(
        [](MessageReaderFeedbackKind kind, std::shared_ptr<Status::StateStatusTimestampedType<MessageKind, int>> status)
        {
            IINFO() << "Feedback " << (char) kind << " status for " << (char) status->State();
        }
    );
}


void testRWChannel()
{
    ScopedTimer a(IINFO());

    std::string yeah = "yeah";

    ChannelHandle handle = ChannelHandle{yeah, yeah, yeah};

    std::shared_ptr<RWChannel<std::string>> channel = ChannelFactory::CreateRWChannel<std::string>(handle);

    setPublisherCallback(channel->publisher());
    setSubscriberCallback(channel->subscriber());

    for(int i = 0; i < 10; ++i)
    {
        channel->publisher()->Next("ping" + std::to_string(i));
    }

    MessageFuture<std::string> message = channel->publisher()->Next("ping1000");

    std::pair<Status::FutureStatus, ChangeForReaderStatusKind> result = message.Result();

    IINFO() << "Message : " << (char) result.second;

    std::map<InstanceHandle, std::string> objects = channel->subscriber()->DataCache().getObjects();

    for(auto entry : objects)
    {
        IINFO() << entry.first << ": " << entry.second;
    }

}

void testPublisherSubscriber()
{
    ScopedTimer a(IINFO());

    std::string yeah = "yeah";

    ChannelHandle handle = ChannelHandle{yeah, yeah, yeah};

    auto subscriber = ChannelFactory::CreateSubscriber<std::string>(handle);

    setSubscriberCallback(subscriber);

    auto publisher = ChannelFactory::CreatePublisher<std::string>(handle);

    setPublisherCallback(publisher);

    publisher->Next("ping0");
    publisher->Next("ping1");
    publisher->Next("ping2");
    publisher->Next("ping3");
    publisher->Next("ping4");
    publisher->Next("ping5", InstanceHandle(1));
    MessageFuture<std::string> message = publisher->Next("ping6");

    std::pair<Status::FutureStatus, ChangeForReaderStatusKind> result = message.Result();

    IINFO() << "Message : " << (char) result.second;

    std::map<InstanceHandle, std::string> objects = subscriber->DataCache().getObjects();

    for(auto entry : objects)
    {
        IINFO() << entry.first << ": " << entry.second;
    }
}

int main(int, char* [])
{
    //testPublisherSubscriber();
    testRWChannel();
}
