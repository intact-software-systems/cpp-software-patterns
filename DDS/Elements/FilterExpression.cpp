/*
 * FilterExpression.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#include "DDS/Elements/FilterExpression.h"

namespace DDS { namespace Elements
{

FilterExpression::FilterExpression(const std::string &filter)
    : filterExpression_(filter)
{

}

FilterExpression::FilterExpression()
{ }

FilterExpression::~FilterExpression()
{ }

}} // DDS::Elements
