/*
 * RtpsEntity.cpp
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#include "RTPS/Structure/RtpsEntity.h"

namespace RTPS
{

RtpsEntity::RtpsEntity()
{
}

RtpsEntity::RtpsEntity(const GUID &guid)
	: guid_(guid)
{

}

RtpsEntity::~RtpsEntity()
{
}

}
