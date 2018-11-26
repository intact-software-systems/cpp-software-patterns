/*
 * History.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */
#ifndef DDS_Policy_History_h_Included
#define DDS_Policy_History_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @brief
 *
 * 1. This policy controls the behavior of the Service when the value of an instance changes before it is finally
 * communicated to some of its existing DataReader entities.
 *
 * 2. If the kind is set to KEEP_LAST, then the Service will only attempt to keep the latest values of the instance and
 * discard the older ones. In this case, the value of depth regulates the maximum number of values (up to and
 * including the most current one) the Service will maintain and deliver. The default (and most common setting) for
 * depth is one, indicating that only the most recent value should be delivered.
 *
 * 3. If the kind is set to KEEP_ALL, then the Service will attempt to maintain and deliver all the values of the instance
 * to existing subscribers. The resources that the Service can use to keep this history are limited by the settings of the
 * RESOURCE_LIMITS QoS. If the limit is reached, then the behavior of the Service will depend on the
 * RELIABILITY QoS. If the reliability kind is BEST_EFFORT, then the old values will be discarded. If reliability
 * is RELIABLE, then the Service will block the DataWriter until it can deliver the necessary old values to all
 * subscribers.
 *
 * The setting of HISTORY depth must be consistent with the RESOURCE_LIMITS max_samples_per_instance. For these
 * two QoS to be consistent, they must verify that depth <= max_samples_per_instance.
 */
class DLL_STATE History : public QosPolicyBase
{
public:
    History()
        : kind_(Policy::HistoryKind::KEEP_LAST)
        , depth_(1)
    { }

    History(Policy::HistoryKind::Type the_kind, int32_t the_depth)
        : kind_(the_kind)
        , depth_(the_depth)
    { }

    DEFINE_POLICY_TRAITS(History, 13, DDS::Policy::RequestedOfferedKind::INDEPENDENT, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32((int32_t) kind_);
        writer->WriteInt32(depth_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::HistoryKind::Type) reader->ReadInt32();
        depth_ = reader->ReadInt32();
    }

public:
    Policy::HistoryKind::Type GetKind() 	const 	{ return kind_; }
    Policy::HistoryKind::Type& GetKind() 			{ return kind_; }

    int32_t 	GetDepth() 					const 	{ return depth_; }
    int32_t& 	GetDepth() 							{ return depth_; }

    void SetKind(Policy::HistoryKind::Type the_kind) 	{ kind_ = the_kind; }
    void SetDepth(int32_t the_depth) 					{ depth_ = the_depth; }

public:
    static History KeepAll()
    {
        return History(Policy::HistoryKind::KEEP_ALL, std::numeric_limits<int32_t>::max());
    }

    static History KeepLast(int32_t depth)
    {
        return History(Policy::HistoryKind::KEEP_LAST, depth);
    }

private:
    Policy::HistoryKind::Type kind_;
    int32_t depth_;
};

}} //  DDS::Policy

#endif // DDS_Policy_GroupData_h_Included

