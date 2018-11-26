/*
 * TopicName.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */
#include "DDS/Elements/TopicName.h"

namespace DDS { namespace Elements
{

TopicName::TopicName(std::string name)
    : BaseLib::Templates::ComparableImmutableType<std::string>(name)
{}

TopicName::~TopicName()
{}

}} // namespace DDS::Elements
