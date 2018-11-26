/*
 * FilterExpression.h
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Elements_FilterExpression_h_IsIncluded
#define DDS_Elements_FilterExpression_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/Export.h"

namespace DDS { namespace Elements
{
/* ---------------------------------------------------------------------------
The filter_expression attribute is a string that specifies the criteria to select
the data samples of interest. It is similar to the WHERE part of an SQL clause.
--------------------------------------------------------------------------- */
class DLL_STATE FilterExpression : public NetworkLib::NetObjectBase
{
public:
    FilterExpression(const std::string &filter);
    FilterExpression();
    virtual ~FilterExpression();

public:
    virtual void Write(SerializeWriter *writer) const
    {
        writer->WriteString(filterExpression_);
    }

    virtual void Read(SerializeReader *reader)
    {
        filterExpression_  = reader->ReadString();
    }

public:
    bool operator==(const FilterExpression &other) const
    {
        return (filterExpression_ == other.filterExpression_);
    }

    bool operator!=(const FilterExpression &other) const
    {
        return !(filterExpression_ == other.filterExpression_);
    }

public:
    void SetFilterExpression(const std::string &filter) 	{ filterExpression_ = filter; }

    std::string GetFilterExpression()	const { return filterExpression_; }

private:
    std::string filterExpression_;
};

}} // DDS::Elements

#endif
