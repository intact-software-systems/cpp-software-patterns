#ifndef RxData_Cache_QueryCriterion_h_IsIncluded
#define RxData_Cache_QueryCriterion_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/selection/SelectionCriterion.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * Specialization of SelectionCriterion that performs a filter based on a query expression.
 * 
 * The query is made of an expression and of parameters that may parameterize the expression 
 * (the number of parameters must fit with the values required by the expression). 
 * See Annex B for the syntax of an expression and its parameters.
 * 
 * It offers methods to:
 * 
 * - Set the value of the expression and its parameters (set_query); a TRUE return value 
 * indicates that they have been successfully changed.
 * 
 * - Set the values of the parameters (set_parameters). The number of parameters must fit with 
 * the values required by the expression. A TRUE return value indicates that they have been 
 * successfully changed.
 *
 * After a successful call to one of those methods the owning Selection is refreshed if its 
 * auto_refresh is TRUE.
 * 
 * @author KVik
 * 
 * @param <DATA>
 */
template <typename DATA>
class QueryCriterion : public SelectionCriterion
{
private:
	// ----------------------------------
	// Attributes
	// ----------------------------------
	
	std::string             expression_;
	std::list<std::string>	parameters_;

public:
	// ----------------------------------
	// Constructor
	// ----------------------------------
	
	/**
	 * 
	 * @param expression
	 * @param parameters
	 */
	QueryCriterion(std::string expression, std::list<std::string> parameters)
        : SelectionCriterion(SelectionCriteria::QUERY_CRITERION)
        , expression_(expression)
        , parameters_(parameters)
	{ }
	
    CLASS_TRAITS(QueryCriterion)

	// ----------------------------------
	// Operations
	// ----------------------------------
	
	/**
	 * Set the value of the expression and its parameters. 
	 * 
	 * @param expression
	 * @param parameters
	 * 
	 * @return  TRUE indicates that they have been successfully changed.
	 */
	bool setQuery(std::string expression, std::list<std::string> parameters)
	{
		this->expression_ = expression;
		this->parameters_ = parameters;
		
		return true;
	}
	
	/**
	 * Set the values of the parameters. The number of parameters must fit with the 
	 * values required by the expression. 
	 * 
	 * @param parameters
	 * @return TRUE indicates that they have been successfully changed.
	 */	
	bool setParameters(std::list<std::string> parameters)
	{
		this->parameters_ = parameters;
		
		return true;
	}
	
	// ----------------------------------------
	// Getters of private attributes
	// ----------------------------------------

	std::string getExpression() const
	{
		return expression_;
	}

	std::list<std::string> getParameters() const
	{
		return parameters_;
	}
};

}

#endif
