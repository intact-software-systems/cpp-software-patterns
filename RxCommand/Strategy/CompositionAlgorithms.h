#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/Strategy/ExecutionStrategy.h"
#include"RxCommand/Export.h"

namespace Reactor
{

struct DLL_STATE CompositionAlgorithms
{
    template <typename Return>
    static bool AggregateComposition(Policy::Criterion successCriterion, IList<Templates::Action0<Return>*> &actions, Command<Return> *command)
    {
        KeyValueRxEvents<Templates::Action0<Return>*, Return> *keyValueSubject = (KeyValueRxEvents<Templates::Action0<Return>*, Return>*) command;
        RxEventsReturn<Return> *rxSubjct = (RxEventsReturn<Return>*) command;

        ASSERT(keyValueSubject != nullptr);
        ASSERT(rxSubjct != nullptr);

        IList<Return> aggregateResult;
        Return previousResult;

        Status::ExecutionStatus status;
        bool criterionMet = true;

        try
        {
            Templates::Action0<Return> *lastAction = nullptr;

            for(Templates::Action0<Return> *action : actions)
            {
                lastAction = action;

                if(command->IsCancelled() || !criterionMet)
                    break;

                aggregateResult.push_back(previousResult);

//                if(action instanceof AggregateResultObserver)
//                {
//                    AggregateResultObserver<K,V> observer = (AggregateResultObserver<K,V>) action;
//                    observer.onAggregate(aggregateResult);
//                }

                previousResult = ExecutionStrategy::CallAction<Return>(command, action, status);

                rxSubjct->OnNext(previousResult);

                criterionMet = BaseLib::Strategy::IsCriterionMet::Perform(status, successCriterion);
            }

            if(lastAction != nullptr)
            {
                keyValueSubject->OnComplete(lastAction);
            }
        }
        catch (GeneralException throwable)
        {
            rxSubjct->OnError(throwable);
            return false;
        }

        if(!criterionMet)
        {
            rxSubjct->OnError(Exception("Success criterion not met while executing actions"));
        }
        else if(!command->IsCancelled())
        {
            rxSubjct->OnComplete();
        }

        return true;
    }

    template <typename Return>
    static bool IndividualComposition(Policy::Criterion successCriterion, IList<Templates::Action0<Return>*> &actions, Command<Return> *command)
    {
        KeyValueRxEvents<Templates::Action0<Return>*, Return> *keyValueSubject = (KeyValueRxEvents<Templates::Action0<Return>*, Return>*) command;
        RxEventsReturn<Return> *rxSubjct = (RxEventsReturn<Return>*) command;

        ASSERT(keyValueSubject != nullptr);
        ASSERT(rxSubjct != nullptr);

        Status::ExecutionStatus status;
        bool criterionMet = true;

        try
        {
            Templates::Action0<Return> *lastAction = nullptr;

            for(Templates::Action0<Return> *action : actions)
            {
                lastAction = action;

                if(command->IsCancelled() || !criterionMet)
                    break;

                Return value = ExecutionStrategy::CallAction<Return>(keyValueSubject, action, status);

                rxSubjct->OnNext(value);

                criterionMet = BaseLib::Strategy::IsCriterionMet::Perform(status, successCriterion);
            }

            if(lastAction != nullptr)
            {
                keyValueSubject->OnComplete(lastAction);
            }
        }
        catch (GeneralException throwable)
        {
            rxSubjct->OnError(throwable);
            return false;
        }

        if(!criterionMet)
        {
            rxSubjct->OnError(Exception("Success criterion not met while executing actions"));
        }
        else if(!command->IsCancelled())
        {
            rxSubjct->OnComplete();
        }

        return true;
    }

    template <typename Return>
    static bool AsyncComposition(Policy::Criterion successCriterion, IList<Templates::Action0<Concurrent::ScheduledFuture<Return>>*> &actions, Command<Concurrent::ScheduledFuture<Return>> *command)
    {
        Status::ExecutionStatus status;
        bool criterionMet = true;

        try
        {
            Templates::Action0<Concurrent::ScheduledFuture<Return>> *lastAction = nullptr;

            for(Templates::Action0<Concurrent::ScheduledFuture<Return>> *action : actions)
            {
                lastAction = action;

                if(command->IsCancelled() || !criterionMet)
                    break;

                Concurrent::ScheduledFuture<Return> value = ExecutionStrategy::CallAsyncAction<Return>(action, status);

                // TODO: call-back to command upon action completion
                //FutureFactory::Create(command, Command::OnNext, action, Return());
                //value.After()


                criterionMet = BaseLib::Strategy::IsCriterionMet::Perform(status, successCriterion);
            }
        }
        catch (GeneralException throwable)
        {
            return false;
        }

//        if(!criterionMet)
//        {
//        }
//        else if(!command->IsCancelled())
//        {
//        }
//
        return true;
    }

};

}
