/*
 * TypeName.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */
#include "DDS/Elements/TypeName.h"

namespace DDS { namespace Elements
{

TypeName::TypeName(std::string name)
    : BaseLib::Templates::ComparableImmutableType<std::string>(name)
{}

TypeName::~TypeName()
{}

}} // namespace DDS::Elements
