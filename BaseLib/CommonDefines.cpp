#include"BaseLib/CommonDefines.h"
#include"BaseLib/Thread.h"
#include"BaseLib/Utility.h"

#ifdef USE_GCC

#include <execinfo.h>

#endif

#include <fstream>

namespace BaseLib {

std::string GetTimeStamp()
{
    return Utility::PrintCurrentTimeMsec();
}

int GetTimeStampInteger()
{
#ifdef USE_GCC
    time_t    ltime;
    struct tm newtime;
    ltime              = time(NULL);
    localtime_r(&ltime, &newtime);

    struct timeval detail_time;
    int            ret = gettimeofday(&detail_time, NULL);
    if(ret == -1) std::cout << "GetTimeStampInteger(): ERORR! gettimeofday() failed! " << std::endl;

    int timeStamp = (newtime.tm_hour * 60 * 60 * 1000) + (newtime.tm_min * 60 * 1000) + (newtime.tm_sec * 1000) + detail_time.tv_usec / 1000;
    return timeStamp;
#elif WIN32
    time_t ltime;
    struct tm newtime;
    ltime = time(NULL);
    localtime_s(&newtime, &ltime);

    struct _timeb detail_time;
    int ret = ::_ftime_s(&detail_time);
    if(ret == -1) std::cout << "HostInformationSpace::GetTimeStamp(): ERROR! ftime() failed! " << std::endl;

    int timeStamp = (newtime.tm_hour * 60 * 60 * 1000) + (newtime.tm_min * 60 * 1000) + (newtime.tm_sec * 1000) + detail_time.millitm;
    return timeStamp;
#endif
}

std::string WriteFunctionToString(const char* function, int line)
{
    Thread* currentThread = Thread::currentThread();

    std::stringstream str;
    str << "[";
    if(currentThread)
    {
        str << *currentThread;
    }
    else
    {
        str << "Main";
    }

    str << ":" << function << "," << line << "] ";
    return str.str();
}

std::string WriteTimedFunctionToString(const std::string& time, const char* function, int line)
{
    Thread* currentThread = Thread::currentThread();

    std::stringstream str;
    str << "[";
    if(currentThread)
    {
        str << *currentThread;
    }
    else
    {
        str << "Main";
    }

    str << ":" << time << "," << function << "," << line << "] ";


    return str.str();
}

void WriteAssertFailedToString(const char* test, const char* file, const char* function, int line, std::function<void()> endFunction)
{
    // -- generate backtrace --
    std::stringstream stream;
    {
#ifdef USE_GCC
        void* array[25];
        int nSize = backtrace(array, 25);
        char** symbols = backtrace_symbols(array, nSize);
        for(int i = 0; i < nSize; i++)
        {
            if(symbols[i] == NULL) continue;

            stream << symbols[i] << std::endl;
        }
        free(symbols);
#endif
    }
    // -- generate backtrace --

    Thread* thread = Thread::currentThread();
    std::stringstream ostr;
    ostr << "---------------------------------- Intact Assert ----------------------------------" << std::endl;
    ostr << "Type:                  \tAbort in thread " << Thread::currentThreadId() << std::endl;
    if(thread)
    {
        ostr << "Thread details:        \t" << thread->getThreadDetails() << std::endl;
    }
    ostr << "Local timestamp:       \t" << Utility::PrintCurrentTimeMsec() << std::endl;
    ostr << "Location:              \t" << file << "," << line << ":" << function << std::endl;
    ostr << "Stack trace:           \t" << std::endl << stream.str() << std::endl;
    ostr << "Assertion failed:      \t" << test << std::endl;
    ostr << "--------------------------------------------------------------------------------------" << std::endl;

    std::ofstream ofile("abort_log.txt", std::ios::out | std::ios::app);
    if(ofile.is_open())
    {
        ofile << ostr.str();
        ofile.flush();
        ofile.close();
    }

    //std::cerr << "Assertion Failed: " << test << std::endl << "Location: " << file << "," << line << ":" << function << std::endl;
    //std::cerr << "Further details are found in abort_log.txt" << std::endl;
    std::cerr << ostr.str();

    endFunction();
}

std::string GetCompilerInfo()
{
    std::stringstream ostr;

    //ostr << "This is the line number " << __LINE__;
    //ostr << " of file " << __FILE__ << ".\n";
    ostr << "The compilation began " << __DATE__;
    ostr << " at " << __TIME__ << "." << std::endl;
    ostr << "The compiler gives a __cplusplus value of " << __cplusplus;
    ostr << std::endl;

    return ostr.str();
}

}
