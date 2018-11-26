/*
 * Lifespan.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Lifespan_h_Included
#define DDS_Policy_Lifespan_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * The purpose of this QoS is to avoid delivering stale data to the
 * application. Each data sample written by the DataWriter has an associated
 * expiration time beyond which the data should not be delivered to any
 * application. Once the sample expires, the data will be removed from the
 * DataReader caches as well as from the transient and persistent
 * information caches. The expiration time of each sample is computed by
 * adding the duration specified by the LIFESPAN QoS to the source timestamp.
 * As described in Section 7.1.2.4.2.11, write and Section 7.1.2.4.2.12,
 * write_w_timestamp the source timestamp is either automatically computed by
 * the Service each time the DataWriter write operation is called, or else
 * supplied by the application by means of the write_w_timestamp operation.
 *
 * This QoS relies on the sender and receiving applications having their clocks
 * sufficiently synchronized. If this is not the case and the Service can
 * detect it, the DataReader is allowed to use the reception timestamp instead
 * of the source timestamp in its computation of the expiration time.
 */
class DLL_STATE Lifespan : public QosPolicyBase
{
public:
    Lifespan(const BaseLib::Duration& d)
		: duration_(d)
    { }
    Lifespan()
		: duration_(BaseLib::Duration::Infinite())
    { }

	DEFINE_POLICY_TRAITS(Lifespan, 21, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
	virtual void Write(NetworkLib::SerializeWriter *) const
	{
		//duration_.Write(writer);
	}

	virtual void Read(NetworkLib::SerializeReader *)
	{
		//duration_.Read(reader);
	}

public:
    void SetDuration(const BaseLib::Duration& d) 		{ duration_ = d; }

    const BaseLib::Duration GetDuration() 		const 	{ return duration_; }
    BaseLib::Duration& GetDuration() 					{ return duration_; }

private:
    BaseLib::Duration duration_;
};

}} //  DDS::Policy

#endif // DDS_Policy_Lifespan_h_Included

