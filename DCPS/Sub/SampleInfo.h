/*
 * SampleInfo.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#pragma once

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief
 *
 * For each instance the middleware internally maintains two counts: the disposed_generation_count and no_writers_generation_count, relative to each DataReader:
 * - The disposed_generation_count and no_writers_generation_count are initialized to zero when the DataReader first detects the presence of a never-seen-before
 *   instance.
 * - The disposed_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_DISPOSED to ALIVE.
 * - The no_writers_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_NO_WRITERS to ALIVE.
 *
 * The disposed_generation_count and no_writers_generation_count available in the SampleInfo capture a snapshot of the corresponding
 * counters at the time the sample was received.
 */
class DLL_STATE GenerationCount
{
public:
    GenerationCount(int32_t disposed, int32_t noWriters)
        : disposed_(disposed), noWriters_(noWriters)
    { }

    GenerationCount()
        : disposed_(0)
        , noWriters_(0)
    { }

    ~GenerationCount()
    { }

    inline int32_t GetDisposed()    const { return disposed_; }
    inline int32_t GetNoWriters()   const { return noWriters_; }

    inline int32_t& GetDisposed()    { return disposed_; }
    inline int32_t& GetNoWriters()   { return noWriters_; }

private:
    /**
     * @brief The disposed count that indicates the number of times the instance had become
     * alive after it was disposed explicitly by a DataWriter, at the time the sample was received.
     */
    int32_t     disposed_;

    /**
     * @brief The no_writers_generation_count that indicates the number of times the instance
     * had become alive after it was disposed because there were no writers, at the time the
     * sample was received.
     */
    int32_t     noWriters_;
};

/**
 * @brief
 *
 * The sample_rank and generation_rank of the sample within the returned sequence. These ranks provide a preview of the samples
 * that follow within the sequence returned by the read or take operations.
 *
 * The sample_rank and generation_rank available in the SampleInfo are computed based solely on the actual
 * samples in the ordered collection returned by read or take.
 *
 * - The sample_rank indicates the number of samples of the same instance that follow the current one in the collection.
 * - The generation_rank available in the SampleInfo indicates the difference in ‘generations’ between the sample (S) and the
 *   Most Recent Sample of the same instance that appears In the returned Collection (MRSIC). That is, it counts the number
 *   of times the instance transitioned from not-alive to alive in the time from the reception of the S to the reception of MRSIC.
 *
 * The generation_rank is computed using the formula:
 *
 * generation_rank = (MRSIC.disposed_generation_count + MRSIC.no_writers_generation_count) - (S.disposed_generation_count + S.no_writers_generation_count)
 *
 * The absolute_generation_rank available in the SampleInfo indicates the difference in ‘generations’ between the sample (S) and the Most Recent Sample
 * of the same instance that the middleware has received (MRS). That is, it counts the number of times the instance transitioned from not-alive to alive
 * in the time from the reception of the S to the time when the read or take was called.
 *
 * absolute_generation_rank = (MRS.disposed_generation_count + MRS.no_writers_generation_count) - (S.disposed_generation_count + S.no_writers_generation_count)
 */
class DLL_STATE Rank
{
public:
    Rank(int32_t sample, int32_t generation, int32_t absoluteGeneration)
        : sample_(sample)
        , generation_(generation)
        , absoluteGeneration_(absoluteGeneration)
    { }

    Rank()
        : sample_(0)
        , generation_(0)
        , absoluteGeneration_(0)
    { }

    ~Rank()
    { }

    inline int32_t GetAbsoluteGeneration()  const {	return absoluteGeneration_;}
    inline int32_t GetGeneration()          const { return generation_; }
    inline int32_t GetSample()              const { return sample_; }

    inline int32_t& GetAbsoluteGeneration()  { return absoluteGeneration_;}
    inline int32_t& GetGeneration()          { return generation_; }
    inline int32_t& GetSample()              { return sample_; }

private:
    /**
     * @brief The sample that indicates the number of samples related to the same instance that follow
     * in the collection returned by read or take.
     */
    int32_t     sample_;

    /**
     * @brief The generation_rank that indicates the generation difference (number of times the
     * instance was disposed and become alive again) between the time the sample was received, and the
     * time the most recent sample in the collection related to the same instance was received.
     */
    int32_t     generation_;

    /**
     * @brief The absolute_generation_rank that indicates the generation difference (number of
     * times the instance was disposed and become alive again) between the time the sample was
     * received, and the time the most recent sample (which may not be in the returned collection)
     * related to the same instance was received.
     */
    int32_t     absoluteGeneration_;
};

class DLL_STATE SampleInfo
{
public:
    SampleInfo();

    SampleInfo(const Status::DataState &dataState,
               const Timestamp &timestamp,
               const InstanceHandle &instanceHandle,
               const InstanceHandle &publicationHandle,
               const GenerationCount &generationCount,
               const Rank &rank,
               bool validData);

    ~SampleInfo();


    static SampleInfo NeverSeenSample(const Timestamp &timestamp,
                                        const InstanceHandle &instanceHandle,
                                        const InstanceHandle &publicationHandle)
    {
        return SampleInfo(Status::DataState::NewInstance(),
                              timestamp,
                              instanceHandle,
                              publicationHandle,
                              GenerationCount(),
                              Rank(),
                              true);
    }

    static SampleInfo SampleReceived(SampleInfo &sampleInfo, const Timestamp &timestamp)
    {
        /*sampleInfo.SetSourceTimestamp(timestamp);

        Status::DataState currDataState = sampleInfo.GetDataState();

        UpdateInstanceState(currDataState.GetInstanceState());
        UpdateViewState(currDataState.GetInstanceState());
        */
        // TODO: Review implementation

        return sampleInfo;
    }

    static SampleInfo InstanceDisposedByWriter(SampleInfo &sampleInfo)
    {
        // TODO: Implement logic

        return sampleInfo;
    }

    static SampleInfo NoLiveWriters(SampleInfo &sampleInfo)
    {
        // TODO: Implement logic

        return sampleInfo;
    }

    /**
     * @brief
     * The generation_rank available in the SampleInfo indicates the difference in ‘generations’ between the sample (S) and the
     * Most Recent Sample of the same instance that appears In the returned Collection (MRSIC). That is, it counts the number
     * of times the instance transitioned from not-alive to alive in the time from the reception of the S to the reception of MRSIC.
     *
     * The generation_rank is computed using the formula:
     * generation_rank = (MRSIC.disposed_generation_count + MRSIC.no_writers_generation_count) - (S.disposed_generation_count + S.no_writers_generation_count)
     *
     * @param mrsic
     * @param currentSample
     * @return
     */
    static int32_t ComputeGenerationRank(const GenerationCount &mrsic, const GenerationCount &currentSample)
    {
        return (mrsic.GetDisposed() + mrsic.GetNoWriters()) - (currentSample.GetDisposed() + currentSample.GetNoWriters());
    }

    /**
     * @brief
     *
     * The absolute_generation_rank available in the SampleInfo indicates the difference in ‘generations’ between the sample (S) and the Most Recent Sample
     * of the same instance that the middleware has received (MRS). That is, it counts the number of times the instance transitioned from not-alive to alive
     * in the time from the reception of the S to the time when the read or take was called.
     *
     * absolute_generation_rank = (MRS.disposed_generation_count + MRS.no_writers_generation_count) - (S.disposed_generation_count + S.no_writers_generation_count)
     *
     * @param mrs
     * @param currentSample
     * @return
     */
    static int32_t ComputeAbsoluteGenerationRank(const GenerationCount &mrs, const GenerationCount &currentSample)
    {
        return (mrs.GetDisposed() + mrs.GetNoWriters()) - (currentSample.GetDisposed() + currentSample.GetNoWriters());
    }


    /**
     * @brief
     *
     * For each instance the middleware internally maintains two counts:
     * - the disposed_generation_count and no_writers_generation_count,
     * relative to each DataReader:
     *
     * - The disposed_generation_count and no_writers_generation_count are initialized to zero when the DataReader first detects the presence of a
     *   never-seen-before instance.
     * - The disposed_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_DISPOSED to ALIVE.
     * - The no_writers_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_NO_WRITERS to ALIVE.
     *
     * The disposed_generation_count and no_writers_generation_count available in the SampleInfo capture a snapshot of the corresponding counters at the time
     * the sample was received.
     *
     * @param instanceState
     */
    void UpdateInstanceState(const Status::InstanceState &instanceState)
    {
        // -------------------------------------------------------------------
        // The disposed_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_DISPOSED to ALIVE.
        // -------------------------------------------------------------------
        if(this->dataState_.GetInstanceState() == Status::InstanceState::NotAliveDisposed() && instanceState == Status::InstanceState::Alive())
        {
            this->generationCount_.GetDisposed()++;
        }
        // -------------------------------------------------------------------
        // The no_writers_generation_count is incremented each time the instance_state of the corresponding instance changes from NOT_ALIVE_NO_WRITERS to ALIVE.
        // -------------------------------------------------------------------
        else if(this->dataState_.GetInstanceState() == Status::InstanceState::NotAliveNoWriters() && instanceState == Status::InstanceState::Alive())
        {
            this->generationCount_.GetNoWriters()++;
        }
    }

    /**
     * @brief
     *
     * For each instance (identified by the key), the middleware internally maintains a view_state relative to each DataReader. The view_state can either be
     * NEW or NOT-NEW.
     *
     * - NEW indicates that either this is the first time that the DataReader has ever accessed samples of that instance, or else that the DataReader has
     *   accessed previous samples of the instance, but the instance has since been reborn (i.e., become not-alive and then alive again). These two cases are
     *   distinguished by examining the disposed_generation_count and the no_writers_generation_count.
     * - NOT-NEW indicates that the DataReader has already accessed samples of the same instance and that the instance has not been reborn since.
     *
     * The view_state available in the SampleInfo is a snapshot of the view_state of the instance relative to the DataReader used to access the samples at
     * the time the collection was obtained (i.e., at the time read or take was called). The view_state is therefore the same for all samples in the returned
     * collection that refer to the same instance.
     */
    void UpdateViewState(const Status::InstanceState &instanceState)
    {
        // TODO: if(this->GetGenerationCount() == 0)
        if(this->dataState_.GetViewState() == Status::ViewState::NewView())
        {
            // ------------------------------------------------------
            // TODO: change from New to NotNew can only change due to read/take operation in DataReader
            // ------------------------------------------------------
        }
        else if(this->dataState_.GetViewState() == Status::ViewState::NotNewView())
        {
            // ------------------------------------------------------
            // If state is in NotAlive then the view state is set to New
            // ------------------------------------------------------
            if(instanceState == Status::InstanceState::NotAliveDisposed() ||
                instanceState == Status::InstanceState::NotAliveNoWriters())
            {
                this->dataState_.SetViewState(Status::ViewState::NewView());
            }
            // ------------------------------------------------------
            // If state is Alive then state is set to NotNew
            // ------------------------------------------------------
            else if(instanceState == Status::InstanceState::Alive())
            {
                this->dataState_.SetViewState(Status::ViewState::NotNewView());
            }
        }
    }

    /**
     * @brief
     *
     * NEW indicates that either this is the first time that the DataReader has ever accessed samples of that instance, or else that the DataReader has
     * accessed previous samples of the instance, but the instance has since been reborn (i.e., become not-alive and then alive again). These two cases are
     * distinguished by examining the disposed_generation_count and the no_writers_generation_count.
     *
     * @return
     */
    bool IsReborn()
    {
        if(this->dataState_.GetViewState() == Status::ViewState::NewView() &&
                (this->generationCount_.GetDisposed() != 0 ||
                 this->generationCount_.GetNoWriters() != 0))
        {
            return true;
        }

        return false;
    }

public:

    // ---------------------------------------------------
    // Setters and Getters
    // ---------------------------------------------------

    void SetSourceTimestamp(const Timestamp &ts)          { sourceTimestamp_ = ts; }
    void SetInstanceHandle(const InstanceHandle &h)       { instanceHandle_ = h; }
    void SetPublicationHandle(const InstanceHandle &p)    { publicationHandle_ = p; }
    void SetValidData(const bool &vd)                     { validData_ = vd; }

    Timestamp GetSourceTimestamp()            const { return sourceTimestamp_; }
    InstanceHandle GetInstanceHandle()        const { return instanceHandle_; }
    InstanceHandle GetPublicationHandle()     const { return publicationHandle_; }

    Status::DataState&  GetDataState()              { return dataState_; }
    GenerationCount&    GetGenerationCount()        { return generationCount_; }
    Rank&               GetRank()                   { return rank_; }

    bool    IsValidData()                     const { return validData_; }

private:

    // ---------------------------------------------------
    // Variables
    // ---------------------------------------------------

    /**
     * The sample_state of the Data value (i.e., if the sample has already been READ or NOT_READ by that same DataReader
     * The view_state of the related instance (i.e., if the instance is NEW, or NOT_NEW for that DataReader)
     * The instance_state of the related instance (i.e., if the instance is ALIVE, NOT_ALIVE_DISPOSED, or NOT_ALIVE_NO_WRITERS)
     *
     * @brief dataState_
     */
    Status::DataState dataState_;

    /**
     * @brief Indicates the time provided by the DataWriter when the sample was written.
     */
    Timestamp sourceTimestamp_;

    /**
     * @brief Identifies locally the corresponding instance.
     */
    InstanceHandle instanceHandle_;

    /**
     * @brief identifies locally the DataWriter that modified the instance. The publication_handle is the same
     * InstanceHandle_t that is returned by the operation get_matched_publications on the DataReader and can
     * also be used as a parameter to the DataReader operation get_matched_publication_data.
     */
    InstanceHandle publicationHandle_;

    /**
     * @brief generationGount_
     */
    GenerationCount generationCount_;

    /**
     * @brief rank_
     */
    Rank rank_;

    /**
     * @brief indicates whether the DataSample contains data or else it is only used to communicate a change in the
     * instance_state of the instance.
     */
    bool validData_;
};

}}
