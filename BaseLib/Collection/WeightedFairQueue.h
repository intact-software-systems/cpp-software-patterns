#pragma once

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/WriteLocker.h"
#include "BaseLib/ReadLocker.h"
#include "BaseLib/WaitCondition.h"
#include "BaseLib/ElapsedTimer.h"
#include "BaseLib/Utility.h"

namespace BaseLib { namespace Collection
{

//#define SERVICE_RATE 512000

typedef int    FlowId;
typedef int    WeightId;

class FlowInfo
{
public:
    FlowInfo(FlowId id, WeightId weight)
        : id_(id)
        , weight_(weight)
    {}
    virtual ~FlowInfo() {}

public:
    const FlowId&  Id()        const { return id_; }
    const WeightId&  Weight()  const { return weight_; }

private:
    FlowId     id_;
    WeightId   weight_;
};


template <typename T>
class ObjectHolder
{
public:
    ObjectHolder(const T &obj)
        : obj_(obj)
        , enqueueTimeMs_(Timestamp::Now())
    {}

    T& Get()
    {
        return obj_;
    }

    //void Set(const T &obj)
    //{
    //    obj_ = obj;
    //    enqueueTimeMs_ = Utility::GetCurrentTimeMs();
    //}

    Timestamp GetEnqueueTime() const
    {
        return enqueueTimeMs_;
    }

private:
    T obj_;

private:
    Timestamp enqueueTimeMs_;
};

template <typename T, typename V = FlowInfo>
class Queue
{
public:
    CLASS_TRAITS(Queue)

    virtual bool Enqueue(const T&, const V&) { return false; }

    virtual T Dequeue() { return T(); }

    virtual bool IsEmpty() const { return true; }

    virtual bool IsEmpty(const FlowId &) const { return true; }

    virtual int64 Length() const { return 0; }

    virtual int64 Length(const FlowId &) const { return 0; }

    virtual bool IsFull(const FlowId &) const  { return true; }

    virtual int64 Capacity() const { return 0; }

    virtual void SetCapacity(int64) { }

    virtual void Clear() {}

    virtual void Clear(const FlowId &) {}
};

/**
 * @brief The WFQueue class
 *
 * Capacity is for each flow to create fairness!
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 */
template <typename T, typename V = FlowInfo>
class WFQueue : public Queue<T, V>
{
public:
    WFQueue(int64 capacity = LONG_MAX)
        : numObjects_(0)
        , capacity_(capacity)
    {}
    virtual ~WFQueue()
    {
        Clear();
    }

private:
    class DCDenominator
    {
    public:
        DCDenominator()
            : DC_(0)
            , DCUsed_(0)
            , DCOrig_(0)
        {}
        DCDenominator(double DC, double DCUsed, double DCOrig)
            : DC_(DC)
            , DCUsed_(DCUsed)
            , DCOrig_(DCOrig)
        {}
        ~DCDenominator() {}

    public:
        double& DC()        { return DC_; }
        double& DCUsed()    { return DCUsed_; }
        double& DCOrig()    { return DCOrig_; }

        const double& DC()      const { return DC_; }
        const double& DCUsed()  const { return DCUsed_; }
        const double& DCOrig()  const { return DCOrig_; }

        friend std::ostream& operator<<(std::ostream& ostr, const DCDenominator &dc)
        {
            ostr << "(" << dc.DC_ << "," << dc.DCUsed_ << "," << dc.DCOrig_ << ")";
            return ostr;
        }

    private:
        double DC_;
        double DCUsed_;
        double DCOrig_;
    };

    class Flow
    {
    public:
        Flow(int id, int weight, int64 capacity = LONG_MAX)
            : id_(id)
            , weight_(weight)
            , isActive_(false)
            , capacity_(capacity)
        {}
        ~Flow() {}

        typedef std::deque<T> ObjectQueue;

    public:

        bool IsActive() const { return isActive_; }
        void Activate()       { isActive_ = true; }
        void Deactivate()     { isActive_ = false; }

        const int&  Id()        const { return id_; }
        const int&  Weight()    const { return weight_; }

        DCDenominator&          Denominator()       { return denominator_; }
        const DCDenominator&    Denominator() const { return denominator_; }

        ObjectQueue&        Queue()         { return queue_; }
        const ObjectQueue   Queue() const   { return queue_; }

        int64       QueueLength() const { return queue_.size(); }
        int64       Capacity() const { return capacity_; }

        bool                HasCapacity() const { return capacity_ > queue_.size(); }
        bool                IsEmpty() const  { return queue_.empty(); }

        friend std::ostream& operator<<(std::ostream &ostr, const Flow &flow)
        {
            ostr << "(" << flow.id_ << "," << flow.weight_ << "," << flow.denominator_ << "," << flow.queue_.size() << "," << flow.capacity_ << ")";
            return ostr;
        }

    private:
        int     id_;
        int     weight_;
        bool    isActive_;

    private:
        DCDenominator   denominator_;
        ObjectQueue     queue_;
        int64   capacity_;
    };

    typedef std::list<Flow*> FlowQueue;

public:
    virtual bool Enqueue(const T& obj, const V &info)
    {
        return enqueue(obj, info);
    }

    virtual T Dequeue()
    {
        return dequeue();
    }

    virtual bool IsEmpty() const
    {
        return numObjects_ == 0;
    }

    virtual bool IsEmpty(const FlowId &flowId) const
    {
        Flow *flow = this->getFlow(flowId);
        return flow == NULL || flow->IsEmpty();

    }

    virtual int64 Length() const
    {
        return numObjects_;
    }

    virtual int64 Length(const FlowId &flowId) const
    {
        Flow *flow = this->getFlow(flowId);
        if(flow == NULL) return 0;

        return flow->QueueLength();
    }

    virtual bool IsFull(const FlowId &flowId) const
    {
        return !hasCapacity(flowId);
    }

    virtual int64 Capacity() const
    {
        return capacity_;
    }

    virtual void SetCapacity(int64 capacity)
    {
        capacity_ = capacity;
    }

    virtual void Clear()
    {
        for(typename FlowQueue::iterator it = weightedQueues_.begin(); it != weightedQueues_.end(); ++it)
        {
            Flow *flow = *it;

            // -- debug --
            ASSERT(flow);
            // -- debug --

            delete flow;
        }

        weightedQueues_.clear();
    }

    virtual void Clear(const FlowId &flowId)
    {
        Flow *flow = this->getFlow(flowId);
        if(flow == NULL) return;

        deleteFlow(flow);
    }

private:
    bool enqueue(const T& obj, const V &info)
    {
        Flow* flow = getOrCreateFlow(info.Id(), info.Weight());

        // -- debug --
        ASSERT(flow);
        // -- debug --

        if(!flow->HasCapacity()) return false;

        flow->Queue().push_back(obj);

        numObjects_++;

        return true;
    }

    T dequeue()
    {
        if(numObjects_ == 0) return T();

        unsigned int i = 0;

        for(i = 0; i < weightedQueues_.size() * 2; i++)
        {
            // ------------------------------------------
            // Get or set active queue, if any
            // ------------------------------------------
            Flow *activeFlow = getOrSetActiveQueue();
            if(activeFlow == NULL) break;

            // ------------------------------------------
            // Check out front object
            // ------------------------------------------
            ASSERT(!activeFlow->IsEmpty());

            T obj = activeFlow->Queue().front();

            // TODO: What if obj is a pointer?
            unsigned int objSz = sizeof(obj);

            // -- debug --
            //IDEBUG() << " Size of object " << objSz;
            //std::cout << "Flow " << *activeFlow << std::endl;
            // -- debug --

            // ------------------------------------------
            // Pop from queue if flow has quota
            // ------------------------------------------
            if(objSz <= activeFlow->Denominator().DC())
            {
                activeFlow->Queue().pop_front();

                updateDC(activeFlow->Denominator(), objSz);

                numObjects_--;

                return obj;
            }
            else // out of quota
            {
                activeFlow->Deactivate();

                moveFrontToBack(activeFlow);
            }
        }

        //ASSERT(i < weightedQueues_.size() * 2);

        return T();
    }

    /*bool isEmpty() const
    {
        if(weightedQueues_.empty()) return true;

        for(typename FlowQueue::const_iterator it = weightedQueues_.begin(), it_end = weightedQueues_.end(); it != it_end; ++it)
        {
            Flow *flow = *it;
            if(flow->Queue().empty() == false)
                return false;
        }

        return true;
    }*/

    Flow* getOrSetActiveQueue()
    {
        Flow *activeFlow= NULL;

        while(!weightedQueues_.empty())
        {
            activeFlow = getFrontActiveQueues();

            ASSERT(activeFlow);

            if(activeFlow->Queue().empty())
            {
                deleteFrontActiveQueues(activeFlow);
            }
            else if(!activeFlow->IsActive())
            {
                int systemWeight = calculateSystemWeight();	// calculate the weight of the system
                calculateDC(activeFlow->Denominator(), activeFlow->Weight(), systemWeight);				// calculates the DC for the flow

                activeFlow->Activate();

                //IDEBUG() << "Calculating DC " << *activeFlow;
            }

            if(activeFlow != NULL)
            {
                ASSERT(activeFlow->IsActive());
                ASSERT(!activeFlow->Queue().empty());

                break;
            }
        }

        return activeFlow;
    }


    /**
    - WRR mechanism (pseudo-code):

    // calculate number of packets to be served each round by connections

    for each flow f
       f.normalized_weight = f.weight / f.mean_packet_size

    min = findSmallestNormalizedWeight

    for each flow f
       f.packets_to_be_served = f.normalized_weight / min

    // main loop
    loop
       for each non-empty flow queue f
          min(f.packets_to_be_served, f.packets_waiting).times do
             servePacket f.getPacket
    */

    void calculateDC(DCDenominator &d, const WeightId &weight, const int &systemWeightCount) const
    {
        double dQuota = (double) weight / systemWeightCount;

        //double normalizedWeight = (double) weight / (double) sizeof(T);

        double serviceQuota = ( (double) sizeof(T) * dQuota) * (1.0f + dQuota);

        d.DC() = d.DC() + serviceQuota;

        // ----------------------------------------------------------
        // The flow gets dQuota % of the SERVICE_RATE to send packets
        // ----------------------------------------------------------
        //d.DC() = d.DC() + (SERVICE_RATE * dQuota);

        d.DCOrig() = d.DC();
    }

    void updateDC(DCDenominator &d, const unsigned int &sz) const
    {
        // ---------------------------------------------
        // Reduce the quota on every update
        // ---------------------------------------------
        d.DC() = d.DC() - sz;
        d.DCUsed() = d.DCUsed() + sz;

        // -- debug --
        ASSERT(d.DC() >= 0);
        // -- debug --
    }

    Flow* getOrCreateFlow(const FlowId &flowId, const WeightId &flowWeight)
    {
        Flow* flow  = getFlow(flowId);

        if(flow == NULL)
        {
            flow = createFlow(flowId, flowWeight);

            ASSERT(flow != NULL);
        }

        return flow;
    }

    Flow* getFlow(const FlowId &id) const
    {
        for(typename FlowQueue::const_iterator it = weightedQueues_.begin(); it != weightedQueues_.end(); ++it)
        {
            if((*it)->Id() == id)
                return (*it);
        }

        return NULL;
    }

    Flow* createFlow(const FlowId &id, const WeightId &weight)
    {
        Flow* flow = new Flow(id, weight);
        weightedQueues_.push_back(flow);

        //IDEBUG() << "Created flow " << *flow;

        return flow;
    }


    void deleteFlow(Flow* &activeFlow)
    {
        weightedQueues_.remove(activeFlow);    // remove from active list

        delete activeFlow;
        activeFlow = NULL;
    }

    Flow* popActiveQueue()
    {
        Flow* flow = weightedQueues_.front();
        weightedQueues_.pop_front();
        return flow;
    }

    Flow* getFrontActiveQueues() const
    {
        return weightedQueues_.front();
    }

    void moveFrontToBack(Flow *activeFlow)
    {
        //IDEBUG() << "Move flow to back " << *activeFlow;

        ASSERT(weightedQueues_.front() == activeFlow);

        weightedQueues_.pop_front();            // remove from active queue
        weightedQueues_.push_back(activeFlow);	// and add it to the end
    }

    void deleteFrontActiveQueues(Flow* &activeFlow)
    {
        // -- debug --
        ASSERT(weightedQueues_.front() == activeFlow);
        // -- debug --

        weightedQueues_.pop_front();    // remove from active list

        //activeFlow->Deactivate();
        delete activeFlow;
        activeFlow = NULL;
    }

    int calculateSystemWeight() const
    {
        int systemWeightCount = 0;					// reset the system weight

        for(typename FlowQueue::const_iterator it = weightedQueues_.begin(), it_end = weightedQueues_.end(); it != it_end; ++it)
        {
            systemWeightCount = systemWeightCount + (*it)->Weight();
        }

        return systemWeightCount;
    }

    bool hasCapacity(const FlowId &flowId) const
    {
        Flow* flow = getFlow(flowId);
        return flow == NULL || flow->HasCapacity();

    }

private:
    FlowQueue       weightedQueues_;
    int64   numObjects_;
    int64   capacity_;

private:
    WFQueue(const WFQueue&) {}
    WFQueue& operator=(const WFQueue &) { return *this; }
};

/**
 * @brief The RWFQueue class
 *
 * Capacity is for each flow to create fairness!
 *
 * TODO: typename LOCK = NullReadWriteLock - makes this synchronized or not
 */
template <typename T, typename V = FlowInfo>
class RWFQueue : public Queue<T, V>
{
public:
    RWFQueue(int64 capacity = LONG_MAX)
        : numObjects_(0)
        , capacity_(capacity)
    {}
    virtual ~RWFQueue()
    {
        Clear();
    }

private:
    class Flow
    {
    public:
        Flow(int id, int weight, int64 capacity = LONG_MAX)
            : id_(id)
            , weight_(weight)
            , isActive_(false)
            , probability_(0)
            , capacity_(capacity)
        {  }

        ~Flow() {}

        typedef std::deque<T> ObjectQueue;

    public:

        bool IsActive() const { return isActive_; }
        void Activate()       { isActive_ = true; }
        void Deactivate()     { isActive_ = false; }

        const int&  Id()        const { return id_; }
        const int&  Weight()    const { return weight_; }

        double&         Probability()       { return probability_; }
        const double&   Probability() const { return probability_; }

        ObjectQueue&        Queue()         { return queue_; }
        const ObjectQueue   Queue() const   { return queue_; }

        int64       QueueLength() const { return queue_.size(); }
        int64       Capacity() const { return capacity_; }

        bool                HasCapacity() const { return capacity_ > queue_.size(); }
        bool                IsEmpty() const     { return queue_.empty(); }

        friend std::ostream& operator<<(std::ostream &ostr, const Flow &flow)
        {
            ostr << "(" << flow.id_ << "," << flow.weight_ << "," << flow.probability_ << "," << flow.queue_.size() << "," << flow.capacity_ << ")";
            return ostr;
        }

    private:
        int     id_;
        int     weight_;
        bool    isActive_;

        double  probability_;

    private:
        ObjectQueue   queue_;
        int64 capacity_;
    };

    typedef std::list<Flow*> FlowQueue;

public:
    virtual bool Enqueue(const T& obj, const V &info)
    {
        return enqueue(obj, info);
    }

    virtual T Dequeue()
    {
        return dequeue();
    }

    virtual bool IsEmpty() const
    {
        return numObjects_ == 0;
    }

    virtual bool IsEmpty(const FlowId &flowId) const
    {
        Flow *flow = this->getFlow(flowId);
        return flow == NULL || flow->IsEmpty();
    }

    virtual int64 Length() const
    {
        return numObjects_;
    }

    virtual int64 Length(const FlowId &flowId) const
    {
        Flow *flow = this->getFlow(flowId);
        if(flow == NULL) return 0;

        return flow->QueueLength();
    }

    virtual bool IsFull(const FlowId &flowId) const
    {
        return !hasCapacity(flowId);
    }

    virtual int64 Capacity() const
    {
        return capacity_;
    }

    virtual void SetCapacity(int64 capacity)
    {
        capacity_ = capacity;
    }

    virtual void Clear()
    {
        for(typename FlowQueue::iterator it = weightedQueues_.begin(); it != weightedQueues_.end(); ++it)
        {
            Flow *flow = *it;

            // -- debug --
            ASSERT(flow);
            // -- debug --

            delete flow;
        }

        weightedQueues_.clear();
    }

    virtual void Clear(const FlowId &flowId)
    {
        Flow *flow = this->getFlow(flowId);
        if(flow == NULL) return;

        deleteFlow(flow);
    }

private:
    bool enqueue(const T& obj, const V &info)
    {
        Flow* flow = getOrCreateFlow(info.Id(), info.Weight());

        // -- debug --
        ASSERT(flow);
        // -- debug --

        if(!flow->HasCapacity()) return false;

        flow->Queue().push_back(obj);

        numObjects_++;

        return true;
    }

    T dequeue()
    {
        // ---------------------------------------------
        // Calculate the probability of choosing the individual flows
        // ---------------------------------------------
        bool isCalculated = calculateProbabilities();
        if(!isCalculated)
        {
            return T();
        }

        // ---------------------------------------------
        // Pick flow to dequeue from based on random values
        // and probability calculated in previous step
        // ---------------------------------------------
        double randomValue = generateRandom(); // randomValue is [0, 1)
        ASSERT(randomValue >= 0 && randomValue < 1);

        double totalProb = 0;

        Flow *activeFlow = NULL;

        for(typename FlowQueue::iterator it = weightedQueues_.begin(), it_end = weightedQueues_.end(); it != it_end; ++it)
        {
            activeFlow = (*it);

            // -- debug --
            ASSERT(activeFlow);
            ASSERT(!activeFlow->Queue().empty());
            // -- debug --

            totalProb = totalProb + activeFlow->Probability();

            if(randomValue < totalProb)
                break;
        }

        // -- debug --
        ASSERT(activeFlow != NULL);
        //IDEBUG() << randomValue << " < " << totalProb << " Dequeuing flow " << *activeFlow;
        // -- debug --

        return dequeue(activeFlow);
    }

    T dequeue(Flow *activeFlow)
    {
        ASSERT(!activeFlow->Queue().empty());

        T obj = activeFlow->Queue().front();
        activeFlow->Queue().pop_front();

        if(activeFlow->Queue().empty())
            deleteFlow(activeFlow);

        numObjects_--;

        return obj;
    }

    /*bool isEmpty() const
    {
        if(weightedQueues_.empty()) return true;

        for(typename FlowQueue::const_iterator it = weightedQueues_.begin(), it_end = weightedQueues_.end(); it != it_end; ++it)
        {
            Flow *flow = *it;
            if(flow->Queue().empty() == false)
                return false;
        }

        return true;
    }*/

    bool calculateProbabilities()
    {
        if(weightedQueues_.empty() || this->numObjects_ == 0)
            return false;

        double denominator = 0;

        for(typename FlowQueue::iterator it = weightedQueues_.begin(); it != weightedQueues_.end(); ++it)
        {
            Flow *flow = *it;

            // -- debug --
            ASSERT(flow);
            ASSERT(!flow->Queue().empty());
            // -- debug --

            // ---------------------------------------------
            // Denominator = Denominator +  W[i]/ H(Queue[i])
            // ---------------------------------------------
            denominator = denominator + ( (double) flow->Weight() / (double) sizeof(flow->Queue().front()) );
        }

        Flow *firstFlow = weightedQueues_.front();

        ASSERT(firstFlow);

        // --------------------------------------------------------------------
        //P[1] = (pFirstPck->weight/pFirstPck->packetSize) * (1/Denominator);
        // --------------------------------------------------------------------
        firstFlow->Probability() = (double) ( (double) firstFlow->Weight() / (double) sizeof(firstFlow->Queue().front() ) ) * (1.0f / denominator);

        double firstFlowDenom = (firstFlow->Probability() * (double) sizeof(firstFlow->Queue().front())) /(double) firstFlow->Weight();

        // -- debug --
        //IDEBUG() << (double) firstFlow->Weight() << " / " << (double) sizeof(firstFlow->Queue().front()) << " * " << 1.0f << "/" << denominator;
        //IDEBUG() << "Flow " << *firstFlow << " firstflow-denom " << firstFlowDenom;
        // -- debug --

        // ---------------------------------------------
        // calculates Probabilities for P[2..NUMBER_OF_FLOWS]
        // ---------------------------------------------
        if(weightedQueues_.size() > 1)
        {
            for(typename FlowQueue::iterator it = ++(weightedQueues_.begin()); it != weightedQueues_.end(); ++it)
            {
                Flow *flow = *it;

                // -- debug --
                ASSERT(flow);
                ASSERT(flow != firstFlow);
                ASSERT(!flow->Queue().empty());
                // -- debug --

                // P[j] = ((P[1]* pFirstPck->packetSize)/pFirstPck->weight) * (pPck->weight/pPck->packetSize);
                flow->Probability() = firstFlowDenom * ( (double) flow->Weight() / (double) sizeof(flow->Queue().front()) );

                //IDEBUG() << "Flow " << *flow;
            }
        }

        return true;
    }

    double generateRandom()
    {
        double x= (double(rand())/RAND_MAX);
        return x;
    }

    Flow* getOrCreateFlow(const FlowId &flowId, const WeightId &flowWeight)
    {
        Flow* flow  = getFlow(flowId);

        if(flow == NULL)
        {
            flow = createFlow(flowId, flowWeight);

            ASSERT(flow != NULL);
        }

        return flow;
    }

    Flow* getFlow(const FlowId &id) const
    {
        for(typename FlowQueue::const_iterator it = weightedQueues_.begin(); it != weightedQueues_.end(); ++it)
        {
            if((*it)->Id() == id)
                return (*it);
        }

        return NULL;
    }

    Flow* createFlow(const FlowId &id, const WeightId &weight)
    {
        Flow* flow = new Flow(id, weight);
        weightedQueues_.push_back(flow);
        return flow;
    }

    void deleteFlow(Flow* &activeFlow)
    {
        weightedQueues_.remove(activeFlow);    // remove from active list

        delete activeFlow;
        activeFlow = NULL;
    }

    bool hasCapacity(const FlowId &flowId) const
    {
        Flow* flow = getFlow(flowId);
        return flow == NULL || flow->HasCapacity();

    }

private:
    FlowQueue       weightedQueues_;
    int64   numObjects_;
    int64   capacity_;

private:
    RWFQueue(const RWFQueue&) {}
    RWFQueue& operator=(const RWFQueue &) { return *this; }
};

/**
 * @brief The ConcurrentWFQueue class
 *
 * Capacity is for each flow to create fairness!
 *
 * TODO:
 *  - See ring-buffer implementation and add Peek/Dequeue/Enqueue vectors of T
 *  - Add functions Dequeue/Peek that return pair<FlowInfo, T> or pair<FlowInfo, Container<T> >
 *
 */
template <typename T, typename V = FlowInfo, typename Container = WFQueue<T,V> >
class ConcurrentWFQueue : public Queue<T,V>
{
public:
    ConcurrentWFQueue(int64 capacity = LONG_MAX)
        : capacity_(capacity)
    {}

    virtual ~ConcurrentWFQueue() {}

    virtual bool Enqueue(const T& obj, const V& info)
    {
        WriteLocker lock(&mutex_);

        return hasCapacity(info.Id()) && this->enqueue(obj, info);

    }

    virtual bool Enqueue(const T& obj, const V& info, int64 msecs)
    {
        WriteLocker lock(&mutex_);

        if(!hasCapacity(info.Id()))
        {
            bool hasCapacity = this->waitForCapacity(msecs, info.Id());
            if(!hasCapacity)
                return false;
        }

        return this->enqueue(obj, info);
    }

    virtual T Dequeue()
    {
        WriteLocker lock(&mutex_);

        if(!container_.IsEmpty())
            waitForCapacity_.wakeOne();

        return container_.Dequeue();
    }

    T Dequeue(int64 msecs)
    {
        WriteLocker lock(&mutex_);

        if(container_.IsEmpty())
        {
           bool hasData = this->waitForData(msecs);
           if(!hasData)
               return T();
        }

        waitForCapacity_.wakeOne();

        return container_.Dequeue();
    }

    virtual bool IsEmpty() const
    {
        ReadLocker lock(&mutex_);
        return container_.IsEmpty();
    }

    virtual bool IsEmpty(const FlowId &flowId) const
    {
        ReadLocker lock(&mutex_);
        return container_.IsEmpty(flowId);
    }

    virtual int64 Length() const
    {
        ReadLocker lock(&mutex_);
        return container_.Length();
    }

    virtual int64 Length(const FlowId &flowId) const
    {
        ReadLocker lock(&mutex_);
        return container_.Length(flowId);
    }

    virtual bool IsFull(const FlowId &flowId) const
    {
        ReadLocker lock(&mutex_);
        return !hasCapacity(flowId);
    }

    virtual int64 Capacity() const
    {
        ReadLocker lock(&mutex_);
        return capacity_;
    }

    virtual void SetCapacity(int64 capacity)
    {
        WriteLocker lock(&mutex_);
        capacity_ = capacity;
    }

    virtual void Clear()
    {
        WriteLocker lock(&mutex_);
        this->container_.Clear();
    }

    virtual void Clear(const FlowId &flowId)
    {
        WriteLocker lock(&mutex_);
        this->container_.Clear(flowId);
    }

public:
    void WakeAll() const
    {
        WriteLocker lock(&mutex_);
        this->waitForCapacity_.wakeAll();
        this->waitForData_.wakeAll();
    }

    void WakeOne() const
    {
        WriteLocker lock(&mutex_);
        this->waitForCapacity_.wakeOne();
        this->waitForData_.wakeOne();
    }

private:
    bool enqueue(const T& obj, const V& info)
    {
        // -- debug --
        if(!hasCapacity(info.Id()))
            IWARNING() << "Capacity violated: " << container_.Length() << " >= " << capacity_;
        ASSERT(hasCapacity(info.Id()));
        // -- debug --

        bool isQueued = container_.Enqueue(obj, info);

        if(isQueued)
            waitForData_.wakeOne();

        return isQueued;
    }

    bool waitForCapacity(int64 msecs, const FlowId &flowId)
    {
        ElapsedTimer timer(msecs);

        while(!hasCapacity(flowId) && !timer.HasExpired())
        {
            bool isWoken = waitForCapacity_.wait(&mutex_, timer.Remaining());
            if(isWoken) break;
        }

        return hasCapacity(flowId);
    }

    bool waitForData(int64 msecs)
    {
        ElapsedTimer timer(msecs);

        while(container_.IsEmpty() && !timer.HasExpired())
        {
            bool isWoken = waitForData_.wait(&mutex_, timer.Remaining());
            if(isWoken) break;
        }

        return container_.IsEmpty() == nullptr;
    }

    bool hasCapacity(const FlowId &flowId) const
    {
        return container_.IsFull(flowId);
    }

private:
    Container       container_;
    int64   capacity_;

private:
    mutable ReadWriteLock   mutex_;

    mutable WaitCondition   waitForData_;
    mutable WaitCondition   waitForCapacity_;
};


}}

