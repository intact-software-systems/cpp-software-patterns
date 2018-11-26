#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandControllerAction.h"
#include"RxCommand/Export.h"
#include"RxCommand/ReactorConfig.h"

namespace Reactor
{

// ---------------------------------------------------------------
// Class CommandFactoryPolicy
// ---------------------------------------------------------------

class DLL_STATE CommandFactoryPolicy
{};

// ---------------------------------------------------------------
// Class CommandFactoryState
// ---------------------------------------------------------------

class DLL_STATE CommandFactoryState
{
public:
    virtual ~CommandFactoryState() {}

    ReactorConfig::CommandControllerAccess executors() const;
};

// ---------------------------------------------------------------
// Class CommandFactory
// ---------------------------------------------------------------

class DLL_STATE CommandFactory
        : public Templates::FactorySingleton<CommandFactory>
        , public Templates::ContextObject<CommandFactoryPolicy, CommandFactoryState>
{
public:
    CommandFactory();
    virtual ~CommandFactory();

    template <typename T>
    static typename CommandActions<T>::Ptr CreateCommand(CommandPolicy policy, typename Templates::Action0<T>::Ptr action)
    {
        typename CommandActions<T>::Ptr command = std::make_shared<CommandActions<T>>(policy);
        command->Add(action);
        return command;
    }

    template <typename Return>
    typename CommandControllerAction<Return>::Ptr CreateCommandController(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
    {
        typename CommandControllerAction<Return>::Ptr commandController(new CommandControllerAction<Return>(policy, threadPool));

        commandController->SetCommandThreadPool(RxThreadPoolFactory::CommandPool());

        this->state().executors().Write(commandController, commandController);
        return commandController;
    }

    template <typename Return>
    typename CommandControllerAction<Return>::Ptr CreateCommandController()
    {
        typename CommandControllerAction<Return>::Ptr commandController(new CommandControllerAction<Return>(
                                                                            CommandControllerPolicy::Default(),
                                                                            RxThreadPoolFactory::ControllerPool()));

        commandController->SetCommandThreadPool(RxThreadPoolFactory::CommandPool());

        this->state().executors().Write(commandController, commandController);
        return commandController;
    }


    template <typename K, typename V>
    typename CommandControllerKeyValue<K,V>::Ptr CreateCommandControllerKeyValue(
            CommandControllerPolicy policy,
            RxThreadPool::Ptr threadPool)
    {
        typename CommandControllerKeyValue<K,V>::Ptr commandController(
                new CommandControllerKeyValue<K,V>(
                        policy,
                        threadPool == nullptr ? RxThreadPoolFactory::ControllerPool() : threadPool
                )
        );

        commandController->SetCommandThreadPool(RxThreadPoolFactory::CommandPool());

        this->state().executors().Write(commandController, commandController);
        return commandController;
    }

private:

    // ------------------------------------------
    // private functions
    // ------------------------------------------

    template<typename K, typename V>
    static CommandControllerAction<std::pair<K, V>>* createCommandController(
            CommandControllerPolicy commandControllerPolicy,
            CommandObserver<std::pair<K, V>> *,
            RxThreadPool::Ptr threadPool)
    {
        CommandControllerAction<std::pair<K, V>> *cmdController = new CommandControllerAction<std::pair<K, V>>(commandControllerPolicy, RxThreadPoolFactory::ControllerPool());

        if (threadPool != nullptr)
            cmdController->SetCommandThreadPool(threadPool);
        else
            cmdController->SetCommandThreadPool(RxThreadPoolFactory::CommandPool());

//        if (observer != nullptr)
//            cmdController->connectDataObserver(observer);

        // CommandControllers are cached since policy handling is done by CommandMonitor
        //CommandFactory::Instance().state().controllercache<K, V>(instance().config()).put(cmdController, cmdController);

        return cmdController;
    }
};

}
