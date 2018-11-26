/*
 * TopicInstance.h
 *
 *  Created on: Oct 29, 2012
 *      Author: knuthelv
 */

#ifndef DCPS_Topic_TopicInstance_h_IsIncluded
#define DCPS_Topic_TopicInstance_h_IsIncluded

#include"DCPS/CommonDefines.h"

namespace DCPS { namespace Topic
{

/**
 * @brief Convenience class to group an InstanceHandle with Topic DATA
 */
template <typename DATA>
class TopicInstance
{
public:
    TopicInstance()
        : handle_()
        , sample_()
    { }

    TopicInstance(const InstanceHandle& handle)
        : handle_(handle)
        , sample_()
    { }

    TopicInstance(const InstanceHandle& handle, const DATA& sample)
        : handle_(handle)
        , sample_(sample)
    { }

public:
    const InstanceHandle& GetHandle() const { return handle_; }

    const DATA& GetSample() const 	{ return sample_; }
    DATA& 		GetSample() 		{ return sample_; }

    void SetHandle(const InstanceHandle& handle) 	{ handle_ = handle; }
    void SetSample(const DATA& sample)              { sample_ = sample; }

private:
    InstanceHandle handle_;
    DATA sample_;
};

}} // DCPS::Topic

#endif // DCPS_Topic_TopicInstance_h_IsIncluded
