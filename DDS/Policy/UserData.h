#ifndef DDS_Policy_UserData_h_Included
#define DDS_Policy_UserData_h_Included

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
 * information to the created Entity objects such that when a remote application
 * discovers their existence it can access that information and use it for its
 * own purposes. One possible use of this QoS is to attach security credentials
 * or some other information that can be used by the remote application to
 * authenticate the source. In combination with operations such as
 * ignore_participant, ignore_publication, ignore_subscription,
 * and ignore_topic these QoS can assist an application to define and enforce
 * its own security policies. The use of this QoS is not limited to security,
 * rather it offers a simple, yet flexible extensibility mechanism.
 */
class DLL_STATE UserData : public QosPolicyBase
{
public:
	/**
	* Create a <code>UserData</code> instance with an empty user data.
	*/
	UserData()
		: value_()
	{ }

	/**
	  * Create a <code>UserData</code> instance.
	  *
	  * @param seq the sequence of octet representing the user data
	  */
	UserData(const Core::ByteSeq& seq)
		: value_(seq)
	{ }

    virtual ~UserData()
    {}

	DEFINE_POLICY_TRAITS(UserData, 1, DDS::Policy::RequestedOfferedKind::INDEPENDENT, true)

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
     * @brief Empty
     * @return
     */
    static UserData Empty()
    {
        return UserData();
    }

public:
	/**
	  * Set the value for the user data.
	  *
	  * @param seq a sequence of octet representing the user data.
	  */
	void SetValue(const Core::ByteSeq& seq) { value_ = seq; }

	/**
	  * Get/Set the user data.
	  *
	  * @return the sequence of octet representing the user data
	  */
	Core::ByteSeq& GetValue() 				{ return value_; }

	/**
	  * Get the user data.
	  *
	  * @return the sequence of octet representing the user data
	  */
	const Core::ByteSeq& GetValue() const { return value_; }

private:
	Core::ByteSeq value_;
};

}} //  DDS::Policy

#endif // DDS_Policy_UserData_h_Included
