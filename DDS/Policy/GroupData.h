/*
 * GroupData.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */
#ifndef DDS_Policy_GroupData_h_Included
#define DDS_Policy_GroupData_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * The purpose of this QoS is to allow the application to attach additional
 * information to the created Publisher or Subscriber.
 * The value of the GROUP_DATA is available to the application on the
 * DataReader and DataWriter entities and is propagated by means of the
 * built-in topics. This QoS can be used by an application combination with
 * the DataReaderListener and DataWriterListener to implement matching policies
 * similar to those of the PARTITION QoS except the decision can be made based
 * on an application-defined policy.
 */
class DLL_STATE GroupData : public QosPolicyBase
{
public:
    /**
     * Create a <code>GroupData<code> instance.
     */
    GroupData()	: value_()
    { }

    /**
     * Create a <code>GroupData<code> instance.
     *
     * @param seq the group data value
     */
    GroupData(const Core::ByteSeq& seq)
		: value_(seq)
    { }

	DEFINE_POLICY_TRAITS(GroupData, 19, DDS::Policy::RequestedOfferedKind::INDEPENDENT, true)

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
        NetworkLib::SocketWriterFunctions::WriteVectorUInt8(writer, value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
        NetworkLib::SocketReaderFunctions::ReadVectorUInt8(reader, value_);
	}

public:
    /**
     * Set the value for this <code>GroupData</code>
     *
     * @param seq the group data value
     */
    void SetValue(const Core::ByteSeq& seq)
    {
        value_ = seq;
    }
    /**
     * Get/Set the value for this <code>GroupData</code>
     *
     * @return  the group data value
     */
    Core::ByteSeq& GetValue()
    {
        return value_;
    }

    /**
     * Get the value for this <code>GroupData</code>
     *
     * @return  the group data value
     */
    const Core::ByteSeq& GetValue() const
    {
        return value_;
    }

private:
    Core::ByteSeq value_;
};

}} //  DDS::Policy

#endif // DDS_Policy_GroupData_h_Included

