#ifndef NetworkLib_Socket_DatagramReader_h_IsIncluded
#define NetworkLib_Socket_DatagramReader_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Serialize/IncludeLibs.h"
#include"NetworkLib/Message/IncludeLibs.h"

namespace NetworkLib
{

/**
 * @brief The DatagramReaderScheduler class is used when UDP is the transport protocol
 *
 * Note! Currently this is only to show concept. To be removed.
 */
class DatagramReaderScheduler : public BaseLib::Concurrent::TaskScheduler
{
public:
    typedef RingBufferQueueMap<char, FlowId> MapFlowBuffer;

    typedef IMap<FlowId, DatagramReader::Ptr> MapDatagramProducer;

public:
    DatagramReaderScheduler(std::string name, MapFlowBuffer::Ptr flowBuffer)
        : threadPool_(new ThreadPool(name))
        , flowBuffer_(flowBuffer)
        , datagramSize_(512)
    {}
    virtual ~DatagramReaderScheduler() {}

    CLASS_TRAITS(DatagramReaderScheduler)

public:
    virtual bool ScheduleTasks()
    {
        // TODO:
        // Use the RingBufferQueueMap and WaitForAnyData
        // Implement a kind of select mechanism in RingBufferQueueMap using WaitSet
        MapFlowBuffer::VectorQID queues = flowBuffer_->SelectQueuesWithSize(datagramSize_, 3000);

        if(!queues.empty())
        {
            for(MapFlowBuffer::VectorQID::iterator it = queues.begin(), it_end = queues.end(); it != it_end; ++it)
            {
                FlowId id = *it;

                if(!producers_.containsKey(id))
                {
                    producers_.put(id, DatagramReader::Ptr(new DatagramReader(flowBuffer_->GetQueue(id), this->datagramSize_)));
                }

                // -- debug --
                ASSERT(producers_.find(id) != producers_.end());
                // -- debug --

                DatagramReader::Ptr producer = producers_.at(id);

                // -- debug --
                ASSERT(producer);
                // -- debug --

                bool isTriggered = threadPool_->TriggerTask((Runnable*) producer.get());

                // -- debug --
                ASSERT(isTriggered);
                // -- debug --
            }
        }
    }

    bool StartReader()
    {
        threadPool_->SetTaskScheduler(this->GetPtr());
        return true;
    }

private:
    /**
     * @brief threadPool_
     */
    ThreadPool<Runnable>::Ptr threadPool_;

    /**
     * @brief consumers_
     */
    MapDatagramProducer producers_;


    /**
     * @brief flowBuffer_
     */
    MapFlowBuffer::Ptr flowBuffer_;


    /**
     * @brief datagramSize_
     *
     * TODO: Make a policy!
     *
     * - Refactor all NetworkLib policies and BaseLib policies into separate folder and header files
     */
    int64 datagramSize_;
};

/**
 * @brief The DatagramReader class
 */
class DatagramReader
        : public Runnable
        , public DatagramProducerObserver
{
private:
    /**
     * @brief CharVector
     */
    typedef std::vector<char>   CharVector;

public:
    DatagramReader(RingBufferQueue<char>::Ptr dataBuffer, int64 datagramsize)
        : dataBuffer_(dataBuffer)
        , datagramSize_(datagramsize)
    {}
    virtual ~DatagramReader() {}


    virtual void run()
    {
        CharVector dataChunk = dataBuffer_->Peek<CharVector>(datagramSize_);
        if(dataChunk.size() != datagramSize_) return;

        DatagramFactory factory;
        Datagram::Ptr datagram = factory.Create(dataChunk);

        dataBuffer_->Pop(datagramSize_);

        // TODO: signalller.Signal(datgram, DatagramProducerObserver::OnDatagramReceived);
    }

private:
    /**
     * @brief datagramSize_
     *
     * TODO: Make a policy!
     *
     * - Refactor all NetworkLib policies and BaseLib policies into separate folder and header files
     */
    int64 datagramSize_;


    RingBufferQueue<char>::Ptr dataBuffer_;
};


class DatagramFactory : public BaseLib::Templates::CreateMethod1<NetworkLib::Datagram::Ptr, NetworkLib::SerializeReader::Ptr>
{
public:
    DatagramFactory() {}
    virtual ~DatagramFactory() {}

    virtual NetworkLib::Datagram::Ptr Create(SerializeReader::Ptr reader)
    {
        Datagram::Ptr datagram(new Datagram(reader));
        return datagram;
    }
};


template <typename Source, typename Target, typename Strategy>
class DataReactor : public Runnable
{
public:
    DataReactor(Source source, Target target, Strategy strategy)
        : source_(source)
        , target_(target)
        , strategy_(strategy)
    {}

    virtual void run()
    {
        strategy_.Invoke(source_, target_);

        target_.Trigger(source_);
    }

private:
    Source source_;
    Target target_;
    Strategy strategy_;
};

}

#endif // DATAGRAMREADER_H
