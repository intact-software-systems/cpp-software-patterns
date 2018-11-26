#pragma once

#include "BaseLib/IncludeLibs.h"
#include "RxData/IncludeLibs.h"
//#include "NetworkLib/IncludeLibs.h"
#include "DataReactor/IncludeLibs.h"
#include "RxFSM/IncludeLibs.h"

namespace DataReactorTest
{

enum class IOState
{
    ReadyRead,
    Datagram,
    Deserialize,
    Export
};

enum class IOEventInput
{
    Read,
    Stop
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const IOState &state);
DLL_STATE std::ostream& operator<<(std::ostream &ostr, const IOEventInput &state);

namespace IOEventChainKind
{
    enum Type
    {
        ReadyRead,
        Datagram,
        Deserialize,
        Export
    };
}

class ReadyReadSelector : public Templates::EventSelectorMethod<Collection::ISet<int>, int>
{
public:
    ReadyReadSelector()
    {}
    virtual ~ReadyReadSelector()
    {}

    virtual Collection::ISet<int> Select(int64 )
    {
        Collection::ISet<int> keys;
        keys.insert(5);
        return keys;
    }
};

class HandlerTest
        : public Templates::InitializeMethod
{
public:
    HandlerTest();

    virtual bool Initialize();
};

class ExampleContext
        : public Templates::ActionContext
{
public:
    // ExitMethod interface
    virtual void Exit()
    {
    }

    // EntryMethod interface
    virtual void Entry()
    {
    }
};

class TestAction : public Reactor::StateMachineAction1<ExampleContext, int, IOEventInput>
{
public:
    TestAction(std::shared_ptr<ExampleContext> const &context) : StateMachineAction1(context)
    { }

    virtual int Invoke(IOEventInput input);
};

void setupStateMachineTest();

}
