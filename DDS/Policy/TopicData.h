/*
 * TopicData.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_TopicData_h_Included
#define DDS_Policy_TopicData_h_Included

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
 * information to the created Topic such that when a remote application
 * discovers their existence it can examine the information and use it in
 * an application-defined way. In combination with the listeners on the
 * DataReader and DataWriter as well as by means of operations such as
 * ignore_topic, these QoS can assist an application to extend the provided QoS.
 */
class DLL_STATE TopicData : public QosPolicyBase
{
public:
    TopicData()
        : value_()
    { }
    TopicData(const Core::ByteSeq& seq)
        : value_(seq)
    { }
    virtual ~TopicData() {}

    DEFINE_POLICY_TRAITS(TopicData, 18, DDS::Policy::RequestedOfferedKind::INDEPENDENT, true)

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
    void SetValue(const Core::ByteSeq& seq) 	{ value_ = seq;}

    Core::ByteSeq& GetValue() 					{ return value_; }
    const Core::ByteSeq& GetValue() 	const 	{ return value_; }

private:
    Core::ByteSeq value_;
};

}}

#endif
