#pragma once

#include "Processor.h"
#include "ActorController.h"
#include "Actor.h"

namespace RxActor {

template <typename T>
class ActorComposer
{
private:
    const ChannelHandle handle_;

    ActorGraph* actorGraph;

public:
    ActorComposer(ChannelHandle handle)
        : handle_(handle)
    { }

    virtual ~ActorComposer()
    { }

    class ProcessorComposer;

    class ProcessorContextComposer
    {
    public:
        ProcessorContextComposer(ProcessorComposer* composer)
            : composer_(composer)
        { }

        virtual ~ProcessorContextComposer()
        { }

        ProcessorContextComposer* OnPreStartDo(std::function<void()> onPreStart)
        {
            onPreStart_ = onPreStart;
            return this;
        }

        ProcessorContextComposer* OnPostStopDo(std::function<void()> onPostStop)
        {
            onPostStop_ = onPostStop;
            return this;
        }

        ProcessorContextComposer* OnPreRestartDo(std::function<void(Exception)> onPreRestart)
        {
            onPreRestart_ = onPreRestart;
            return this;
        }

        ProcessorContextComposer* OnPostRestartDo(std::function<void(Exception)> onPostRestart)
        {
            onPostRestart_ = onPostRestart;
            return this;
        }

        ProcessorContextPtr Build()
        {
            return std::make_shared<ProcessorContextInvoker>(onPreStart_, onPostStop_, onPreRestart_, onPostRestart_);
        }

        ProcessorComposer* Then()
        {
            return composer_;
        }

    private:
        std::function<void()>          onPreStart_    = [] { };
        std::function<void()>          onPostStop_    = [] { };
        std::function<void(Exception)> onPreRestart_  = [](Exception e) { };
        std::function<void(Exception)> onPostRestart_ = [](Exception e) { };

    private:
        ProcessorComposer* composer_;
    };

    /**
     * Build a std::function<void (T) > processor
    */
    class ProcessorComposer
    {
    public:
        ProcessorComposer(ActorComposer<T>* composer)
            : composer_(composer)
            , contextComposer_(std::make_unique<ProcessorContextComposer>(this))
        { }

        virtual ~ProcessorComposer()
        { }

        std::unique_ptr<ProcessorContextComposer>& Context()
        {
            return contextComposer_;
        }

        /**
         *  Compose an actor system. An ActorComposer should support:
         *  - Content-based routing
         *  - Message filter on content, etc
         *  - Dynamic router
         *  - Message transformation
         *  - Selective consumers (based on routers).
         *
         *  For example: composer.actor(a).routeTo(b).or(c).or(d).criteria(routerFunction);
         */
        ActorComposer<T>* Process(const ActorId& actorId, std::function<bool(RxActor::Actor<T>*, T)> processor)
        {
//            std::shared_ptr<details::Actor<T>> actor = std::make_shared<details::Actor<T>>(
//                composer_.handle_,
//                actorId,
//                std::make_shared<ProcessorInvoker<T>>(
//                    contextComposer_.Build(),
//                    processor
//                ),
//                Supervisor(RxActor::DeciderAlgorithm),
//                ActorPolicy{SupervisorPolicy{SupervisorStrategy::AllForOne, Policy::Criterion::All()}, TaskPolicy::Default()}
//
//            );
            // TODO: composer_->actorGraph->Add(actor);

            return composer_;
        }

        ProcessorComposer* RouteTo(std::function<bool(RxActor::Actor<T>*, T)> router)
        {
            return this;
        }

        void Producer(std::function<T ()> producer)
        {
            // Function that provides input to actor
        }

        /**
         *
         * Build lambda collection that works as follows:
         *
         * input: value
         *
         * if(condition(value)) A();
         * if(condition(value)) B();
         * if(condition(value)) C();
         */
//        ProcessorComposer* ReceiverWithCondition(std::function<> condition)
//        {
//            return this;
//        }

        ProcessorComposer* SupervisorPolicy(int maxNrOfRetries, Duration withinTimeRange)
        {
            return this;
        }

        ProcessorComposer* SupervisorConditions(std::function<Policy::ReactionKind(BaseLib::Exception)> condition)
        {
            // Add to list of conditions
            return this;
        }


    private:
        std::unique_ptr<ProcessorContextComposer> contextComposer_;

    private:
        ActorComposer<T>                                      * composer_;
        std::function<bool(RxActor::Actor<T>*, T)>            processor_;

        std::list<std::function<bool(RxActor::Actor<T>*, T)>> conditions_;

        // TODO: List of routers based on conditions?
        // TODO: filter list
        // TODO: Supervisor strategy builder

    };


    static std::unique_ptr<ActorComposer<T>> From(ChannelHandle handle)
    {
        return std::make_unique<ActorComposer>(handle);
    }

    std::unique_ptr<ProcessorComposer> Processor()
    {
        return std::make_unique<ProcessorComposer>(this);
    }

    ActorController<T> Build()
    {
        return ActorController<T>(actorGraph, new ActorControllerPolicy());
    }
};

}
