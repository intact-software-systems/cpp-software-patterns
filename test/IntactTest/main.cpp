#include "BaseLib/IncludeLibs.h"

using namespace std;

using namespace BaseLib;
using namespace BaseLib::Concurrent;
using namespace BaseLib::Templates;
using namespace BaseLib::Collection;

namespace ProducerConsumerTest
{

/**
 * @brief The ObjectProduced class
 */
class ObjectProduced
{
public:
    ObjectProduced()
        : id_(0)
        , weight_(0)
    {}

    ObjectProduced(FlowId id, WeightId weight, int cnt)
        : id_(id)
        , weight_(weight)
        , cnt_(cnt)
    {}
    ~ObjectProduced() {}

public:

    FlowId GetId() const { return id_; }

    WeightId GetWeight() const { return weight_; }

    unsigned int GetSize() const { return sizeof(this); }

    bool IsValid() const  { return id_ != 0; }

    int GetCnt() const { return cnt_; }

    friend std::ostream& operator<<(std::ostream &ostr, const ObjectProduced &obj)
    {
        ostr << "(" << obj.GetId() << "," << obj.GetWeight() << "," << obj.GetSize() << "," << obj.GetCnt() << ")";
        return ostr;
    }

private:
    FlowId id_;
    WeightId weight_;
    int cnt_;
};

class Producer : public Runnable
{
public:
    Producer(Queue<ObjectProduced> &queue, int sleepTime, FlowId id, WeightId weight)
        : queue_(queue)
        , id_(id)
        , weight_(weight)
        , lastExecutionTime_(Utility::GetCurrentTimeMs())
        , sleepTime_(sleepTime)
    {}
    ~Producer() {}

    virtual void run()
    {
        static ProtectedVar<int> cnt(0);

        //if(queue_.HasCapacity())
        //{
            int prodCnt = ++cnt;

            ObjectProduced prod(id_, weight_, prodCnt);

            bool isPushed = queue_.Enqueue(prod, FlowInfo(id_, weight_));

            //if(isPushed == false)
            //{
                //IDEBUG() << "Couldn't push back!";
                //ASSERT(isPushed == true);
            //}
            //else
            //    IDEBUG() << id_ << " produced " << prod;
        //}

        //if(std::abs( (Utility::GetCurrentTimeMs() - lastExecutionTime_) - sleepTime_) >= 70)
        //{
        //    IINFO() << id_ << " execution interval " << Utility::GetCurrentTimeMs() - lastExecutionTime_ << " should be " << sleepTime_;
        //}

        lastExecutionTime_ = Utility::GetCurrentTimeMs();
    }

    virtual int GetId() { return id_; }

private:
    Queue<ObjectProduced> &queue_;

    FlowId      id_;
    WeightId    weight_;
    int64       lastExecutionTime_;
    int         sleepTime_;
};

/**
 * @brief The Consumer class
 */
class Consumer : public Runnable
{
public:
    Consumer(Queue<ObjectProduced> &queue, int sleepTime, int id)
        : queue_(queue)
        , id_(id)
        , lastExecutionTime_(Utility::GetCurrentTimeMs())
        , sleepTime_(sleepTime)
    {}
    ~Consumer() {}

    virtual void run()
    {
        if(!queue_.IsEmpty())
        {
            ObjectProduced cnt = queue_.Dequeue();

            //if(cnt.IsValid())
            //{
                static Mutex mutex;

                MutexLocker loc(&mutex);
                resultMap_[cnt.GetWeight()]++;
            //}

            //if(cnt.IsValid() == false)
            //    IDEBUG() << "Consumed invalid object";
            //else
            //IDEBUG() << id_ << " consumed: " << cnt; //queue_.consumer_pop_front();
        }

        //if(std::abs( (Utility::GetCurrentTimeMs() - lastExecutionTime_) - sleepTime_) >= 70)
        //{
        //    IINFO() << id_ << " execution interval " << Utility::GetCurrentTimeMs() - lastExecutionTime_ << " should be " << sleepTime_;
        //}

        lastExecutionTime_ = Utility::GetCurrentTimeMs();
    }

    virtual int GetId() { return id_; }

    static void PrintResult()
    {
        int total = 0;

        IDEBUG() << "Counters:";
        for(std::map<int, int>::iterator it = resultMap_.begin(), it_end = resultMap_.end(); it != it_end; ++it)
        {
            IDEBUG() << it->first << "\t" << it->second;
            total += it->second;
        }

        IDEBUG() << "Total \t" << total;

        IDEBUG() << "Percentage:";

        for(std::map<int, int>::iterator it = resultMap_.begin(), it_end = resultMap_.end(); it != it_end; ++it)
        {
            IDEBUG() << it->first << "\t" << (double) it->second / (double) total;
        }
    }

private:
    static std::map<int, int> resultMap_;

private:
    Queue<ObjectProduced> &queue_;
    int id_;
    int64   lastExecutionTime_;
    int     sleepTime_;
};

std::map<int, int> Consumer::resultMap_;

void testProducerConsumer()
{
    //BlockingQueue<int> queue(50);
    //Collection::ConcurrentWFQueue<ObjectProduced, Collection::FlowInfo, RWFQueue<ObjectProduced> > queue(50);
    Collection::ConcurrentWFQueue<ObjectProduced, Collection::FlowInfo, RWFQueue<ObjectProduced> > queue(15);

    //BaseLib::Templates::TypeName<std::string> poolName; //(std::string("testPool"));
    //poolName.SetName("hallo");

    std::string poolName("hallo");

    ThreadPool threadPool(poolName); //, QosPool(QosThreadCount(1, 0)));
    threadPool.Start();

    int i = 1;
    int sleepTime = 5;

    for( ; i < 2; i++)
    {
        threadPool.Schedule(new Consumer(queue, sleepTime, i), TaskPolicy::RunForever(sleepTime));
    }

    for( ; i < 3; i++)
    {
        threadPool.Schedule(new Producer(queue, sleepTime, 1, 50), TaskPolicy::RunForever(sleepTime));
    }

    for( ; i < 4; i++)
    {
        threadPool.Schedule(new Producer(queue, sleepTime, 2, 40), TaskPolicy::RunForever(sleepTime));
    }

    for( ; i < 5; i++)
    {
        threadPool.Schedule(new Producer(queue, sleepTime, 3, 10), TaskPolicy::RunForever(sleepTime));
    }

    threadPool.wait(60000);


    Consumer::PrintResult();
}

}

// -----------------------------------------------------------
//  ThreadPoolTests
// -----------------------------------------------------------
namespace ThreadPoolTest
{

class Worker : public BaseLib::Runnable
{
public:
    Worker(int32 sleepTime, int id)
        : sleepTime_(sleepTime)
        , lastExecutionTime_(Utility::GetCurrentTimeMs())
        , id_(id)
    {

    }
    virtual ~Worker()
    {
        //IDEBUG() << "Destructed";
    }

private:
    virtual void run()
    {
        int64 executionTime = Utility::GetCurrentTimeMs() - lastExecutionTime_;

        //if(executionTime > (sleepTime_ + 50))
        //{
            IINFO() << id_ << " execution interval " << Utility::GetCurrentTimeMs() - lastExecutionTime_ << " should be " << sleepTime_;
        //}

        //ASSERT(std::abs( (Utility::GetCurrentTimeMs() - lastExecutionTime_) - sleepTime_) < 30);

        lastExecutionTime_ = Utility::GetCurrentTimeMs();
    }

    virtual int GetId() { return id_; }

private:
    int32   sleepTime_;
    int64   lastExecutionTime_;
    int     id_;
};

class TestTask //: public Callable<std::string>
{
public:
    //TestTask() {}
    virtual ~TestTask() {}

    std::string call()
    {
        IDEBUG() << "I am here";
        return std::string("BLUE");
    }

    std::string Blue()
    {
        IDEBUG() << "I am here";
        return std::string("FINAL");
    }

    std::string Final()
    {
        IDEBUG() << "I am here";
        return std::string("FINAL");
    }

    std::string Hallo(std::string arg)
    {
        IDEBUG() << arg;
        return std::string("blah to you too");
    }

    double Man(std::string arg)
    {
        IDEBUG() << arg;
        return 5.34;
    }
};

void testStateMachine()
{
    TestTask testTask;
    Concurrent::StateMachineObjects<std::string> machine;

    machine.CreateStartState("START")->AddAction<TestTask>(&testTask, &TestTask::call);
    machine.GetOrCreateState("BLUE")->AddAction<TestTask>(&testTask, &TestTask::Blue);
    machine.CreateFinalState("FINAL")->AddAction<TestTask>(&testTask, &TestTask::Final);

    //machine.run();

    while(machine.ExecuteNext() == false);
}

void testFutureTask()
{
    //BaseLib::Templates::TypeName<std::string> poolName; //(std::string("testPool"));
    //poolName.SetName("hallo");

    std::string poolName("hallo");

    ThreadPool threadPool(poolName); //, QosPool(QosThreadCount(30, 0)));

    TestTask testTask;

    FutureTask<std::string>::Ptr futureTask = FutureFactory::Create<std::string, TestTask, std::string>(&testTask, &TestTask::Hallo, std::string("blahdibla"));

    threadPool.Schedule<std::string>(futureTask, TaskPolicy::RunOnceImmediately());

    futureTask->WaitFor();

    IDEBUG() << "Got result " << futureTask->Result().second;

    FutureTasks futureTasks;

    InstanceHandle strHandle = futureTasks.AddTask<std::string, TestTask, std::string>(&testTask, &TestTask::Hallo, std::string("hodlihodli"));
    InstanceHandle anothStrHandle = futureTasks.AddTask<std::string, TestTask, std::string>(&testTask, &TestTask::Hallo, std::string("hodlihodliagain"));
    InstanceHandle dblHandle = futureTasks.AddTask<double, TestTask, std::string>(&testTask, &TestTask::Man, std::string("hodlihodli"));

    threadPool.Schedule(&futureTasks, TaskPolicy::RunOnceImmediately());

    futureTasks.WaitFor();

    IDEBUG() << "Got " << futureTasks.Result<double>(dblHandle).second;
    IDEBUG() << "Got " << futureTasks.Result<std::string>(strHandle).second;
    //IDEBUG() << "Got " << futureTasks.Result<std::string>(dblHandle).second;


    InstanceHandleSet handleSet = futureTasks.GetHandlesWithReturnType<std::string>();
    for(InstanceHandleSet::iterator it = handleSet.begin(), it_end = handleSet.end(); it != it_end; ++it)
    {
        InstanceHandle handle = *it;
        IDEBUG() << "Got " << futureTasks.Result<std::string>(handle).second;
    }

//    handleSet = futureTasks.GetHandlesFromClass<std::string, TestTask, std::string>();
//    for(InstanceHandleSet::iterator it = handleSet.begin(), it_end = handleSet.end(); it != it_end; ++it)
//    {
//        InstanceHandle handle = *it;
//        IDEBUG() << "Got " << futureTasks.Result<std::string>(handle).second;
//    }
}

void testThreadPool()
{
    std::string poolName("hallo");

    ThreadPool::Ptr threadPool = ThreadPoolFactory::Instance().Create(poolName);

    int i = 1;

    threadPool->Schedule(new Worker(1000, i++), TaskPolicy::RunForever(1000));
    threadPool->Schedule(new Worker(2000, i++), TaskPolicy::RunForever(2000));
    threadPool->Schedule(new Worker(3000, i++), TaskPolicy::RunForever(3000));
    threadPool->Schedule(new Worker(4000, i++), TaskPolicy::RunForever(4000));
    threadPool->Schedule(new Worker(5000, i++), TaskPolicy::RunForever(5000));
    threadPool->Schedule(new Worker(600, i++), TaskPolicy::RunForever(600));
    threadPool->Schedule(new Worker(300, i++), TaskPolicy::RunForever(300));

    for( ; i < 500; i++)
    {
        threadPool->Schedule(new Worker(600, i), TaskPolicy::RunForever(600));
    }

    for( ; i < 1000; i++)
    {
        threadPool->Schedule(new Worker(600, i), TaskPolicy::RunForever(600));
    }

    IDEBUG() << "Number of workers " << i;

    threadPool->Start();

    threadPool->wait(10000);
    threadPool->Report(std::cout);

    //threadPool->ClearTasks();
    //Thread::sleep(3);

//    for( ; i < 1500; i++)
//    {
//        threadPool->Schedule(new Worker(600, i), TaskPolicy::RunForever(600));
//    }

//    for( ; i < 2000; i++)
//    {
//        threadPool->Schedule(new Worker(100, i), TaskPolicy::RunForever(100));
//    }

//    threadPool->wait(50000);

//    threadPool->Report(std::cout);

//    threadPool->ClearTasks();

    //IDEBUG() << "Cleared all tasks now stopping";
    //threadPool.Stop();
}

} // namespace ThreadPoolTest

// -----------------------------------------------------------
//  EventHolderTest
// -----------------------------------------------------------
/*namespace EventHolderTest
{
class Observer
{
public:
    virtual void onUpdate(int cnt) = 0;
    virtual void onUpdate(void) = 0;

};

class ConcreteObserver : public Observer
{
public:
    ConcreteObserver() {}

    virtual void onUpdate(int cnt)
    {
        IDEBUG() << "The cnt is " << cnt;
    }

    virtual void onUpdate(void)
    {
        IDEBUG() << "Is updated!";
    }
};

class Subject
{
public:
    Subject() { }

    void doUpdate(int cnt)
    {
        IDEBUG() << "The cnt is " << cnt;

        onUpdateEvent_.Notify(cnt);
    }


    void Attach(Observer &obs)
    {
        onUpdateEvent_.Attach<Observer>(&obs, &Observer::onUpdate);
    }

    //void Detach(Observer &obs)
    //{
    //    onUpdateEvent_.Detach<Observer>(&obs, &Observer::onUpdate);
    //}

    OneArgEvent<void, int>& GetOnUpdateEvent() { return onUpdateEvent_; }

private:
    OneArgEvent<void, int>  onUpdateEvent_;
};

void testOneArgEvent()
{
    Subject sub;

    for(int i = 0; i < 50; i++)
    {
        Observer *obs = new ConcreteObserver();
        sub.Attach(*obs);

        //sub.GetOnUpdateEvent().Attach<Observer>(obs, &Observer::onUpdate);
    }

    sub.doUpdate(5);
}

} // namespace EventHolderTest

*/
// -----------------------------------------------------------
//  CommandHolderTest
// -----------------------------------------------------------

/*namespace CommandHolderTest
{

class Observer
{
public:
    virtual void onCommand(int cnt) = 0;
};

class ConcreteObserver : public Observer
{
public:
    ConcreteObserver() {}

    virtual void onCommand(int cnt)
    {
        IDEBUG() << "The cnt is " << cnt;
    }
};

class Subject
{
public:
    Subject() { }

    void doCommand(int cnt)
    {
        IDEBUG() << "The cnt is " << cnt;

        onUpdateCommand_.Execute(cnt);
    }

    void Enqueue(Observer &obs)
    {
        onUpdateCommand_.Enqueue<Observer>(&obs, &Observer::onCommand);
    }

private:
    BaseLib::Concurrent::OneArgCommandQueue<void, int>  onUpdateCommand_;
};

void testCommandHolder()
{
    Subject sub;

    for(int i = 0; i < 50; i++)
    {
        Observer *obs = new ConcreteObserver();
        sub.Enqueue(*obs);
    }

    sub.doCommand(5);
}

} // namespace CommandHolderTest
*/

// -----------------------------------------------------------
//  SlotHolderTest
// -----------------------------------------------------------
namespace SlotHolderTest
{
class Observer
{
public:
    virtual void onUpdate(int cnt) = 0;
    virtual void onUpdateAgain(int cnt) = 0;
};

class ConcreteObserver : public Observer
{
public:
    ConcreteObserver()
    {}

    virtual void onUpdate(int cnt)
    {
        IDEBUG() << "The cnt is " << cnt;
    }

    virtual void onUpdateAgain(int cnt)
    {
        IDEBUG() << "The cnt is still " << cnt;
    }
};

class Subject
{
public:
    Subject()
        : onUpdateSignal_(new Signaller1<void, int>())
    { }

    void doUpdate(int cnt)
    {
        //IDEBUG() << "The cnt is " << cnt;
        onUpdateSignal_->AsynchSignal(cnt);
    }

    //void Attach(Observer &obs)
    //{
    //    onUpdateSignal_.Attach<Observer>(&obs, &Observer::onUpdate);
    //}

    //void Detach(Observer &obs)
    //{
    //    onUpdateEvent_.Detach<Observer>(&obs, &Observer::onUpdate);
    //}

    Signaller1<void, int>::Ptr GetSignal() { return onUpdateSignal_->GetPtr(); }

private:
    Signaller1<void, int>::Ptr  onUpdateSignal_;
};

void testSlots()
{
    Subject *sub = new Subject();

    std::vector<Observer*> observers;

    for(int i = 0; i < 50; i++)
    {
        Observer *obs = new ConcreteObserver();

        //OneArgSlot<Observer, void, int> *onUpdateSlot = new OneArgSlot<Observer, void, int>(obs, &Observer::onUpdate);
        //onUpdateSlot->Connect(&sub.GetSignal());
        sub->GetSignal()->Connect<Observer>(obs, &Observer::onUpdate);
        //sub.GetSignal().Connect<Observer>(obs, &Observer::onUpdate);
        //sub.GetSignal().Connect<Observer>(obs, &Observer::onUpdate);
        //sub.GetSignal().Connect<Observer>(obs, &Observer::onUpdate);
        sub->GetSignal()->Connect<Observer>(obs, &Observer::onUpdateAgain);

        observers.push_back(obs);
    }

    sub->doUpdate(5);

    for(size_t i = 0; i < observers.size(); i++)
    {
        Observer *obs = observers[i];

        bool isRemoved = sub->GetSignal()->Disconnect<Observer>(obs); //, &Observer::onUpdate);
        ASSERT(isRemoved);

        //isRemoved = sub->GetSignal()->Disconnect<Subject>(sub); //, &Observer::onUpdate);
        //ASSERT(isRemoved == false);

        //isRemoved = sub->GetSignal().Disconnect<Observer>(obs, &Observer::onUpdateAgain);
        //ASSERT(isRemoved);
        //sub.GetSignal().
    }

    sub->doUpdate(4);


    Thread::sleep(5);
}


void testObserverSubjects() {

    BaseLib::Templates::KeyValueSubject<int, std::string> keyValueSubject;

    keyValueSubject.OnObjectCreated(1, "alle");
}


}

/**
 * @brief main
 * @return
 */
int main()
{
    ::srand(22345);					// initializes rand()

    IDEBUG() << "Hello World!";

    std::cout << "hello" << std::endl;

    //SlotHolderTest::testSlots();
    //SlotHolderTest::testObserverSubjects();

    //EventHolderTest::testOneArgEvent();
    ThreadPoolTest::testThreadPool();
//    ThreadPoolTest::testFutureTask();
    //ProducerConsumerTest::testProducerConsumer();
//    ThreadPoolTest::testStateMachine();

    //ThreadPoolFactory::Instance().GetDefault()->Stop();

    return 0;
}

