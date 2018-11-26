/*
 * FilterExpressionParameters.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#include"DDS/Elements/FilterExpressionParameters.h"

namespace DDS { namespace Elements
{

FilterExpressionParameters::FilterExpressionParameters(const std::vector<std::string> &params)
	: params_(params)
{ }

FilterExpressionParameters::FilterExpressionParameters()
{ }

FilterExpressionParameters::~FilterExpressionParameters()
{ }

}} // DDS::Elements
