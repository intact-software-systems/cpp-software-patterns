/*
 * DomainId.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */

#include "DDS/Elements/DomainId.h"

namespace DDS { namespace Elements
{

DomainId::DomainId(DomainIdType domainId) : BaseLib::Templates::ComparableMutableType<DomainIdType>(domainId)
{}

DomainId::~DomainId()
{}

}} // namespace NetworkLib::DDS::Elements
