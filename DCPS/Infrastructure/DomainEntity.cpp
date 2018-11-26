/*
 * DomainEntity.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#include "DCPS/Infrastructure/DomainEntity.h"

namespace DCPS { namespace Infrastructure
{

DomainEntity::DomainEntity(bool enabled)
	: Entity(enabled)
{ }

DomainEntity::DomainEntity(const Status::StatusMask &mask, bool enabled)
	: Entity(mask, enabled)
{ }

DomainEntity::~DomainEntity()
{ }

}}


