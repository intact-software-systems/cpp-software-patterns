/*
 * GUID.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_GUID_h_IsIncluded
#define RTPS_Elements_GUID_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/GuidPrefix.h"
#include"RTPS/Elements/EntityId.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type GUID_t
	The PSM maps the GUID_t to the following structure:

	struct GUID_t
	{
		GuidPrefix_t guidPrefix;
		EntityId_t entityId;
	};

	typedef GUID_t GUID;

The reserved constant GUID_UNKNOWN defined by the PIM is mapped to:
#define GUID_UNKNOWN { GUIDPREFIX_UNKNOWN, ENTITYID_UNKNOWN }
---------------------------------------------------------------------------------------------*/

/**
 * @brief The GUID (Globally Unique Identifier) is an attribute of all RTPS Entities and uniquely identifies
 * the Entity within a DDS Domain.
 *
 * The GUID is built as a tuple <prefix, entityId> combining a GuidPrefix_t prefix and an EntityId_t entityId.
 *
 * GuidPrefix - Uniquely identifies the Participant within the Domain.
 * EntityId - Uniquely identifies the Entity within the Participant
 */
class DLL_STATE GUID : public NetworkLib::NetObjectBase
{
public:
	GUID(const GuidPrefix &prefix, const EntityId &entity)
		: guidPrefix_(prefix)
		, entityId_(entity)
	{}
	GUID()
		: guidPrefix_()
		, entityId_()
	{}
	virtual ~GUID()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		guidPrefix_.Write(writer);
		entityId_.Write(writer);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		guidPrefix_.Read(reader);
		entityId_.Read(reader);
	}

public:
	static GUID GUID_UNKNOWN()
	{
		static GUID id(GuidPrefix::UNKNOWN(), EntityId::UNKNOWN());
		return id;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = GuidPrefix::SIZE() + EntityId::SIZE();
		return sz;
	}

public:
	inline void 		SetGuidPrefix(const GuidPrefix &prefix)		{ guidPrefix_ = prefix; }
	inline void 		SetEntityId(const EntityId &id)				{ entityId_ = id; }

	inline GuidPrefix 	GetGuidPrefix()	const 	{ return guidPrefix_; }
	inline EntityId		GetEntityId()	const	{ return entityId_; }

public:
	bool operator==(const GUID &other) const
	{
		return (this->guidPrefix_ == other.guidPrefix_) && (this->entityId_ == other.entityId_);
	}

	bool operator<(const GUID &other) const
	{
		return (this->guidPrefix_ < other.guidPrefix_) && (this->entityId_ < other.entityId_);
	}

private:
	GuidPrefix 	guidPrefix_;
	EntityId 	entityId_;
};

} // namespace RTPS

#endif // RTPS_Elements_GUID_h_IsIncluded
