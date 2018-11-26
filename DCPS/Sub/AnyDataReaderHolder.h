/*
 * DataReader.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Subscription_AnyDataReaderHolder_h_IsIncluded
#define DCPS_Subscription_AnyDataReaderHolder_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Sub/DataReader.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief The AnyDataReaderHolderBase class
 */
class DLL_STATE AnyDataReaderHolderBase
{
public:
    virtual ~AnyDataReaderHolderBase() { }

    CLASS_TRAITS(AnyDataReaderHolderBase)

    virtual const DDS::DataReaderQoS GetQoS() const = 0;

    virtual void SetQoS(const DDS::DataReaderQoS& qos) = 0;

    virtual const DDS::Elements::TopicName& 	GetTopicName() const = 0;
    virtual const DDS::Elements::TypeName&      GetTypeName() const = 0;

    virtual DCPS::Subscription::SubscriberPtr Parent() const = 0;

    virtual void Close() = 0;

    // TODO: Add other non type-dependent methods.
};

/**
 * @brief The AnyDataReaderHolder class
 */
template <typename DATA>
class AnyDataReaderHolder : public AnyDataReaderHolderBase
{
public:
    AnyDataReaderHolder(const typename DCPS::Subscription::DataReader<DATA>::Ptr &dr)
        : dr_(dr)
    { }
    virtual ~AnyDataReaderHolder()
    { }

    CLASS_TRAITS(AnyDataReaderHolder)

public:
    virtual const DDS::DataReaderQoS GetQoS() const
    {
        return dr_->config().GetQoS();
    }

    virtual void SetQoS(const DDS::DataReaderQoS &the_qos)
    {
        dr_->config().SetQoS(the_qos);
    }

    virtual const DDS::Elements::TopicName& GetTopicName() const
    {
        return dr_->GetTopic()->GetTopicName();
    }

    virtual const DDS::Elements::TypeName& GetTypeName() const
    {
        return dr_->GetTopic()->GetTypeName();
    }

    virtual DCPS::Subscription::SubscriberPtr Parent() const
    {
        return dr_->Parent();
    }

    virtual void Close()
    {
        dr_->Close();
    }

    typename DCPS::Subscription::DataReader<DATA>::Ptr get() const
    {
       return dr_;
    }

private:
    typename DCPS::Subscription::DataReader<DATA>::Ptr dr_;
};

}}

#endif
