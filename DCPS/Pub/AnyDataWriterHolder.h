/*
 * DataWriter.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#ifndef DCPS_Publication_AnyDataWriterHolder_h_IsIncluded
#define DCPS_Publication_AnyDataWriterHolder_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Pub/DataWriter.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Publication
{

/**
 * @brief The AnyDataWriterHolderBase class
 *
 * TODO: Add other non type-dependent methods.
 */
class DLL_STATE AnyDataWriterHolderBase
{
public:
    virtual ~AnyDataWriterHolderBase() { }

    CLASS_TRAITS(AnyDataWriterHolderBase)

    virtual const DDS::DataWriterQoS GetQoS() const = 0;

    virtual void SetQoS(const DDS::DataWriterQoS& qos) = 0;


    virtual const DDS::Elements::TopicName&     GetTopicName() const = 0;
    virtual const DDS::Elements::TypeName&      GetTypeName() const = 0;


    virtual DCPS::Publication::PublisherPtr     Parent() const = 0;

    virtual void Close() = 0;
};

/**
 * @brief The AnyDataWriterHolder class
 */
template <typename DATA>
class AnyDataWriterHolder : public AnyDataWriterHolderBase
{
public:
    AnyDataWriterHolder(const typename DCPS::Publication::DataWriter<DATA>::Ptr &dr)
        : dr_(dr)
    { }
    virtual ~AnyDataWriterHolder()
    { }

    CLASS_TRAITS(AnyDataWriterHolder)

public:
    virtual const DDS::DataWriterQoS GetQoS() const
    {
        return dr_->config().GetQoS();
    }

    virtual void SetQoS(const DDS::DataWriterQoS &the_qos)
    {
        dr_->config().SetQoS(the_qos);
    }

    virtual const DDS::Elements::TopicName& GetTopicName() const
    {
        return dr_->state().GetTopic()->GetTopicName();
    }

    virtual const DDS::Elements::TypeName& GetTypeName() const
    {
        return dr_->state().GetTopic()->GetTypeName();
    }

    virtual DCPS::Publication::PublisherPtr Parent() const
    {
        return dr_->state().Parent();
    }

    virtual void Close()
    {
        dr_->Close();
    }

    typename DCPS::Publication::DataWriter<DATA>::Ptr get() const
    {
        return dr_;
    }

private:
    typename DCPS::Publication::DataWriter<DATA>::Ptr dr_;
};

}}

#endif

