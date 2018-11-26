/*
 * ContentFilteredTopic.h
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */
#ifndef DCPS_Topic_ContentFilteredTopic_h_IsIncluded
#define DCPS_Topic_ContentFilteredTopic_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Topic/TopicDescription.h"
#include"DCPS/Topic/Topic.h"
#include"DCPS/Core/IncludeLibs.h"

namespace DCPS { namespace Topic
{

/**
 * @brief
 *
 * ContentFilteredTopic describes a more sophisticated subscription that indicates the subscriber does not want to
 * necessarily see all values of each instance published under the Topic. Rather, it wants to see only the values whose
 * contents satisfy certain criteria. This class therefore can be used to request content-based subscriptions.
 * The selection of the content is done using the filter_expression with parameters expression_parameters.
 *
 * - The filter_expression attribute is a string that specifies the criteria to select the data samples of interest. It is similar to
 *    the WHERE part of an SQL clause.
 * - The expression_parameters attribute is a sequence of strings that give values to the ‘parameters’ (i.e., "%n" tokens) in
 * 	the filter_expression. The number of supplied parameters must fit with the requested values in the filter_expression
 *    (i.e., the number of %n tokens).
 * Annex A describes the syntax of filter_expression and expression_parameters.
 */
template <typename DATA>
class ContentFilteredTopic : public DCPS::Topic::TopicDescription<DATA>
{
public:
    ContentFilteredTopic(const typename DCPS::Topic::Topic<DATA>::Ptr &topic,  const Core::Query &query)
        : DCPS::Topic::TopicDescription<DATA>(topic->GetParticipant(), topic->GetTopicName(), topic->GetTypeName())
        , topic_(topic)
        , query_(query)
    { }

    virtual ~ContentFilteredTopic()
    { }

    CLASS_TRAITS(ContentFilteredTopic)

    /**
     * @brief
     *
     * This operation returns the expression_parameters associated with the ContentFilteredTopic. That is, the parameters
     * specified on the last successful call to set_expression_parameters, or if set_expression_parameters was never called, the
     * parameters specified when the ContentFilteredTopic was created.
     */
    const DDS::Elements::FilterExpressionParameters& GetFilterParamters() const
    {
        return query_.GetFilterExpressionParameters();
    }

    /**
     * @brief
     *
     * This operation changes the expression_parameters associated with the ContentFilteredTopic.
     */
    void SetFilterParameters(DDS::Elements::FilterExpressionParameters &filter)
    {
        query_.SetFilterExpressionParameters(filter);
    }

    /**
     * @brief Updates the filter parameters for this content filtered topic.
     */
    template <typename FWIterator>
    void SetFilterParameters(const FWIterator& begin, const FWIterator& end)
    {
        query_.SetParameters(begin, end);
    }

    /**
     * @brief
     *
     * The filter_expression associated with the ContentFilteredTopic. That is, the expression specified when the
     * ContentFilteredTopic was created.
     */
    const Core::Query&  GetQuery() const
    {
        return query_;
    }

    /**
     * @brief
     *
     * This operation returns the Topic associated with the ContentFilteredTopic. That is, the Topic specified when the
     * ContentFilteredTopic was created.
     */
    const typename Topic<DATA>::Ptr& GetRelatedTopic()  const
    {
        return topic_;
    }

private:
    typename Topic<DATA>::Ptr    topic_;
    Core::Query                  query_;
};

}} // DCPS::Topic

#endif // DCPS_Topic_ContentFilteredTopic_h_IsIncluded
