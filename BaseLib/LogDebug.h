#ifndef BaseLib_LogDebug_h_Included
#define BaseLib_LogDebug_h_Included

#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
//#include<inttypes.h>
#include<sstream>

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Export.h"

namespace BaseLib
{
// ----------------------------------------------------------------
//		class LogDebugBase
// ----------------------------------------------------------------
class DLL_STATE LogDebugBase
{
public:
	enum LogType 
	{
		NoLogType = -1,
		INFO,
		DEBUG,
		WARNING,
		CRITICAL,
		FATAL
	};

public:
	LogDebugBase() {}
	~LogDebugBase() {}

	static void LogMessage(LogDebugBase::LogType type, const char *msg, va_list ap);
	static void LogMessage(LogDebugBase::LogType type, const char *msg);

    friend std::ostream& operator<<(std::ostream& ostr, const LogType& logType) throw()
    {
        switch(logType)
        {
    #define CASE(t) case t: ostr << #t; break;
        CASE(INFO)
        CASE(DEBUG)
        CASE(WARNING)
        CASE(CRITICAL)
        CASE(FATAL)
    #undef CASE
        default:
            std::cerr << TSPRETTY_FUNCTION << "CRITICAL! Missing operator<< case for LogType!" << std::endl;
            break;
        }

        return ostr;
    }

private:
	static char* makeMessage(const char *fmt, va_list ap);
};

//std::ostream& operator<<(std::ostream &str, const LogDebugBase::LogType &type) throw();

// ----------------------------------------------------------------
// 		debug functions
// ----------------------------------------------------------------
DLL_STATE void iInfo(const char *msg, ...);
DLL_STATE void iDebug(const char *msg, ...);
DLL_STATE void iWarning(const char *msg, ...);
DLL_STATE void iCritical(const char *msg, ...);
DLL_STATE void iFatal(const char *msg, ...);

/*#define INTACT_NO_IDEBUG_MACRO while (false) iDebug
#ifdef INTACT_NO_DEBUG_OUTPUT
#  define iDebug INTACT_NO_IDEBUG_MACRO
#endif

#define INTACT_NO_IWARNING_MACRO while (false) iWarning
#ifdef INTACT_NO_WARNING_OUTPUT
#  define iWarning INTACT_NO_IWARNING_MACRO
#endif
*/

// ----------------------------------------------------------------
//		class DebugReport
// ----------------------------------------------------------------
class DLL_STATE DebugReport
{
private:
	DebugReport(std::string title = std::string(""))
			: numberOfWarnings(0)
			, numberOfErrors(0)
			, numberOfExceptions(0)
			, reportTitle(title)
	{}
	~DebugReport()
	{}

public:
	void ReportWarning(std::string report);
	void ReportError(std::string report);
	void ReportException(std::string report);

	/*friend std::ostream& operator<<(std::ostream &ostr, const DebugReport &testReport);
	{
		if(testReport.GetReportTitle().empty() == false)
			ostr << "-------------- " << testReport.GetReportTitle() << " DebugReport --------------" << endl;
		else
			ostr << "-------------- DebugReport --------------" << endl;

		if(testReport.GetWarningString().empty() == false)
		{
			ostr << "List of Warnings: " << endl;
			ostr << testReport.GetWarningString() << endl;
		}
		if(testReport.GetErrorString().empty() == false)
		{
			ostr << "List of Errors: " << endl;
			ostr << testReport.GetErrorString() << endl;
		}
		if(testReport.GetExceptionString().empty() == false)
		{
			ostr << "List of Exceptions: " << endl;
			ostr << testReport.GetExceptionString() << endl;
		}


		ostr << "Number of Warnings: " << testReport.GetNumberOfWarnings() << endl;
		ostr << "Number of Errors: " << testReport.GetNumberOfErrors() << endl;
		ostr << "Number of Exceptions: " << testReport.GetNumberOfExceptions() << endl;
		ostr << endl;		
		return ostr;
	}*/

	std::string GetWarningString()		const { return warningStream.str(); }
	std::string GetErrorString()		const { return errorStream.str(); }
	std::string GetExceptionString()	const { return exceptionStream.str(); }

	std::string GetReportTitle()		const { return reportTitle; }

	int GetNumberOfWarnings()			const { return numberOfWarnings; }
	int GetNumberOfErrors()				const { return numberOfErrors; }
	int GetNumberOfExceptions()			const { return numberOfExceptions; }
	
	static DebugReport*			GetOrCreate(); // throw();

private:
	static DebugReport *staticDebugReport;

	int					numberOfWarnings;
	int					numberOfErrors;
	int					numberOfExceptions;
	
	std::stringstream	warningStream;
	std::stringstream	errorStream;
	std::stringstream 	exceptionStream;

	std::string			reportTitle;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const DebugReport &testReport);

} // namespace BaseLib

#endif  // BaseLib_LogDebug_h_IsIncluded

