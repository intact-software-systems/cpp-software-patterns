/*
 * Partition.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Partition_h_Included
#define DDS_Policy_Partition_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This policy allows the introduction of a logical partition concept inside the 'physical' partition induced by a domain.
 *
 * For a DataReader to see the changes made to an instance by a DataWriter, not only the Topic must match, but also they
 * must share a common partition. Each string in the list that defines this QoS policy defines a partition name. A partition
 * name may contain wildcards. Sharing a common partition means that one of the partition names matches.
 * Failure to match partitions is not considered an "incompatible" QoS and does not trigger any listeners nor conditions.
 *
 * This policy is changeable. A change of this policy can potentially modify the "match" of existing DataReader and
 * DataWriter entities. It may establish new "matches" that did not exist before, or break existing matches.
 * PARTITION names can be regular expressions and include wildcards as defined by the POSIX fnmatch API (1003.2-1992
 * section B.6). Either Publisher or Subscriber may include regular expressions in partition names, but no two names that
 * both contain wildcards will ever be considered to match. This means that although regular expressions may be used both
 * at publisher as well as subscriber side, the service will not try to match two regular expressions (between publishers and
 * subscribers).
 *
 * Partitions are different from creating Entity objects in different domains in several ways. First, entities belonging to
 * different domains are completely isolated from each other; there is no traffic, meta-traffic or any other way for an
 * application or the Service itself to see entities in a domain it does not belong to. Second, an Entity can only belong to one
 * domain whereas an Entity can be in multiple partitions. Finally, as far as the DDS Service is concerned, each unique data
 * instance is identified by the tuple (domainId, Topic, key). Therefore two Entity objects in different domains cannot refer
 * to the same data instance. On the other hand, the same data-instance can be made available (published) or requested
 * (subscribed) on one or more partitions.
 */
class DLL_STATE Partition : public QosPolicyBase
{
public:
    Partition(const std::string& partition)
		: name_()
    {
        name_.push_back(partition);
    }

    Partition(const Core::StringSeq& partitions)
		: name_(partitions)
    { }

    Partition() //: name_()
    {
//        name_.push_back("");
    }

	DEFINE_POLICY_TRAITS(Partition, 10, DDS::Policy::RequestedOfferedKind::INDEPENDENT, true)

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
        NetworkLib::SocketWriterFunctions::WriteVectorString(writer, name_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
        NetworkLib::SocketReaderFunctions::ReadVectorString(reader, name_);
	}

public:
    void SetName(const Core::StringSeq& partitions) { name_ = partitions; }

    const Core::StringSeq& 	GetName() 	const 		{ return name_; }
    Core::StringSeq& 		GetName() 				{ return name_; }

private:
    Core::StringSeq name_;
};

}} //  DDS::Policy

#endif // DDS_Policy_Partition_h_Included
