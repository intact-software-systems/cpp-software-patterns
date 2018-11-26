#ifndef BaseLib_Debug_h_Included
#define BaseLib_Debug_h_Included

#include <sstream>
#include <stdio.h>
#include <cstdarg>
#include <iostream>
#include <stdint.h>

#include "BaseLib/LogDebug.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/Export.h"
namespace BaseLib
{

class DLL_STATE IDebug
{
private:
	struct Stream 
	{
        Stream(LogDebugBase::LogType t, std::string append = std::string())
			: ref(1)
			, type(t)
			, space(true)
			, message_output(true) 
		{
            ts << append << type << ": ";
        }
		
		std::stringstream		ts;
		int 					ref;
		LogDebugBase::LogType	type;
		bool 					space;
		bool 					message_output;
	};

public:
    inline IDebug(LogDebugBase::LogType type, std::string append = std::string()) : stream_(new Stream(type, append))
	{}
    inline IDebug(const IDebug &o)
		: stream_(o.stream_) 
	{ 
		++stream_->ref; 
	}
    inline IDebug &operator=(const IDebug &other);
    inline ~IDebug() 
	{
        if(!--stream_->ref) 
		{
            if(stream_->message_output) 
			{
                try
				{
					LogDebugBase::LogMessage(stream_->type, stream_->ts.str().c_str());
                } 
				catch(std::bad_alloc&) 
				{ 
					// We're out of memory - give up.
				}
            }
            delete stream_;
        }
    }

    inline IDebug &space() 			{ stream_->space = true; stream_->ts << ' '; return *this; }
    inline IDebug &nospace() 		{ stream_->space = false; return *this; }
    inline IDebug &maybeSpace() 	{ if (stream_->space) stream_->ts << ' '; return *this; }


    inline IDebug &operator<<(bool t) 				{ stream_->ts << (t ? "true" : "false"); return maybeSpace(); }
    inline IDebug &operator<<(char t) 				{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(signed short t) 		{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(unsigned short t) 	{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(signed int t) 		{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(unsigned int t) 		{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(signed long t) 		{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(unsigned long t)      { stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(signed long long t)   { stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(unsigned long long t) { stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(float t) 				{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(double t) 			{ stream_->ts << t; return maybeSpace(); }
    inline IDebug &operator<<(const char* t) 		{ stream_->ts << t; return maybeSpace(); }
    //inline IDebug &operator<<(const QString & t) 	{ stream_->ts << '\"' << t  << '\"'; return maybeSpace(); }
    //inline IDebug &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline IDebug &operator<<(const void * t) 		{ stream_->ts << t; return maybeSpace(); }

    inline IDebug &operator<<(const std::string &t)	{ stream_->ts << t; return maybeSpace(); }
    //inline IDebug &operator<<(const std::ostream &ostr)	{ stream_->ts << ostr; return maybeSpace(); }
    //inline IDebug &operator<<(const std::basic_ostream<unsigned short, std::char_traits<unsigned short> > &e) { stream_->ts << e; return maybeSpace(); }
    //inline IDebug &operator<<(std::basic_ostream<wchar_t> &e) { stream_->ts << e; return maybeSpace(); }
    //inline IDebug &operator<<(const std::basic_ostream<char> &e) { stream_->ts << e; return maybeSpace(); }

    template <class T>
    inline IDebug &operator<<(const T &t) { stream_->ts << t; return maybeSpace(); }

private:
	Stream *stream_;
};

class DLL_STATE INoDebug
{
public:
    inline INoDebug(){}
    inline INoDebug(const IDebug &){}
    inline ~INoDebug(){}
    
	inline INoDebug &space() 		{ return *this; }
    inline INoDebug &nospace() 		{ return *this; }
    inline INoDebug &maybeSpace() 	{ return *this; }

    template<typename T>
    inline INoDebug &operator<<(const T &) { return *this; }
};

inline IDebug &IDebug::operator=(const IDebug &other)
{
    if (this != &other) 
	{
        IDebug copy(other);
        std::swap(stream_, copy.stream_);
    }
    return *this;
}

#ifndef INTACT_NO_DEBUG_OUTPUT

    DLL_STATE IDebug iInfo(std::string append = std::string(""));
    DLL_STATE IDebug iDebug(std::string append = std::string(""));
    DLL_STATE IDebug iWarning(std::string append = std::string(""));
    DLL_STATE IDebug iCritical(std::string append = std::string(""));
    DLL_STATE IDebug iFatal(std::string append = std::string(""));

    #define APPENDLOG TSPRETTY_FUNCTION

    #define IINFO()			iInfo(APPENDLOG)
    #define IDEBUG() 		iDebug(APPENDLOG)
    #define IWARNING() 		iWarning(APPENDLOG)
    #define ICRITICAL() 	iCritical(APPENDLOG)
    #define IFATAL() 		iFatal(APPENDLOG)

#else

    DLL_STATE INoDebug iInfo(std::string append = std::string());
    DLL_STATE INoDebug iDebug(std::string append = std::string());
    DLL_STATE INoDebug iWarning(std::string append = std::string());
    DLL_STATE INoDebug iCritical(std::string append = std::string());
    DLL_STATE INoDebug iFatal(std::string append = std::string());

    #define IINFO()			iInfo()
    #define IDEBUG() 		iDebug()
    #define IWARNING() 		iWarning()
    #define ICRITICAL() 	iCritical()
    #define IFATAL() 		iFatal()

#endif


} // namespace BaseLib

#endif // BaseLib_Debug_h_Included


