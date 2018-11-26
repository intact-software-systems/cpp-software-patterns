#include "StdWaitCondition.h"
#include "StdMutex.h"

namespace BaseLib {

StdWaitCondition::StdWaitCondition()
    : condition_(std::make_shared<std::condition_variable_any>())
{ }

StdWaitCondition::~StdWaitCondition()
{ }

bool StdWaitCondition::wait(StdMutex* mutex, int64 milliseconds)
{
    std::cv_status status = this->condition_->wait_for(mutex->mutex_.operator*(), std::chrono::milliseconds(milliseconds));
    return status == std::cv_status::no_timeout;
}

bool StdWaitCondition::wait(StdMutex* mutex, Duration duration)
{
    std::cv_status status = this->condition_->wait_for(mutex->mutex_.operator*(), duration);
    return status == std::cv_status::no_timeout;
}

bool StdWaitCondition::wait(StdMutex* mutex, Duration duration, std::function<bool()>&& function)
{
    return this->condition_->wait_for(mutex->mutex_.operator*(), duration, function);
}

bool StdWaitCondition::waitUntil(StdMutex* mutex, Timestamp timestamp)
{
    std::cv_status status = this->condition_->wait_until(mutex->mutex_.operator*(), timestamp);
    return status == std::cv_status::no_timeout;
}

bool StdWaitCondition::waitUntil(StdMutex* mutex, Timestamp timestamp, std::function<bool()>&& function)
{
    return this->condition_->wait_until(mutex->mutex_.operator*(), timestamp, function);
}

void StdWaitCondition::wakeAll()
{
    condition_->notify_all();
}

void StdWaitCondition::wakeOne()
{
    condition_->notify_one();
}

}