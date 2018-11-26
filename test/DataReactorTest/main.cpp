#include <RxMonitor/CommandMonitor.h>
#include "test/DataReactorTest/HandlerTest.h"

using namespace BaseLib;
using namespace BaseLib::Collection;
using namespace BaseLib::Concurrent;
using namespace BaseLib::Templates;
//using namespace Reactor;
//using namespace NetworkLib;

//using namespace DataManagement;
//using namespace DataManagement::Cache;
//using namespace DataManagement::Machine;

namespace DataReactorTest {

void test()
{
//    Reactor::RxScheduler::Ptr<AbstractSocket::Ptr, > rxScheduler = Reactor::ReactorFactory::Instance().CreateRxScheduler();


}

class AStrategy
        : public Strategy1<bool, const int &>
        , public StaticSingletonPtr<AStrategy>
{
public:
    virtual bool Perform(const int &t)
    {
        IINFO() << t;
        return true;
    }
};

class KeyValueStrategy
        : public Strategy1<IMap<int, int>, int>
        , public StaticSingletonPtr<KeyValueStrategy>
{
public:
    virtual ~KeyValueStrategy()
    {
        IDEBUG() << "Destructed";
    }

    virtual IMap<int, int> Perform(int t)
    {
        IMap<int, int> map;
        map.put(t, t);
        IINFO() << t;

        return map;
    }
};


class Producer : public BaseLib::RxObserverSubjectNew<int>
{
public:

};


class Consumer
        : public BaseLib::RxObserverNew<int>
        , public ENABLE_SHARED_FROM_THIS(Consumer)
{
public:
    Consumer()
    {}
    virtual ~Consumer()
    {
        IDEBUG() << "Destructing";
    }

    CLASS_TRAITS(Consumer)

    virtual bool OnComplete()
    {
        IDEBUG() << "Complete:";
        return true;
    }

    virtual bool OnNext(int t)
    {
        IDEBUG() << "Got " << t;
        return true;
    }

    virtual bool OnError(GeneralException exception)
    {
        IDEBUG() << exception.msg();
        return true;
    }
};


class SimpleAction : public Action1<int, int>
{
public:
    virtual ~SimpleAction() {}

    CLASS_TRAITS(SimpleAction)

    virtual int Invoke(int a)
    {
        IINFO() << "Hallo: " << a;
        return a + 1;
    }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }
};


class KeyValueSimpleAction : public Action1<IMap<int, int>, int>
{
public:
    virtual ~KeyValueSimpleAction() {}

    CLASS_TRAITS(KeyValueSimpleAction)

    virtual IMap<int, int> Invoke(int a)
    {
        IMap<int, int> aMap;
        aMap.put(a,a);

        IINFO() << "Hallo: " << a;

        return aMap;
    }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }
};


class MementoAction : public MementoAction1<int, int, int>
{
public:
    MementoAction()
    {
        SetOriginatorState(10);
    }

    CLASS_TRAITS(MementoAction)

    virtual int Invoke(int a)
    {
        IINFO() << "Hallo: " << a;

        this->SetOriginatorState(a);

        return a + 1;
    }

    virtual bool Cancel()
    {
        return false;
    }

    virtual bool IsDone() const
    {
        return false;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }
};

}

void testObservers()
{
    DataReactorTest::Producer producer;

    for(int i = 0; i < 10; i++)
    {
        producer.event().Next(i);
    }

    DataReactorTest::Consumer::Ptr consumer = std::make_shared<DataReactorTest::Consumer>();
    consumer->event().Subscribe(consumer->shared_from_this());

    DataReactorTest::Consumer::Ptr consumer1 = std::make_shared<DataReactorTest::Consumer>();
    consumer1->event().Subscribe(consumer->shared_from_this());

    DataReactorTest::Consumer::Ptr consumer2 = std::make_shared<DataReactorTest::Consumer>();
    consumer2->event().Subscribe(consumer->shared_from_this());

    Concurrent::Event::Ptr event = producer.event().Complete();
    bool isReceived = event->WaitFor(50000);

    ASSERT(isReceived);

    //consumer->event().Unsubscribe();
}

void testMementoCommand()
{
    using namespace Reactor;

    DataReactorTest::MementoAction::Ptr action(new DataReactorTest::MementoAction());

    MementoCommandActions<int>::Ptr command(new MementoCommandActions<int>());
    command->Add<int, int>(action, 9);

    MementoCommandActions<int>::Ptr command1(new MementoCommandActions<int>());
    command1->Add<int, int>(action, 8);

    MementoCommandActions<int>::Ptr command2(new MementoCommandActions<int>());
    command2->Add<int, int>(action, 7);

    MementoCommandController<int>::Ptr controller(
                new MementoCommandController<int>(
                CommandControllerPolicy::SequentialAnd(
                        Policy::Ordering::Fifo(),
                        Policy::Timeout::FromSeconds(30)),
                RxThreadPoolFactory::ControllerPool())
    );

    controller->Add(command);
    controller->Add(command1);
    controller->Add(command2);

    CommandControllerSubscription<int> sub = controller->Execute();

    //Reactor::CommandSubscription<int> sub = command->Execute(Reactor::RxThreadPoolFactory::CommandPool());

    sub.WaitFor();

    IINFO() << "Data : " << action->GetOriginatorState();

    controller->Undo();


//    command->Undo();
//    command->Undo();
//    command->Undo();
//    command->CreateMemento();

    //command->Redo();

    IINFO() << "Data : " << action->GetOriginatorState();

}

void testCommand()
{
    using namespace Reactor;

    DataReactorTest::SimpleAction::Ptr action(new DataReactorTest::SimpleAction());

    CommandActions<int>::Ptr command(new CommandActions<int>(CommandPolicy::Default()));

    CommandActions<int>::Ptr command1(new CommandActions<int>(CommandPolicy::Default()));

    CommandActions<int>::Ptr command2(new CommandActions<int>(CommandPolicy::Default()));

    command->Add<int>(action, 5);
    command1->Add<int>(action, 4);
    command2->Add<int>(action, 3);

    CommandControllerAction<int>::Ptr controller(
                new CommandControllerAction<int>(
                CommandControllerPolicy::ParallelAnd(
                        Policy::Ordering::Fifo(),
                        Policy::Timeout::FromSeconds(30)),
                RxThreadPoolFactory::ControllerPool())
    );

    controller->SetCommandThreadPool(RxThreadPoolFactory::CommandPool());

    controller->Add(command);
    controller->Add(command1);
    controller->Add(command2);

    CommandControllerSubscription<int> subscription = controller->Execute();

    subscription.WaitFor(1000);

    FutureResult<IList<int>>::ResultHolder result = subscription.Result(4000);

    for(int i : result.second)
    {
        IDEBUG() << "command result " << i;
    }
}

void testDataAccess()
{
    using namespace Reactor;

    DataReactorTest::KeyValueSimpleAction::Ptr action(new DataReactorTest::KeyValueSimpleAction());

    CommandActions<IMap<int, int>>::Ptr command(new CommandActions<IMap<int, int>>(CommandPolicy::Default()));
    command->Add<int>(action, 5);


    DataAccess<int, int>::Ptr access(
                new DataAccess<int, int>(
                    RxData::CacheDescription("test"),
                    DataAccessPolicy::ReloadOnExpiry(CommandControllerPolicy::ParallelAnd(Policy::Ordering::Fifo(), Policy::Timeout::FromSeconds(30)),RxData::ObjectHomePolicy::Default())
                    )
                );

    access->Add(command);

    access->Subscribe();

    CommandControllerSubscription<IMap<int, int>> subscription = access->Result();

//    subscription.WaitFor(1000);

//    Thread::sleep(5);

    FutureResult<IList<IMap<int, int>>>::ResultHolder result = subscription.Result(4000);

    IDEBUG() << "command result " << result.first;
    IDEBUG() << "map size " << result.second.size();

    for(IMap<int, int> i : result.second)
    {
        for(std::pair<int, int> k : i)
        {
            IDEBUG() << "command result " << k.first << ","  << k.second;
        }
    }
}


void testStrategy()
{
//    using namespace DataReactorTest;
//    using namespace Reactor;

//    int i = 3;
//    AStrategy::Instance().Perform(3);

//    ActionContextPtr context(new ActionContext());
//    Reactor::Streamer streamer = Reactor::Streamer::CreateUnique();

//    streamer.Sequential().Chain<int, int, int>(5, KeyValueStrategy::InstancePtr());

//    streamer.Subscribe().WaitFor(4000);
}

void testStrategy2(int i)
{
    using namespace DataReactorTest;
    AStrategy::Instance().Perform(i);
}

void testFunction()
{
    std::function<void()> test = testStrategy;

    auto lambda = [] (int i) {  IINFO() << i; return true; };

    std::function<bool (int)> test1 = lambda; //[] (int i) {  IINFO() << i; return true; };

    std::function<void (int)> test2  = testStrategy2;

    std::function<bool (int)> test3 = std::bind(&DataReactorTest::AStrategy::Perform, &DataReactorTest::AStrategy::Instance(), std::placeholders::_1);

    test2(3);

    test3(2);

    test();

    test1(4);
}

void testfunctionPtr()
{
    using namespace DataReactorTest;

    std::shared_ptr<AStrategy> strategyPtr(new AStrategy());
    BaseLib::Templates::Function1<AStrategy, bool, const int &> func(strategyPtr, &AStrategy::Perform);


    func(1);
}

void testMonitor()
{
    Reactor::CommandMonitor::Supervisor();

    Thread::sleep(20);
}


static IMap<int, int> staticMethod()
{
    IINFO() << "Hello";
    return IMap<int, int>();
}

void testStreamer()
{
    Reactor::Streamer streamer = Reactor::Streamer::CreateUnique();

    streamer.Sequential().Chain<int, int, int>(5, [] (int input)
    {
        IINFO() << "lambda one input: " << input;

        IMap<int, int> a;
        a.put(1, input);
        return a;
    });

    Reactor::StreamerGroup parallel = streamer.Parallel();

    parallel.Chain<int, int>(&staticMethod);

    //auto func = []() { };

    for (int i = 0; i < 10; i++)
    {
        parallel.Chain<int, int, int>(i, [=](int b)
        {
            auto a = b + 5;

            IINFO() << "lambda key: " << b << " value: " << a;

            IMap<int, int> coll;
            coll.put(b, a);
            return coll;
        });
    }

    std::map<int, int> results = streamer
            .Subscribe()
            .WaitFor(20000)
            .Reader<int, int>()
            .getObjects();

    for (auto item : results)
    {
        IINFO() << "key: " << item.first << " value: " << item.second;
    }
}

void testComposing()
{
    auto func = [=](int a) -> int
    {
        IINFO() << "Composing " << a + 1;
        return a + 1;
    };

    auto composer = std::make_shared<Composer1<int, int>>(func);
    auto task = composer
        ->Then<int>(func)
        ->Then<int>(func)
        ->Then<int>(func)
        ->Execute(1);

    {
        ScopedTimer timer(IINFO());

        auto result = task.Result(1000);
        ASSERT(result.first.IsReady());

        IINFO() << "Executed chain and got " << result.second;
    }
}


int main(int argc, char* argv[])
{
    //InterruptHandling::InitSignalHandlers();

//    testObservers();
//    testCommand();
//    testDataAccess();
//    testStrategy();
//    testStrategy();

//    testfunctionPtr();
//    testFunction();

//    IINFO() << Utility::GetCurrentTimeMs();
//    IINFO() << INTACT_INT64_MAX << " ulong max" << LONG_MAX;

//    ElapsedTimer timer(LONG_MAX);

//    int64 long out = LONG_MAX;

//    IINFO() << " long max" << out;


//    ASSERT(!timer.HasExpired());

//    //testMonitor();
//    //testMementoCommand();

//    testStrategy();



    testStreamer();
//    testComposing();

    return 0;
}

