#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/MessagePolicy.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE SubMessage
{
public:
    SubMessage(const MessageKind& kind)
        : kind_(kind)
    { }

    virtual ~SubMessage()
    { }

    const MessageKind& Kind() const
    {
        return kind_;
    }

    virtual std::string toString() const = 0;

private:
    const MessageKind kind_;
};

typedef std::shared_ptr<SubMessage> SubMessagePtr;

// ---------------------------------
// Writer --> Reader
// ---------------------------------

class DLL_STATE Gap : public SubMessage
{
public:
    Gap(const WriterId& writerId, const SequenceNumberSet& gapList)
        : SubMessage(MessageKind::Gap)
        , writerId_(writerId)
        , gapList_(gapList)
    { }

    virtual ~Gap()
    { }

    const WriterId& Id() const
    {
        return writerId_;
    }

    const SequenceNumberSet& irrelevant() const
    {
        return gapList_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "Gap(" << writerId_ << "," << gapList_ << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Gap& gap)
    {
        ostr << gap.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<Gap> gap)
    {
        ostr << gap->toString();
        return ostr;
    }

private:
    const WriterId          writerId_;
    const SequenceNumberSet gapList_;
};

// ---------------------------------
// Writer --> Reader
// ---------------------------------

class DLL_STATE Heartbeat : public SubMessage
{
public:
    Heartbeat(
        WriterId writerId,
        SequenceNumber first,
        SequenceNumber last,
        bool responseRequiredFromReader,
        bool readerRespondWithMissingChanges
    )
        : SubMessage(MessageKind::Heartbeat)
        , writerId_(writerId)
        , first_(first)
        , last_(last)
        , responseRequiredFromReader_(responseRequiredFromReader)
        , readerRespondWithMissingChanges_(readerRespondWithMissingChanges)
    {
        ASSERT(!writerId_.empty());
    }

    virtual ~Heartbeat()
    { }

    const WriterId& Id() const
    {
        return writerId_;
    }

    const SequenceNumber& First() const
    {
        return first_;
    }

    const SequenceNumber& Last() const
    {
        return last_;
    }

    bool IsResponseRequired() const
    {
        return responseRequiredFromReader_;
    }

    bool IsResponseRequiredWhenMissingChanges() const
    {
        return readerRespondWithMissingChanges_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "Heartbeat(" << writerId_ << "," << first_ << ", " << last_ << ", " << responseRequiredFromReader_ << ", " << readerRespondWithMissingChanges_ << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Heartbeat heartbeat)
    {
        ostr << heartbeat.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<Heartbeat> heartbeat)
    {
        ostr << heartbeat->toString();
        return ostr;
    }

private:
    const WriterId       writerId_;
    const SequenceNumber first_;
    const SequenceNumber last_;

    const bool responseRequiredFromReader_;
    const bool readerRespondWithMissingChanges_;
};

// ---------------------------------
// Writer --> Reader
// ---------------------------------

template <typename T>
struct DataHolder
{
    DataHolder(const T& t, const WriterId& writerId, const InstanceHandle& instanceHandle, const MessageChangeKind kind, const MessagePolicy& policy)
        : t_(t)
        , writerId_(writerId)
        , instanceHandle_(instanceHandle)
        , kind_(kind)
        , policy_(policy)
    {
        status_.NextState(TransmissionStatusKind::CREATED, 1);
    }

    virtual ~DataHolder()
    { }

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "DataHolder(" << t_ << "," << writerId_ << "," << instanceHandle_ << "," << (char) kind_ << ") First access: " << status_.TimeSinceFirstAccess();
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const DataHolder& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<DataHolder> data)
    {
        ostr << data->toString();
        return ostr;
    }

    const T                 t_;
    const WriterId          writerId_;
    const InstanceHandle    instanceHandle_;
    const MessageChangeKind kind_;

    const MessagePolicy policy_;
    DataMessageStatus   status_;
};

template <typename T>
class DataMessage : public SubMessage
{
public:
    DataMessage(const DataHolder<T>& dataHolder, SequenceNumber sequenceNumber)
        : SubMessage(MessageKind::Data)
        , dataHolder_(dataHolder)
        , sequenceNumber_(sequenceNumber)
        , deliveryPromise_(std::make_shared<ChangeForReaderPromise>())
    { }

    virtual ~DataMessage()
    { }

    const WriterId& Id() const
    {
        return dataHolder_.writerId_;
    }

    const T& Value() const
    {
        return dataHolder_.t_;
    }

    const MessageChangeKind& ChangeKind() const
    {
        return dataHolder_.kind_;
    }

    const InstanceHandle& Handle() const
    {
        return dataHolder_.instanceHandle_;
    }

    DataMessageStatus& Status()
    {
        return dataHolder_.status_;
    }

    const SequenceNumber& SeqNumber() const
    {
        return sequenceNumber_;
    }

    ChangeForReaderPromisePtr DeliveryPromise() const
    {
        return deliveryPromise_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "DataMessage(" << dataHolder_ << ", SeqNum=" << sequenceNumber_ << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const DataMessage& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<DataMessage> data)
    {
        ostr << data->toString();
        return ostr;
    }

private:
    DataHolder<T>        dataHolder_;
    const SequenceNumber sequenceNumber_;

    ChangeForReaderPromisePtr deliveryPromise_;
};

// ---------------------------------
// Reader --> Writer
// ---------------------------------
class DLL_STATE AckNack : public SubMessage
{
public:
    AckNack(ReaderId readerId, SequenceNumber ackUntil, SequenceNumberSet nacks)
        : SubMessage(MessageKind::AckNack)
        , readerId_(readerId)
        , ackUntil_(ackUntil)
        , nacks_(nacks)
    { }

    virtual ~AckNack()
    { }

    const ReaderId& Id() const
    {
        return readerId_;
    }

    const SequenceNumber& AckUntil() const
    {
        return ackUntil_;
    }

    const SequenceNumberSet& Nacks() const
    {
        return nacks_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "AckNack(" << readerId_ << "," << ackUntil_ << "," << nacks_ << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const AckNack& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<AckNack> data)
    {
        ostr << data->toString();
        return ostr;
    }

private:
    const ReaderId          readerId_;
    const SequenceNumber    ackUntil_;
    const SequenceNumberSet nacks_;
};

typedef std::shared_ptr<AckNack> AckNackPtr;

// ---------------------------------
// Reader --> Writer
// ---------------------------------

class DLL_STATE Request : public SubMessage
{
public:
    Request(ReaderId readerId, Count number)
        : SubMessage(MessageKind::Request)
        , readerId_(readerId)
        , number_(number)
    { }

    virtual ~Request()
    { }

    const ReaderId& Id() const
    {
        return readerId_;
    }

    const Count& number() const
    {
        return number_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    std::string toString() const
    {
        std::stringstream ostr;
        ostr << "Request(" << readerId_ << "," << number_ << ")";
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Request& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<Request> data)
    {
        ostr << data->toString();
        return ostr;
    }

private:
    const ReaderId readerId_;
    const Count    number_;
};

typedef std::shared_ptr<Request> RequestPtr;

class MessageData
{
public:
    MessageData()
        : handle_()
        , from_()
        , to_()
        , messages_()
        , accessorKind_(ChannelAccessorKind::NO)
    { }

    MessageData(ChannelHandle handle, const GUUID& from, const GUUID& to, const std::list<SubMessagePtr>& messages, const ChannelAccessorKind accessorKind)
        : handle_(handle)
        , from_(from)
        , to_(to)
        , messages_(messages)
        , accessorKind_(accessorKind)
    { }

    virtual ~MessageData()
    { }

    const ChannelHandle& handle() const
    {
        return handle_;
    }

    const GUUID& From() const
    {
        return from_;
    }

    const GUUID& To() const
    {
        return to_;
    }

    const ChannelAccessorKind& AccessorKind() const
    {
        return accessorKind_;
    }

    const std::list<SubMessagePtr>& Messages() const
    {
        return messages_;
    }

    // ---------------------------------------------
    // Printing message data
    // ---------------------------------------------

    virtual std::string toString() const
    {
        std::stringstream ostr;
        ostr << "MessageData(" << handle_ << ", (" << from_ << "," << to_ << "), ChannelAccessorKind="
        << (char) accessorKind_ << ", submessages=" << messages_.size() << "): " << std::endl;

        for(auto item : messages_)
        {
            ostr << "\t" << item->toString() << std::endl;
        }

        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& ostr, const MessageData& data)
    {
        ostr << data.toString();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<MessageData> data)
    {
        ostr << data->toString();
        return ostr;
    }

private:
    const ChannelHandle            handle_;
    const GUUID                    from_;
    const GUUID                    to_;
    const std::list<SubMessagePtr> messages_;
    const ChannelAccessorKind      accessorKind_;
};

template <typename T>
class Message
    : public Templates::ContextObjectShared<MessagePolicy, MessageData, MessageStatus>
{
public:
    Message()
        : Templates::ContextObjectShared<MessagePolicy, MessageData, MessageStatus>
        (
            new MessagePolicy(), new MessageData(), new MessageStatus()
        )
    { }

    Message(ChannelHandle handle, const GUUID& from, const GUUID& to, std::list<SubMessagePtr> messages, const MessagePolicy& policy, ChannelAccessorKind channelAccessor)
        : Templates::ContextObjectShared<MessagePolicy, MessageData, MessageStatus>
        (
            new MessagePolicy(policy),
            new MessageData(handle, from, to, messages, channelAccessor),
            new MessageStatus()
        )
    { }

    virtual ~Message()
    { }

    bool IsContent() const
    {
        return !this->data()->Messages().empty();
    }

    int NumberOf(MessageKind kind) const
    {
        int count = 0;

        for(const auto& message : this->data()->Messages())
        {
            if(message->Kind() == kind)
            {
                ++count;
            }
        }
        return count;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const Message<T>& message)
    {
        ostr << message.data()->toString();
        return ostr;
    }

    static Message Empty()
    {
        return Concurrent::Message<T>();
    }
};

}}
