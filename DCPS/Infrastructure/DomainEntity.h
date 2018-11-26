/*
 * DomainEntity.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Infrastructure_DomainEntity_h_IsIncluded
#define DCPS_Infrastructure_DomainEntity_h_IsIncluded

#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Infrastructure
{

/**
 * @brief
 *
 * DomainEntity is the abstract base class for all DCPS entities, except for the DomainParticipant. Its sole purpose is to
 * express that DomainParticipant is a special kind of Entity, which acts as a container of all other Entity, but itself cannot
 * contain other DomainParticipant.
 */
class DLL_STATE DomainEntity : public Entity
{
public:
	DomainEntity(bool enabled = true);
	DomainEntity(const Status::StatusMask &mask, bool enabled = true);

	virtual ~DomainEntity();
};

}}

#endif

