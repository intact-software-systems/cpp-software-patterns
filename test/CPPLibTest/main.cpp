#include "BaseLib/IncludeLibs.h"
#include "RxData/IncludeLibs.h"
#include "NetworkLib/IncludeLibs.h"

using namespace BaseLib;
using namespace BaseLib::Collection;
using namespace BaseLib::Concurrent;
using namespace NetworkLib;

namespace CPPLibTest
{

void testSelector()
{
    SocketReactor::Ptr coordinator(new SocketReactor());

    coordinator->StartReactor();

    TcpServer::Ptr socket = coordinator->CreateAndAddServerSocket();

    IDEBUG() << "Socket: " << socket;

    if(socket->isValid())
    {
        NetworkLib::TCPConnectorStrategy::Ptr strategy(
                        new NetworkLib::TCPConnectorStrategy(
                            *(NetworkLib::NetworkInterface::allAddresses().begin()),
                            socket->GetPortNumber()
                        )
                    );

//        Reactor::CommandResult result = coordinator->ConnectToHost(strategy);
//        bool isDone = result.Wait();
//        Thread::sleep(5);

        IDEBUG() << "\n" << *DebugReport::GetOrCreate();
    }
}

}

int main(int argc, char *argv[])
{
    IDEBUG() << NetworkLib::NetworkInterface::allAddresses();
    IDEBUG() << NetworkLib::NetworkInterface::allInterfaces();


    CPPLibTest::testSelector();

    return 0;
}
