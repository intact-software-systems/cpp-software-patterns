/*
 * Query.h
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#ifndef DCPS_Core_Query_h_IsIncluded
#define DCPS_Core_Query_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Core
{

/**
 * @brief The Query class
 */
class DLL_STATE Query
{
public:
    typedef std::vector<std::string>::iterator 			iterator;
    typedef std::vector<std::string>::const_iterator 	const_iterator;

public:
    Query();
    Query(const DDS::Elements::FilterExpression &queryExpression);
    Query(const DDS::Elements::FilterExpression &queryExpression, const DDS::Elements::FilterExpressionParameters& params);

    template <typename FWIterator>
    Query(const DDS::Elements::FilterExpression &queryExpression, const FWIterator& begin, const FWIterator& end)
        : expression_(queryExpression)
        , params_(begin, end)
    { }

    virtual ~Query();

public:
    void		AddParameter(const std::string &param) 			{ params_.AddParameter(param); }
    void 		ClearParameters()								{ params_.Clear(); }
    std::size_t	GetLength()								const 	{ return params_.GetLength(); }

public:
    iterator 	End()		{ return params_.End(); }
    iterator 	Begin() 	{ return params_.Begin(); }

    template <typename FWIterator>
    void SetParameters(const FWIterator& begin, const FWIterator end)
    {
        params_.SetParameters(begin, end);
    }

public:
    const DDS::Elements::FilterExpression&	 			GetFilterExpression()            const { return expression_; }
    const DDS::Elements::FilterExpressionParameters& 	GetFilterExpressionParameters()  const { return params_; }

    void    SetFilterExpression(const DDS::Elements::FilterExpression &filterExpression)             { expression_ = filterExpression; }
    void    SetFilterExpressionParameters(const DDS::Elements::FilterExpressionParameters &params)   { params_ = params; }

private:
    DDS::Elements::FilterExpression 			expression_;
    DDS::Elements::FilterExpressionParameters 	params_;
};

}} // DCPS::Core

#endif // DCPS_Core_Query_h_IsIncluded
