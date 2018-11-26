#include "BaseLib/LogDebug.h"
#include "BaseLib/Debug.h"
#include "BaseLib/Exception.h"

#include <stdio.h>

using namespace std;

namespace BaseLib
{

/*std::ostream& operator<<(std::ostream &str, const LogDebugBase::LogType &type) throw()
{
#define PRINTOUT( name ) case name: str << #name; break;
    switch(type)
    {
        PRINTOUT(LogDebugBase::NoLogType)
        PRINTOUT(LogDebugBase::INFO)
        PRINTOUT(LogDebugBase::DEBUG)
        PRINTOUT(LogDebugBase::WARNING)
        PRINTOUT(LogDebugBase::CRITICAL)
        PRINTOUT(LogDebugBase::FATAL)
        default:
            IDEBUG() << " did not recoginze LogDebugBase::LogType" ;
            break;
    }
#undef PRINTOUT
    return str;
}*/


// ----------------------------------------------------------------
//		class LogDebugBase
// ----------------------------------------------------------------
char* LogDebugBase::makeMessage(const char *fmt, va_list ap)
{
    // Guess we need no more than 100 bytes.
    int n, size = 150;
    char *p, *np;

    if ((p = (char*)malloc(size)) == NULL)
        return NULL;

    //bool firstTime = true;
    while(true)
    {
        // Try to print in the allocated space.
        //if(firstTime == false)
        //	va_start(ap); //, fmt);
        //firstTime = true;
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);
        // If that worked, return the string.
        if (n > -1 && n < size)
            return p;

        // Failsafe: try again with more space.
        if (n > -1)    // glibc 2.1
            size = n+1; // precisely what is needed
        else           // glibc 2.0
            size *= 2;  // twice the old size

        if ((np = (char*)realloc (p, size)) == NULL)
        {
            free(p);
            return NULL;
        }
        else
        {
            p = np;
        }
    }
}


void LogDebugBase::LogMessage(LogDebugBase::LogType type, const char *msg, va_list ap)
{
    char *buf = NULL;

    try
    {
        buf = LogDebugBase::makeMessage(msg, ap);
        ASSERT(buf);

        //IDEBUG() << "Made message : " << buf ;
        LogDebugBase::LogMessage(type, buf);
    }
    catch(GeneralException ex)
    {
        IDEBUG() << "WARNING! Exception caught : " << ex.what();
    }
    catch(...)
    {
        IDEBUG() << "WARNING! Unhandled exception caught!";
    }

    if(buf) free(buf);
}

void LogDebugBase::LogMessage(LogDebugBase::LogType type, const char *buf)
{
    try
    {
        //TODO: fputs buf to file if logging is necessary
        switch(type)
        {
            case LogDebugBase::INFO:
            {
                fprintf(stdout, "%s\n", buf);
                fflush(stdout);
                break;
            }
            case LogDebugBase::DEBUG:
            {
                fprintf(stdout, "%s\n", buf);
                fflush(stdout);
                break;
            }
            case LogDebugBase::WARNING:
            {
                fprintf(stdout, "%s\n", buf);
                fflush(stdout);
                break;
            }
            case LogDebugBase::CRITICAL:
            {
                fprintf(stderr, "%s\n", buf);
                fflush(stderr);

                DebugReport::GetOrCreate()->ReportError(buf);
                //ASSERT(type != LogDebugBase::CRITICAL);
                break;
            }
            case LogDebugBase::FATAL:
            {
                fprintf(stderr, "%s\n", buf);
                fflush(stderr);

                DebugReport::GetOrCreate()->ReportError(buf);
                ASSERT(type != LogDebugBase::FATAL);
                break;
            }
            case LogDebugBase::NoLogType:
            default:
                // do not print anything!
                break;
        }

        std::ofstream ofile("intact_log.txt", std::ios::out | std::ios::app);
        if(ofile.is_open())
        {
            ofile << buf << std::endl;
            ofile.flush();
            ofile.close();
        }
    }
    catch(GeneralException ex)
    {
        IDEBUG() << "WARNING! Exception caught : " << ex.what();
    }
    catch(...)
    {
        IDEBUG() << "WARNING! Unhandled exception caught!";
    }
}

// ----------------------------------------------------------------
// 		debug functions
// ----------------------------------------------------------------
void iInfo(const char *msg, ...)
{
#ifndef INTACT_NO_DEBUG_OUTPUT
    va_list ap;
    va_start(ap, msg); // use variable arg list
    LogDebugBase::LogMessage(LogDebugBase::INFO, msg, ap);
    va_end(ap);
#endif
}

void iDebug(const char *msg, ...)
{
#ifndef INTACT_NO_DEBUG_OUTPUT
    va_list ap;
    va_start(ap, msg); // use variable arg list
    LogDebugBase::LogMessage(LogDebugBase::DEBUG, msg, ap);
    va_end(ap);
#endif
}

void iWarning(const char *msg, ...)
{
#ifndef INTACT_NO_DEBUG_OUTPUT
    va_list ap;
    va_start(ap, msg); // use variable arg list
    LogDebugBase::LogMessage(LogDebugBase::WARNING, msg, ap);
    va_end(ap);
#endif
}

void iCritical(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    LogDebugBase::LogMessage(LogDebugBase::CRITICAL, msg, ap);
    va_end(ap);
}

void iFatal(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    LogDebugBase::LogMessage(LogDebugBase::FATAL, msg, ap);
    va_end(ap);
}

// ----------------------------------------------------------------
//		class DebugReport
// ----------------------------------------------------------------
DebugReport* DebugReport::staticDebugReport = NULL;

DebugReport* DebugReport::GetOrCreate()	//throw()
{
    if(DebugReport::staticDebugReport == NULL)
        DebugReport::staticDebugReport = new DebugReport(string("Intact"));

    return DebugReport::staticDebugReport;
}

void DebugReport::ReportWarning(std::string report)
{
    if(report.empty())
        cout << "ReportWarning(report): WARNING! Reported warning is empty!" << std::endl;

    numberOfWarnings++;
    warningStream << numberOfWarnings << ": " << report << std::endl;
}

void DebugReport::ReportError(std::string report)
{
    if(report.empty())
        cout << "ReportError(report): WARNING! Reported error is empty!" << std::endl;

    numberOfErrors++;
    errorStream << numberOfErrors << ": " << report << std::endl;
}

void DebugReport::ReportException(std::string report)
{
    if(report.empty())
        cout << "ReportException(report): WARNING! Reported exception is empty!" << std::endl;

    numberOfExceptions++;
    exceptionStream << numberOfExceptions << ": " << report << std::endl;
}

std::ostream& operator<<(std::ostream &ostr, const DebugReport &testReport)
{
    if(!testReport.GetReportTitle().empty())
        ostr << "-------------- " << testReport.GetReportTitle() << " DebugReport --------------" << endl;
    else
        ostr << "-------------- DebugReport --------------" << endl;

    if(!testReport.GetWarningString().empty())
    {
        ostr << "List of Warnings: " << endl;
        ostr << testReport.GetWarningString() << endl;
    }
    if(!testReport.GetErrorString().empty())
    {
        ostr << "List of Errors: " << endl;
        ostr << testReport.GetErrorString() << endl;
    }
    if(!testReport.GetExceptionString().empty())
    {
        ostr << "List of Exceptions: " << endl;
        ostr << testReport.GetExceptionString() << endl;
    }


    ostr << "Number of Warnings: " << testReport.GetNumberOfWarnings() << endl;
    ostr << "Number of Errors: " << testReport.GetNumberOfErrors() << endl;
    ostr << "Number of Exceptions: " << testReport.GetNumberOfExceptions() << endl;
    ostr << endl;
    return ostr;
}



} // namespace BaseLib

/*char* LogDebugBase::makeMessage(const char *fmt, ...)
{
    // Guess we need no more than 100 bytes.
    int n, size = 150;
    char *p, *np;
    va_list ap;

    if ((p = malloc(size)) == NULL)
        return NULL;

    while(true)
    {
        // Try to print in the allocated space.
        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);
        // If that worked, return the string.
        if (n > -1 && n < size)
            return p;
        // Else try again with more space.
        if (n > -1)    // glibc 2.1
            size = n+1; // precisely what is needed
        else           // glibc 2.0
            size *= 2;  // twice the old size

        if ((np = realloc (p, size)) == NULL)
        {
            free(p);
            return NULL;
        }
        else
        {
            p = np;
        }
    }
}
*/

