#ifndef BaseLib_Exception_h_Included
#define BaseLib_Exception_h_Included

#include "BaseLib/CommonDefines.h"

#ifdef USE_GCC
#include <execinfo.h>
#endif

#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include"BaseLib/Export.h"
namespace BaseLib
{

/*-----------------------------------------------------------------------
    ExceptionTracer class
        -> Exception classes inherit from this class
        -> The constructor creates a stack trace when en exception is thrown
------------------------------------------------------------------------- */
class DLL_STATE ExceptionTracer
{
public:
    ExceptionTracer();
    virtual ~ExceptionTracer();

    virtual std::string GetExceptionTrace() const throw();

private:
    std::string exceptionTrace_;
};

/*-----------------------------------------------------------------------
                ExceptionBase class
------------------------------------------------------------------------- */
class DLL_STATE ExceptionBase //: ObjectBase
{
public:
    enum class DLL_STATE ExceptionType
    {
        GeneralException = 0,
        Exception,
        IOException,
        ErrorException,
        WarningException,
        CriticalException,
        FatalException,
        DeadlockException,
        CancelException,
        InterruptedException,
        UnsupportedException
    };

public:
    ExceptionBase(ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::GeneralException);
    virtual ~ExceptionBase() throw();

    virtual ExceptionBase::ExceptionType GetExceptionType() const throw();

private:
    ExceptionType exceptionType_;
};

DLL_STATE std::ostream& operator<<(std::ostream &str, const ExceptionBase::ExceptionType &type) throw();

/*-----------------------------------------------------------------------
                General Exception class
------------------------------------------------------------------------- */
class DLL_STATE GeneralException
        : public std::runtime_error
        , public ExceptionTracer
        , public ExceptionBase
{
public:
    GeneralException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::GeneralException);
    virtual ~GeneralException() throw();

    std::string msg() const throw();
    virtual const char* what() const throw();


    friend std::ostream& operator<<(std::ostream& ostr, const GeneralException &e)
    {
        ostr << e.GetExceptionType() << ": " << e.exceptionMsg_ << std::endl << "Trace:" << std::endl << e.GetExceptionTrace();
        return ostr;
    }

protected:
    std::string exceptionMsg_;
};

/*-----------------------------------------------------------------------
            specialized exception classes
------------------------------------------------------------------------- */
class DLL_STATE Exception : public GeneralException
{
public:
    Exception(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::Exception);
};

class DLL_STATE IOException : public GeneralException
{
public:
    IOException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::IOException);
};

class DLL_STATE ErrorException : public GeneralException
{
public:
    ErrorException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::ErrorException);
};

class DLL_STATE WarningException : public GeneralException
{
public:
    WarningException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::WarningException);
};

class DLL_STATE CriticalException : public GeneralException
{
public:
    CriticalException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::CriticalException);
};

class DLL_STATE FatalException : public GeneralException
{
public:
    FatalException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::FatalException);
};

class DLL_STATE DeadlockException : public GeneralException
{
public:
    DeadlockException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::DeadlockException);
};

class DLL_STATE CancelException : public GeneralException
{
public:
    CancelException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::CancelException);
};

class DLL_STATE InterruptedException : public GeneralException
{
public:
    InterruptedException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::InterruptedException);
};

class DLL_STATE UnsupportedException : public GeneralException
{
public:
    UnsupportedException(std::string txt, ExceptionBase::ExceptionType type = ExceptionBase::ExceptionType::UnsupportedException);
};

/*
class AlreadyClosedError : public GeneralException, public std::logic_error
{
public:
    explicit AlreadyClosedError(const std::string& msg);
    AlreadyClosedError(const AlreadyClosedError& src);
    virtual ~AlreadyClosedError() throw ();

public:
    virtual const char* what() const throw ();
};


class IllegalOperationError : public GeneralException, public std::logic_error
{
public:
    explicit IllegalOperationError(const std::string& msg);
    IllegalOperationError(const IllegalOperationError& src);
    virtual ~IllegalOperationError() throw ();

public:
    virtual const char* what() const throw ();
};


class ImmutablePolicyError : public GeneralException, public std::logic_error
{
public:
    explicit ImmutablePolicyError(const std::string& msg);
    ImmutablePolicyError(const ImmutablePolicyError& src);
    virtual ~ImmutablePolicyError() throw ();

public:
    virtual const char* what() const throw ();
};


class InconsistentPolicyError : public GeneralException, public std::logic_error
{
public:
    explicit InconsistentPolicyError(const std::string& msg);
    InconsistentPolicyError(const InconsistentPolicyError& src);
    virtual ~InconsistentPolicyError() throw ();

public:
    virtual const char* what() const throw ();
};


class InvalidArgumentError : public GeneralException, public std::invalid_argument
{
public:
    explicit InvalidArgumentError(const std::string& msg);
    InvalidArgumentError(const InvalidArgumentError& src);
    virtual ~InvalidArgumentError() throw ();

public:
    virtual const char* what() const throw ();
};


class NotEnabledError : public GeneralException, public std::logic_error
{
public:
    explicit NotEnabledError(const std::string& msg);
    NotEnabledError(const NotEnabledError& src);
    virtual ~NotEnabledError() throw ();

public:
    virtual const char* what() const throw ();
};


class OutOfResourcesError : public GeneralException, public std::runtime_error
{
public:
    explicit OutOfResourcesError(const std::string& msg);
    OutOfResourcesError(const OutOfResourcesError& src);
    virtual ~OutOfResourcesError() throw ();

public:
    virtual const char* what() const throw ();
};


class PreconditionNotMetError : public GeneralException, public std::logic_error
{
public:
    explicit PreconditionNotMetError(const std::string& msg);
    PreconditionNotMetError(const PreconditionNotMetError& src);
    virtual ~PreconditionNotMetError() throw ();

public:
    virtual const char* what() const throw ();
};


class TimeoutError : public GeneralException, public std::runtime_error
{
public:
    explicit TimeoutError(const std::string& msg);
    TimeoutError(const TimeoutError& src);
    virtual ~TimeoutError() throw ();

public:
    virtual const char* what() const throw ();
};


class UnsupportedError : public GeneralException, public std::logic_error
{
public:
    explicit UnsupportedError(const std::string& msg);
    UnsupportedError(const UnsupportedError& src);
    virtual ~UnsupportedError() throw ();

public:
    virtual const char* what() const throw ();
};

class InvalidDowncastError : public GeneralException, public std::runtime_error
{
public:
    explicit InvalidDowncastError(const std::string& msg);
    InvalidDowncastError(const InvalidDowncastError& src);
    virtual ~InvalidDowncastError() throw();

public:
    virtual const char* what() const throw ();
};

class NullReferenceError : public GeneralException, public std::runtime_error
{
public:
    explicit NullReferenceError(const std::string& msg);
    NullReferenceError(const NullReferenceError& src);
    virtual ~NullReferenceError() throw();

public:
    virtual const char* what() const throw ();
};


class InvalidDataError : public GeneralException, public std::logic_error
{
public:
    explicit InvalidDataError(const std::string& msg);
    InvalidDataError(const UnsupportedError& src);
    virtual ~InvalidDataError() throw ();

public:
    virtual const char* what() const throw ();
};*/

/*-----------------------------------------------------------------------
        Handling signal exceptions
------------------------------------------------------------------------- */
/*template <class SignalExceptionClass> class SignalTranslator
{
    private:
        class SingleTonTranslator
        {
            public:
                SingleTonTranslator()
                {
                    signal(SignalExceptionClass::GetSignalNumber(), SignalHandler);
                }

                static void SignalHandler(int)
                {
                    throw SignalExceptionClass();
                }
        };

    public:
        SignalTranslator()
        {
            static SingleTonTranslator s_objTranslator;
        }
};

// An example for SIGSEGV
class SegmentationFault : public ExceptionTracer, public exception
{
    public:
        static int GetSignalNumber() {return SIGSEGV;}
};

SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;

// An example for SIGFPE
class FloatingPointException : public ExceptionTracer, public exception
{
    public:
        static int GetSignalNumber() {return SIGFPE;}
};

SignalTranslator<FloatingPointException> g_objFloatingPointExceptionTranslator;

//Listing 3. Handling exceptions in a constructor
 // Before defining any global variable, we define a dummy instance
 // of ExceptionHandler object to make sure that
 // ExceptionHandler::SingleTonHandler::SingleTonHandler() is invoked
 //ExceptionHandler g_objExceptionHandler;
 //A g_a;
class ExceptionHandler
{
private:
    class SingleTonHandler
    {
    public:
        SingleTonHandler()
        {
            set_terminate(Handler);
        }

        static void Handler()
        {
            // Exception from construction/destruction of global variables
            try
            {
                // re-throw
                throw;
            }
            catch (SegmentationFault &)
            {
                IDEBUG() << "SegmentationFault" ;
            }
            catch (FloatingPointException &)
            {
                IDEBUG() << "FloatingPointException" ;
            }
            catch (...)
            {
                IDEBUG() << "Unknown Exception" ;
            }

            //if this is a thread performing some core activity
            abort();
            // else if this is a thread used to service requests
            // pthread_exit();
        }
    };

public:
    ExceptionHandler()
    {
        static SingleTonHandler s_objHandler;
    }
};*/

}

#endif // BaseLib_Exception_h_Included

