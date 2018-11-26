#include"BaseLib/Utility.h"
#include"BaseLib/Debug.h"

#define BILLION     1000000000
#define MILLION     1000000
#define THOUSAND    1000

namespace BaseLib
{
#ifdef MACX
/*#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int clk_id, struct timespec *t)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t time;
    time = mach_absolute_time();
    double nseconds = ((double)time * (double)timebase.numer)/((double)timebase.denom);
    double seconds = ((double)time * (double)timebase.numer)/((double)timebase.denom * 1e9);
    t->tv_sec = seconds;
    t->tv_nsec = nseconds;
    return 0;
}*/

void current_utc_time(struct timespec *ts)
{
#ifdef MACX // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_REALTIME, ts);
#endif
}

#endif
// ----------------------------------------------------------------
// No gettimeofday on windows use:
// Use: http://social.msdn.microsoft.com/Forums/vstudio/en-US/430449b3-f6dd-4e18-84de-eebd26a8d668/gettimeofday
// ----------------------------------------------------------------

#ifdef WIN32

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000 //Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000 //ULL
#endif

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;

  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    int64 tmpres = 0;
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;  /*convert into microseconds*/
    tv->tv_sec = long(tmpres / MILLION);
    tv->tv_usec = long(tmpres % MILLION);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

#define CLOCK_REALTIME 1

int clock_gettime(int , struct timespec *tv) //, struct timezone *tz)
{
  FILETIME ft;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    uint64 tmpres = 0;
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;  /*convert into microseconds*/

    //IDEBUG() << " tmpres " << tmpres;

    tv->tv_sec = (tmpres / MILLION);
    tv->tv_nsec = (tmpres % MILLION) * THOUSAND;
  }

  //IDEBUG() << " tv->tv_sec " << tv->tv_sec << " tv->tv_nsec " <<  tv->tv_nsec;

  return 0;
}


/*
LARGE_INTEGER
getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int
clock_gettime(int X, struct timeval *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}

int
clock_gettime(int X, struct timespec *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    //double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if(!initialized)
    {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);

        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }


    if (usePerformanceCounter)
    {
        QueryPerformanceCounter(&t);
    }
    else
    {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    double microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / MILLION;
    tv->tv_nsec = (t.QuadPart % MILLION) * THOUSAND;

    IDEBUG() << tv->tv_nsec;

    return (0);
}

*/
#endif

/**
 * @brief Utility::Utility
 */
Utility::Utility()
{ }

/**
 * @brief GenerateUUID generates a universally unique id (UUID) aka. globally unique id (GUID).
 *
 * @param length defines the length of the returned UUID
 * @return a UUID
 */
std::vector<char> Utility::GenerateUUID(unsigned int length)
{
    std::vector<char> uuid(length, 'i');

    for(unsigned int i = 0; i < length; i++)
    {
        char rand = (char) (std::rand() % 10);
        uuid.push_back(rand);
    }

    return uuid;
}

/**
 * @brief Utility::ProcessorClockTime
 *
 * Returns the processor time consumed by the program.
 *
 * The value returned is expressed in clock ticks, which are units of time of a constant
 * but system-specific length (with a relation of CLOCKS_PER_SEC clock ticks per second).
 *
 * The epoch used as reference by clock varies between systems, but it is related to the
 * program execution (generally its launch). To calculate the actual processing time of a program,
 * the value returned by clock shall be compared to a value returned by a previous call to the same function.
 *
 * Returns the approximate processor time used by the process since the beginning of an implementation-defined
 * era related to the program's execution. To convert result value to seconds divide it by CLOCKS_PER_SEC.
 *
 * Only the difference between two values returned by different calls to std::clock is meaningful,
 * as the beginning of the std::clock era does not have to coincide with the start of the program.
 *
 * std::clock time may advance faster or slower than the wall clock, depending on the execution
 * resources given to the program by the operating system. For example, if the CPU is shared by
 * other processes, std::clock time may advance slower than wall clock. On the other hand, if
 * the current process is multithreaded and more than one execution core is available, std::clock time
 * may advance faster than wall clock.
 *
 * @return
 */
clock_t Utility::ProcessorClockTime()
{
    return std::clock();
}

/**
 * @brief Utility::ComputeElapsedProcessorClockMS
 * @param startTime
 * @return
 */
int64 Utility::ComputeElapsedProcessorClockMS(int64 startTime)
{
    int64 t = Utility::ProcessorClockTime() - startTime;

    return (int64) ((double) t/ CLOCKS_PER_SEC) * THOUSAND;
}


/**
 * @brief Utility::ComputeElapsedProcessorClockMsec
 * @param startTime
 * @return
 */
int64 Utility::ComputeElapsedProcessorClockMsec(int64 startTime)
{
    int64 t = Utility::ProcessorClockTime() - startTime;

    return (int64) (( (double) t) / CLOCKS_PER_SEC) * MILLION;
}

/**
 * @brief Utility::getCurrentTimeMs
 * @return
 */
int64 Utility::GetCurrentTimeMs()
{
    using Clock = std::chrono::high_resolution_clock;
    return Clock::now().time_since_epoch().count();

//    struct timespec ts = {0,0};
//    int ret = Utility::ClockCurrentTime(ts);
//
//    if(ret != 0)
//    {
//        ICRITICAL() << "Failed to initialize timespec!";
//        return -1;
//    }
//
//    int64 currentTimeMs = (ts.tv_sec * THOUSAND) + floor(ts.tv_nsec / MILLION);
//
//    ASSERT(currentTimeMs > 0);
//
//    return currentTimeMs;
}

/**
 * @brief Utility::PrintCurrentTimeMs
 * @return
 */
std::string Utility::PrintCurrentTimeMsec()
{
    char buffer[30];
    struct timeval tv = {0,0};

    gettimeofday(&tv, NULL);
    time_t curtime = std::time(NULL); //Utility::GetCurrentTimeSec();

    std::strftime(buffer,30, "%m/%d/%y %H:%M:%S", std::localtime(&curtime));

    std::stringstream out;
    out << buffer << "." << tv.tv_usec;
    return out.str();
}

/**
 * @brief Utility::GetCurrentTimeSec
 * @return
 */
int64 Utility::GetCurrentTimeSec()
{
    return ::time(NULL);
}

/**
 * @brief Utility::ClockInitWaitTimevalMsec
 * @param tv
 * @param msecs
 * @return
 */
int Utility::ClockInitWaitTimevalMsec(timeval &tv, uint64 msecs)
{
    if(msecs >= LONG_MAX)
    {
        tv.tv_sec = LONG_MAX;
        tv.tv_usec = INT_MAX;
        return 0;
    }

    return Utility::ClockInitWaitTimevalUsec(tv, msecs * THOUSAND);
}

/**
 * @brief Utility::ClockInitWaitTimevalUsec
 * @param tv
 * @param usecs
 * @return
 */
int Utility::ClockInitWaitTimevalUsec(timeval &tv, uint64 usecs)
{
    tv.tv_sec = long(floor(usecs/MILLION));
    tv.tv_usec = usecs % MILLION;

    return 0;
}

/**
 * @brief Utility::clockInitWaitTimespecMsec
 * @param ts
 * @param msecs
 * @return
 */
int Utility::ClockInitWaitTimespecMsec(struct timespec &ts, uint64 msecs)
{
    return Utility::ClockInitWaitTimespecNsec(ts, msecs * MILLION);
}

/**
 * @brief Utility::clockInitWaitTimespecUsec
 * @param ts
 * @param usecs
 * @return
 */
int Utility::ClockInitWaitTimespecUsec(struct timespec &ts, uint64 usecs)
{
    return Utility::ClockInitWaitTimespecNsec(ts, usecs * THOUSAND);
}

/**
 * @brief Utility::clockInitWaitTimespecNsec
 * @param ts
 * @param nsecs
 * @return
 */
int Utility::ClockInitWaitTimespecNsec(struct timespec &ts, uint64 nsecs)
{
    ts.tv_sec = time_t(floor(nsecs/BILLION));
    ts.tv_nsec = nsecs % BILLION;

    return 0;
}

/**
 * @brief Utility::clockInitEpochWaitTimeMsec
 * @param ts
 * @param msecs
 * @return
 */
int Utility::ClockInitEpochWaitTimeMsec(struct timespec &ts, uint64 msecs)
{
    if(msecs >= LONG_MAX)
    {
        return Utility::ClockInitEpochWaitTimeNsec(ts, msecs);
    }

    return Utility::ClockInitEpochWaitTimeNsec(ts, msecs * MILLION);
}

/**
 * @brief Utility::clockInitEpochWaitTimeUsec
 * @param ts
 * @param usecs
 * @return
 */
int Utility::ClockInitEpochWaitTimeUsec(struct timespec &ts, uint64 usecs)
{
    if(usecs >= LONG_MAX)
    {
        return Utility::ClockInitEpochWaitTimeNsec(ts, usecs);
    }

    return Utility::ClockInitEpochWaitTimeNsec(ts, usecs * THOUSAND);
}

/**
 * @brief Utility::clockInitEpochWaitTimeNsec
 * @param ts
 * @param nsecs
 * @return
 */
int Utility::ClockInitEpochWaitTimeNsec(struct timespec &ts, uint64 nsecs)
{
#if 0 //def WIN32 //MACX
    struct timeval tv;
    int rv = gettimeofday(&tv, NULL);
    if (rv) return rv;

    //uint64 seconds = floor(nsecs/BILLION);
    //uint64 nanoseconds = (nsecs % BILLION);

    ts.tv_nsec = (tv.tv_usec * THOUSAND) + nsecs;
    ts.tv_sec  = tv.tv_sec + floor(ts.tv_nsec / BILLION);

    ts.tv_nsec %= BILLION;

#elif MACX
    struct timespec tv = {0,0};

    current_utc_time(&tv);

    if(nsecs >= LONG_MAX)
    {
        ts.tv_sec = tv.tv_sec + (LONG_MAX - 1 - tv.tv_sec);
        ts.tv_nsec = 0; //tv.tv_nsec + MILLION;
    }
    else
    {
        ts.tv_nsec = tv.tv_nsec + nsecs;
        ts.tv_sec  = tv.tv_sec + floor(ts.tv_nsec / BILLION);

        ts.tv_nsec %= BILLION;
    }

#else //if USE_GCC
    struct timespec tv = {0,0};
    int ret = clock_gettime(CLOCK_REALTIME, &tv);
    if(ret != 0) throw std::runtime_error("Utility::clockGetTime(time): clock_gettime(CLOCK_REALTIME, ts) failed");

    ts.tv_nsec = tv.tv_nsec + nsecs;
    ts.tv_sec  = tv.tv_sec + time_t(floor(ts.tv_nsec / BILLION));

    ts.tv_nsec %= BILLION;
#endif

    // -- debug --
    if(ts.tv_nsec < 0 || ts.tv_sec < 0)
    {
        ICRITICAL() << "ts {" << ts.tv_sec << "," << ts.tv_nsec << ")";
        ASSERT(ts.tv_nsec >= 0);
        ASSERT(ts.tv_sec >= 0);
    }
    // -- debug --

    return 0;
}

/**
 * @brief Utility::clockCurrentTime
 * @param ts
 * @return
 */
int Utility::ClockCurrentTime(struct timespec &ts)
{

#if 0 //def WIN32
    // Use: http://social.msdn.microsoft.com/Forums/vstudio/en-US/430449b3-f6dd-4e18-84de-eebd26a8d668/gettimeofday
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;

    ts.tv_sec  = now.tv_sec;
    ts.tv_nsec = now.tv_usec * THOUSAND;

    return 0;
#elif 0
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;

    ts.tv_sec  = now.tv_sec;
    ts.tv_nsec = now.tv_usec * THOUSAND;

    return 0;
#elif MACX
    current_utc_time(&ts);

    return 0;
#else
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
    if(ret != 0) throw std::runtime_error("Utility::clockCurrentTime(time): clock_gettime(CLOCK_REALTIME, ts) failed");

    return 0;
#endif
}

#ifdef MACX

struct args {
    int joined;
    pthread_t td;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    void **res;
};

static void *waiter(void *ap)
{
    struct args *args = (struct args*)ap;
    pthread_join(args->td, args->res);
    pthread_mutex_lock(&args->mtx);
    pthread_mutex_unlock(&args->mtx);
    args->joined = 1;
    pthread_cond_signal(&args->cond);
    return 0;
}

int pthread_timedjoin_macx(pthread_t td, void **res, struct timespec *ts)
{
    pthread_t tmp;
    int ret;
    struct args args; // = {0,0,0,0,0,0}; // = { .td = td, .res = res };
    args.td = td;
    args.res = res;
    args.joined = 0;

    pthread_mutex_init(&args.mtx, 0);
    pthread_cond_init(&args.cond, 0);
    pthread_mutex_lock(&args.mtx);

    ret = pthread_create(&tmp, 0, waiter, &args);
    if (ret) return ret;

    do ret = ::pthread_cond_timedwait(&args.cond, &args.mtx, ts);
    while (!args.joined && ret != ETIMEDOUT);

    pthread_cancel(tmp);
    pthread_join(tmp, 0);

    pthread_cond_destroy(&args.cond);
    pthread_mutex_destroy(&args.mtx);

    return args.joined ? 0 : ret;
}

#endif

#ifndef WIN32
int Utility::pthread_timedjoin(pthread_t td, void **res, timespec *ts)
{
#ifdef MACX
    return pthread_timedjoin_macx(td, res, ts);
#elif USE_GCC
    return pthread_timedjoin_np(td, res, ts);
#endif
    return -1;
}

int Utility::pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *ts)
{
    int code = ::pthread_cond_timedwait(cond, mutex, ts);

    // -- debug --
    if(code == ETIMEDOUT)
        IDEBUG() << "The time specified by abstime to pthread_cond_timedwait() has passed";
    else if(code == EINVAL)
        ICRITICAL() << "The value specified by abstime is invalid: ts(" << ts->tv_sec << "," << ts->tv_nsec << ")";
    else if(code == EPERM)
        iWarning("The mutex was not owned by the current thread at the time of the call");
    else if(code == EDEADLK)
        iWarning("Thread::wait(): EDEADLK A deadlock was detected or the value  of  thread  specifies  the calling thread.");
    // -- debug --

    return code;
}

#endif

}
