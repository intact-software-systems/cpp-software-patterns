/*
 * SampleInfo.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#include "DCPS/Sub/SampleInfo.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief SampleInfo::SampleInfo
 */
SampleInfo::SampleInfo()
    : dataState_()
    , sourceTimestamp_()
    , instanceHandle_()
    , publicationHandle_()
    , generationCount_()
    , rank_()
    , validData_(false)
{

}

/**
 * @brief
 *
 * @param dataState
 * @param sourceTimestamp
 * @param instanceHandle
 * @param publicationHandle
 * @param generationCount
 * @param rank
 * @param validData
 */
SampleInfo::SampleInfo(const Status::DataState &dataState,
           const Timestamp &sourceTimestamp,
           const InstanceHandle &instanceHandle,
           const InstanceHandle &publicationHandle,
           const GenerationCount &generationCount,
           const Rank &rank,
           bool validData)
    : dataState_(dataState)
    , sourceTimestamp_(sourceTimestamp)
    , instanceHandle_(instanceHandle)
    , publicationHandle_(publicationHandle)
    , generationCount_(generationCount)
    , rank_(rank)
    , validData_(validData)
{

}

/**
 * @brief Empty destructor
 */
SampleInfo::~SampleInfo()
{

}

}} // namespace DCPS::Subscription
