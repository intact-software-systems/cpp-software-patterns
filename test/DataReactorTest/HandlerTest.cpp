#include "test/DataReactorTest/HandlerTest.h"
#include <RxCommand/CommandControllerPolicy.h>

namespace DataReactorTest
{

std::ostream &operator<<(std::ostream &ostr, const IOState &state)
{
    return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const IOEventInput &state)
{
    return ostr;
}

HandlerTest::HandlerTest()
{
}

bool HandlerTest::Initialize()
{
    Reactor::RxHandlerChain<IOEventChainKind::Type>::Ptr handlerChain(new Reactor::RxHandlerChain<IOEventChainKind::Type>());

//    handlerChain->AddHandler(IOEventChainKind::ReadyRead);

    return true;
}

void setupStateMachineTest()
{
    Reactor::StateTransitionsBuilder<IOState, int, IOEventInput> builder;

    builder.HandlerFor(
            IOState::ReadyRead,
            Reactor::State<IOState, int, IOEventInput>(
                    IOState::ReadyRead,
                    Reactor::StateDescription::Default(),
                    Reactor::CommandControllerPolicy::Default(),
                    RxThreadPoolFactory::CommandPool()
            )
    );

    builder.HandlerFor(
            IOState::Datagram,
            Reactor::State<IOState, int, IOEventInput>(
                    IOState::Datagram,
                    Reactor::StateDescription::Default(),
                    Reactor::CommandControllerPolicy::Default(),
                    RxThreadPoolFactory::CommandPool()
            )
    );


    builder.HandlerFor(
            IOState::Deserialize,
            Reactor::State<IOState, int, IOEventInput>(
                    IOState::Deserialize,
                    Reactor::StateDescription::Default(),
                    Reactor::CommandControllerPolicy::Default(),
                    RxThreadPoolFactory::CommandPool()
            )
    );


    builder.HandlerFor(
            IOState::Export,
            Reactor::State<IOState, int, IOEventInput>(
                    IOState::Export,
                    Reactor::StateDescription::Default(),
                    Reactor::CommandControllerPolicy::Default(),
                    RxThreadPoolFactory::CommandPool()
            )
    );

    builder.Configure(IOState::ReadyRead).InputActions();

    builder.TransitionsFor(IOState::ReadyRead).insert(IOState::Datagram);
    builder.TransitionsFor(IOState::Datagram).insert(IOState::Deserialize);
    builder.TransitionsFor(IOState::Deserialize).insert(IOState::Export);

    builder.Permit(IOState::ReadyRead, IOState::Datagram, IOEventInput::Read);
    builder.Permit(IOState::Datagram, IOState::Deserialize, IOEventInput::Read);
    builder.Permit(IOState::Deserialize, IOState::Export, IOEventInput::Read);

    Reactor::StateTransitionTable<IOState, int, IOEventInput> table = builder.Build();

    //Reactor::StateMachineController<IOState, int, IOEventInput, ExampleContext> stateMachine(ExampleContext(), table);


}

int TestAction::Invoke(IOEventInput input)
{
    return 0;
}

}
