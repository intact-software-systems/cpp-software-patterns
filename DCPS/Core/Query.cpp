/*
 * Query.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#include"DCPS/Core/Query.h"

namespace DCPS { namespace Core
{

Query::Query()
{ }

Query::Query(const DDS::Elements::FilterExpression &query_expression)
    : expression_(query_expression)
{ }

Query::Query(const DDS::Elements::FilterExpression &query_expression, const DDS::Elements::FilterExpressionParameters& params)
    : expression_(query_expression)
    , params_(params)
{ }

Query::~Query()
{ }

}}
