/*
 * EntityFactory.h
 *
 *  Created on: Sep 5, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_EntityFactory_h_Included
#define DDS_Policy_EntityFactory_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * This policy controls the behavior of the Entity as a factory for other
 * entities. This policy concerns only DomainParticipant (as factory for
 * Publisher, Subscriber, and Topic), Publisher (as factory for DataWriter),
 * and Subscriber (as factory for DataReader). This policy is mutable.
 * A change in the policy affects only the entities created after the change;
 * not the previously created entities.
 * The setting of autoenable_created_entities to TRUE indicates that the
 * newly created object will be enabled by default.
 * A setting of FALSE indicates that the Entity will not be automatically
 * enabled. The application will need to enable it explicitly by means of the
 * enable operation (see Section 7.1.2.1.1.7, enable). The default setting
 * of autoenable_created_entities = TRUE means that, by default, it is not
 * necessary to explicitly call enable on newly created entities.
 */
class DLL_STATE EntityFactory : public QosPolicyBase
{
public:
    EntityFactory() : autoEnable_(true)
    { }

    EntityFactory(bool autoEnable)
		: autoEnable_(autoEnable)
    { }

	DEFINE_POLICY_TRAITS(EntityFactory, 15, DDS::Policy::RequestedOfferedKind::INDEPENDENT, true)

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt8(autoEnable_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
        autoEnable_ = reader->ReadInt8();
	}

public:
    void SetAutoEnable(bool on) 		{ autoEnable_ = on; }
	bool IsAutoEnable()			const { return autoEnable_; }

public:
	static EntityFactory AutoEnable()
	{
		static EntityFactory entityFactory(true);

		return entityFactory;
	}

	static EntityFactory ManuallyEnable()
	{
		static EntityFactory entityFactory(false);

		return entityFactory;
	}

private:
    bool autoEnable_;
};

}} //  DDS::Policy

#endif // DDS_Policy_EntityFactory_h_Included
