/*
 * FilterExpressionParameters.h
 *
 *  Created on: Sep 19, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Elements_FilterExpressionParameters_h_IsIncluded
#define DDS_Elements_FilterExpressionParameters_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/CommonDefines.h"
#include"DDS/Export.h"

namespace DDS { namespace Elements
{

/* ----------------------------------------------------------------
 Used in Core::Query

 TODO: Describe use
 ---------------------------------------------------------------- */
class DLL_STATE FilterExpressionParameters : public NetworkLib::NetObjectBase
{
public:
    typedef std::vector<std::string>::iterator 			iterator;
    typedef std::vector<std::string>::const_iterator 	const_iterator;

public:
    FilterExpressionParameters(const std::vector<std::string> &params_);

    template <typename FWIterator>
    FilterExpressionParameters(const FWIterator& begin, const FWIterator& end)
    {
        std::copy(begin, end, std::back_insert_iterator<std::vector<std::string> >(params_));
    }

    FilterExpressionParameters();
    virtual ~FilterExpressionParameters();

public:
    virtual void Write(SerializeWriter *writer) const
    {
        NetworkLib::SocketWriterFunctions::WriteVectorString(writer, params_);
    }

    virtual void Read(SerializeReader *reader)
    {
        NetworkLib::SocketReaderFunctions::ReadVectorString(reader, params_);
    }

public:
    void AddParameter(const std::string &param)
    {
        params_.push_back(param);
    }

    void Clear()
    {
        params_.erase(params_.begin(), params_.end());
    }

    std::size_t	GetLength()	const
    {
        return params_.size();
    }

public:
    iterator 	End()		{ return params_.end(); }
    iterator 	Begin() 	{ return params_.begin(); }

    template <typename FWIterator>
    void SetParameters(const FWIterator& begin, const FWIterator end)
    {
        Clear();

        std::copy(begin, end, std::back_insert_iterator<std::vector<std::string> >(params_));
    }

private:
    std::vector<std::string> params_;
};

}}

#endif
