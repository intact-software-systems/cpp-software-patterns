#pragma once

#ifdef USE_GCC
#include <sys/time.h>
#endif

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

/**
 * Mac OS X does not have CLOCK_REALTIME defined, so I define it here to make code compile.
 *
 * TODO: How can be sure this does not have serious drawbacks?
 */
//#ifdef MACX
//#define CLOCK_REALTIME 0
//#endif

/**
 * @brief The Utility class provides some platform independent functions.
 */
class DLL_STATE Utility
{
private:
    Utility();

public:

    /**
     * Measure how much tima function call takes. Uses "Execute around object" software design pattern.
     */
//    template <typename Functor, typename ... Args>
//    auto measure(Functor f, Args && ... args)
//        -> decltype(f(std::forward<Args>(args)...))
//    {
//        struct scoped_timer
//        {
//            scoped_timer() : now_(std::chrono::high_resolution_clock::now()) {}
//            ~scoped_timer()
//            {
//                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now_).count();
//                std::cout << "Time elapsed: " << elapsed << "ms" << std::endl;
//            }
//            private:
//                std::chrono::high_resolution_clock::time_point const now_;
//        } scoped_timer;
//        return f(std::forward<Args>(args)...);
//    }


    /**
     * @brief GenerateUUID generates a universally unique id (UUID) aka. globally unique id (GUID).
     *
     * @param length defines the length of the returned UUID
     * @return a UUID
     */
    static std::vector<char> GenerateUUID(unsigned int length = 16);

    /**
     * @brief GetTypeName generates a typename based on type T
     * @return TYPE_NAME(T)
     */
    template <typename T>
    static std::string GetTypeName()
    {
        return TYPE_NAME(T);
    }

    /**
     * TODO: This only checks if T is
     */
    template<typename Base, typename T>
    static bool instanceof(T* t)
    {
        return dynamic_cast<Base*>(t) != nullptr;
        //return std::is_base_of<Base, T>::value;
    }

    template <typename T, typename V>
    static std::shared_ptr<T> To(std::shared_ptr<V> value)
    {
        return std::dynamic_pointer_cast<T>(value);
    }

    /**
     * @brief ProcessorClockTime
     *
     * Returns the processor time consumed by the program.
     * The value returned is expressed in clock ticks, which are units of time of a constant
     * but system-specific length (with a relation of CLOCKS_PER_SEC clock ticks per second).
     *
     * The epoch used as reference by clock varies between systems, but it is related to the
     * program execution (generally its launch). To calculate the actual processing time of a program,
     * the value returned by clock shall be compared to a value returned by a previous call to the same function.
     *
     * @return
     */
    static clock_t ProcessorClockTime();

    /**
     * @brief ComputeElapsedProcessorClockMS
     * @param startTime
     * @return
     */
    static int64 ComputeElapsedProcessorClockMS(int64 startTime);

    /**
     * @brief ComputeElapsedProcessorClockMsec
     * @param startTime
     * @return
     */
    static int64 ComputeElapsedProcessorClockMsec(int64 startTime);

    /**
     * @brief getCurrentTimeMs
     *
     * @return time in msec since epoch
     */
    static int64 GetCurrentTimeMs();

    /**
     * @brief PrintCurrentTimeMs
     * @return
     */
    static std::string PrintCurrentTimeMsec();

    /**
     * @brief GetCurrentTimeSec
     *
     * @return time in seconds since epoch
     */
    static int64 GetCurrentTimeSec();

    /**
     * @brief ClockInitWaitTimevalMsec
     * @param tv
     * @param msecs
     * @return
     */
    static int ClockInitWaitTimevalMsec(struct timeval &tv, uint64 msecs);

    /**
     * @brief ClockInitWaitTimevalUsec
     * @param tv
     * @param usecs
     * @return
     */
    static int ClockInitWaitTimevalUsec(struct timeval &tv, uint64 usecs);

    /**
     * @brief clockInitWaitTimespecMsec
     *
     * @param ts
     * @param nsecs
     * @return
     */
    static int ClockInitWaitTimespecMsec(struct timespec &ts, uint64 msecs);

    /**
     * @brief clockInitWaitTimespecUsec
     *
     * @param ts
     * @param nsecs
     * @return
     */
    static int ClockInitWaitTimespecUsec(struct timespec &ts, uint64 usecs);

    /**
     * @brief clockInitWaitTimespecNsec
     *
     * @param ts
     * @param nsecs
     * @return
     */
    static int ClockInitWaitTimespecNsec(struct timespec &ts, uint64 nsecs);

    /**
     * @brief clockInitWaitTimeMsec
     *
     * @param ts
     * @param msecs
     * @return 0 if success
     */
    static int ClockInitEpochWaitTimeMsec(struct timespec &ts, uint64 msecs);

    /**
     * @brief clockInitWaitTimeUsec
     *
     * @param ts
     * @param usecs
     * @return 0 if success
     */
    static int ClockInitEpochWaitTimeUsec(struct timespec &ts, uint64 usecs);

    /**
     * @brief clockInitWaitTimeNsec
     *
     * @param ts
     * @param nsecs
     * @return 0 if success
     */
    static int ClockInitEpochWaitTimeNsec(struct timespec &ts, uint64 nsecs);

    /**
     * @brief clockGetRealTime
     *
     * Mac OS X does not support clock_gettime, so this function hides the fact. In addition, CLOCK_REALTIME
     * is not found on Mac OS X either, so I simply create a specialized function for this.
     *
     * Windows has its own implementation.
     *
     * @param typeœ
     * @param ts
     * @return 0 if success
     */
    static int ClockCurrentTime(struct timespec &ts);

    /**
     * @brief pthread_timedjoin
     *
     * A portable implementation of pthread_timedjoin_np. It includes a "hack" of a function to support
     * the timedjoin on Mac OS X. Windows has its own implementation.
     *
     *
     * @param td
     * @param res
     * @param ts
     * @return
     */
#ifndef WIN32
    static int pthread_timedjoin(pthread_t td, void **res, struct timespec *ts);

    static int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const timespec *ts);
#endif

};

}
