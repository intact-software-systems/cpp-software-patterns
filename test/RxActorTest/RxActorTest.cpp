#include <BaseLib/Debug.h>
#include <BaseLib/ScopedTimers.h>
#include <RxActor/ActorComposer.h>
#include "gtest/gtest.h"

using namespace BaseLib;

TEST(RxActor, aBasicTest)
{
    ScopedTimer a(IINFO());

    const std::string   name = "yeah";
    const ChannelHandle handle{name, name, name};

    RxActor::ActorController<std::string> controller = RxActor::ActorComposer<std::string>::From(handle)
        ->Processor()
        ->Context()->Then()
        ->Process(
            RxActor::ActorId::Create(), [](RxActor::Actor<std::string>* actor, std::string value) -> bool
            {
                actor->Send("hei");
                IINFO() << "Got value: " << value;
                return true;
            }
        )
        ->Processor()
        ->Process(
            RxActor::ActorId::Create(), [](RxActor::Actor<std::string>* actor, std::string value) -> bool
            {
                actor->Send("hei");
                IINFO() << "Got value: " << value;
                return true;
            }
        )
        ->Build();

    controller.SendVia("atest", RxActor::ActorId::Create());
}
