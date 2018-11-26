/*
 * SampleInfo.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Subscription_Sample_h_IsIncluded
#define DCPS_Subscription_Sample_h_IsIncluded

#include"DCPS/Sub/SampleInfo.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
  @brief Wraps SampleInfo and DATA
  */
template <typename DATA>
class DLL_STATE Sample
{
public:
	Sample()
	{ }

	Sample(const DATA& d, const SampleInfo& i)
		: data_(d)
        , info_(i)
	{ }

	Sample(const Sample& other)
        : data_(other.GetData())
        , info_(other.info_)
	{ }

public:
	const DATA&         GetData() const { return data_; }
	const SampleInfo&   GetInfo() const { return info_; }

	void SetData(const DATA& d)         { data_ = d; }
	void SetInfo(const SampleInfo& i)   { info_ = i; }

private:
	DATA        data_;
	SampleInfo  info_;
};

template <typename DATA>
class DLL_STATE SampleSeq
{
public:
	SampleSeq() { }

	SampleSeq(const DATA &data, const SampleInfo &sampleInfo)
		: sampleSeq_(Sample<DATA>(data, sampleInfo))
	{ }

	SampleSeq(const SampleSeq &other)
		: sampleSeq_(other)
	{ }

	typedef std::vector< Sample<DATA> > SampleVector;

public:
	bool push_back(Sample<DATA> &sample)
	{
		return sampleSeq_.push_back(sample);
	}

	SampleVector& GetSamples() { return sampleSeq_; }

private:
	SampleVector sampleSeq_;
};

}}

#endif // DCPS_Subscription_Sample_h_IsIncluded
