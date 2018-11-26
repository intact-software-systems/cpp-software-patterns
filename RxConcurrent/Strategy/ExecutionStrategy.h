#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/ThreadPool/ScheduledTasks.h"
#include"RxConcurrent/WaitSet.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Strategy {

struct DLL_STATE WaitSetExecutionStrategy
{
    static bool Perform(Concurrent::WaitSet& waitSet, Status::ExecutionStatus& status);
};

struct DLL_STATE TaskTriggerAlgorithm
{
    static int triggerTasks(std::list<Templates::ScheduledRunnableFuture::Ptr>& scheduledTasks);

    // TODO: Support for execution in "parallel"/"overlapping" policy
    static bool triggerTask(Templates::ScheduledRunnableFuture* future);
};

struct DLL_STATE TriggerTaskByInstance
{
    static std::list<Concurrent::ScheduledRunnableTask::Ptr> Perform(const std::list<Templates::RunnableFuture::Ptr>& tasks, Concurrent::ScheduledTasks& scheduledTasks);
};

struct DLL_STATE DefaultTaskTrigger
{
    static int Perform(Concurrent::ScheduledTasksBase* tasks);
};

struct DLL_STATE DefaultFindReady
{
    static Duration Perform(std::list<Templates::ScheduledRunnableFuture::Ptr>& allTasks, std::list<Templates::ScheduledRunnableFuture::Ptr>& scheduledTasks);
};

}}

