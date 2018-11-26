#include "BaseLib/Exception.h"
#include "BaseLib/Thread.h"
#include "BaseLib/Utility.h"

namespace BaseLib {

std::ostream& operator<<(std::ostream& str, const ExceptionBase::ExceptionType& type) throw()
{
#define PRINTOUT(name) case name: str << #name; break;
    switch(type)
    {
        PRINTOUT(ExceptionBase::ExceptionType::GeneralException)
        PRINTOUT(ExceptionBase::ExceptionType::Exception)
        PRINTOUT(ExceptionBase::ExceptionType::IOException)
        PRINTOUT(ExceptionBase::ExceptionType::ErrorException)
        PRINTOUT(ExceptionBase::ExceptionType::WarningException)
        PRINTOUT(ExceptionBase::ExceptionType::CriticalException)
        PRINTOUT(ExceptionBase::ExceptionType::FatalException)
        PRINTOUT(ExceptionBase::ExceptionType::DeadlockException)
        PRINTOUT(ExceptionBase::ExceptionType::InterruptedException)
        PRINTOUT(ExceptionBase::ExceptionType::UnsupportedException)
        default:
            IDEBUG() << " did not recognize ExceptionType";
            break;
    }
#undef PRINTOUT
    return str;
}

/*-----------------------------------------------------------------------
        constructor/destructor/functions ExceptionTracer
------------------------------------------------------------------------- */

ExceptionTracer::ExceptionTracer()
try
{
#ifdef USE_GCC
    void* array[25];
    int nSize = backtrace(array, 25);
    char** symbols = backtrace_symbols(array, nSize);

    std::stringstream stream;
    stream << PRETTY_FUNCTION << std::endl;

    for(int i = 0; i < nSize; i++)
    {
        if(symbols[i] == NULL) continue;

        stream << symbols[i] << std::endl;
    }

    exceptionTrace_ = stream.str();
    free(symbols);
#endif
}
catch(std::exception ex)
{
    IDEBUG() << "ExceptionTracer failed: " << ex.what();
}
catch(...)
{
    IDEBUG() << "ExceptionTracer failed: Unknown error.";
}

ExceptionTracer::~ExceptionTracer()
{

}

std::string ExceptionTracer::GetExceptionTrace() const throw()
{
    return exceptionTrace_;
}

/*-----------------------------------------------------------------------
        constructor/destructor/functions ExceptionBase
------------------------------------------------------------------------- */
ExceptionBase::ExceptionBase(ExceptionBase::ExceptionType type)
//: ObjectBase()
    : exceptionType_(type)
{
    //static int count = 0;
    //stringstream ostr;
    //ostr << "BaseLib.Exception." << type << "." << count++;
    //ObjectBase::SetObjectName(ostr.str());
}

ExceptionBase::~ExceptionBase() throw()
{

}

ExceptionBase::ExceptionType ExceptionBase::GetExceptionType() const throw()
{
    return exceptionType_;
}

/*-----------------------------------------------------------------------
        constructor/destructor/functions Exception
------------------------------------------------------------------------- */
GeneralException::GeneralException(std::string txt, ExceptionBase::ExceptionType type)
    : std::runtime_error(txt)
    , ExceptionBase(type)
{
    Thread* thread = Thread::currentThread();

    std::stringstream ostr;
    ostr << "---------------------------------- Intact Exception ----------------------------------" << std::endl;
    ostr << "Type:                  \t" << this->GetExceptionType() << " in thread " << Thread::currentThreadId() << std::endl;
    if(thread)
    {
        ostr << "Thread details 		\t" << thread->getThreadDetails() << std::endl;
    }
    ostr << "Local timestamp:       \t" << Utility::PrintCurrentTimeMsec() << std::endl;
    ostr << "Stack trace:           \t" << std::endl << GetExceptionTrace() << std::endl;
    ostr << "Message details:       \t" << std::runtime_error::what() << std::endl;
    ostr << "--------------------------------------------------------------------------------------" << std::endl;

    exceptionMsg_ = ostr.str();
}

GeneralException::~GeneralException() throw()
{

}

std::string GeneralException::msg() const throw()
{
    return exceptionMsg_;
}

const char* GeneralException::what() const throw()
{
    return exceptionMsg_.c_str();
}

/*-----------------------------------------------------------------------
                specialized exception classes
------------------------------------------------------------------------- */
Exception::Exception(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    // NOTE: This exception is used in allowed states of the system
    //DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

IOException::IOException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

ErrorException::ErrorException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

WarningException::WarningException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

CriticalException::CriticalException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

FatalException::FatalException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

DeadlockException::DeadlockException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

CancelException::CancelException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

InterruptedException::InterruptedException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

UnsupportedException::UnsupportedException(std::string txt, ExceptionBase::ExceptionType type)
    : GeneralException(txt, type)
{
    DebugReport::GetOrCreate()->ReportException(exceptionMsg_);
}

}


