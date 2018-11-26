#pragma once

#include"BaseLib/Policy/MaxLimit.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Collection {

class DLL_STATE RingBufferQueuePolicy
{
public:
    RingBufferQueuePolicy();
    RingBufferQueuePolicy(const Policy::MaxLimit<int>& capacity, bool overwrite);
    RingBufferQueuePolicy(const RingBufferQueuePolicy& policy);
    virtual ~RingBufferQueuePolicy();

    int Limit() const;

    bool IsOverwrite() const;

    static RingBufferQueuePolicy Overwrite(int capacity);
    static RingBufferQueuePolicy LimitedTo(int capacity);

private:
    Policy::MaxLimit<int> capacity_;
    bool                  overwrite_;
};

}}
