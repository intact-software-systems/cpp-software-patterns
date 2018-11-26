/*
 * Deadline.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Deadline_h_Included
#define DDS_Policy_Deadline_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * This policy is useful for cases where a Topic is expected to have each
 * instance updated periodically. On the publishing side this setting
 * establishes a contract that the application must meet. On the subscribing
 * side the setting establishes a minimum requirement for the remote publishers
 * that are expected to supply the data values. When the Service matches a
 * DataWriter and a DataReader it checks whether the settings are compatible
 * (i.e., offered deadline period<= requested deadline period) if they are not,
 * the two entities are informed (via the listener or condition mechanism)
 * of the incompatibility of the QoS settings and communication will not occur.
 * Assuming that the reader and writer ends have compatible settings, the
 * fulfillment of this contract is monitored by the Service and the application
 * is informed of any violations by means of the proper listener or condition.
 * The value offered is considered compatible with the value requested if and
 * only if the inequality offered deadline period <= requested deadline period
 * evaluates to TRUE. The setting of the DEADLINE policy must be set
 * consistently with that of the TIME_BASED_FILTER.
 * For these two policies to be consistent the settings must be such that
 * deadline period>= minimum_separation.
 */
class DLL_STATE Deadline : public QosPolicyBase
{
public:
    Deadline(const BaseLib::Duration& d)
        : period_(d)
    { }

    Deadline()
        : period_(BaseLib::Duration::Infinite())
    { }

    DEFINE_POLICY_TRAITS(Deadline, 4, DDS::Policy::RequestedOfferedKind::COMPATIBLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *) const
    {
        // TODO: Write Duration
    }

    virtual void Read(NetworkLib::SerializeReader *)
    {
        // TODO: Read Duration
    }

public:
    void SetPeriod(const BaseLib::Duration& d) 		{ period_ = d; }
    const BaseLib::Duration GetPeriod() 		const 	{ return period_; }

private:
    BaseLib::Duration period_;
};

}}

#endif
