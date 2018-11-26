#ifndef MicroMiddleware_CommonDefines_h_IsIncluded
#define MicroMiddleware_CommonDefines_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "MicroMiddleware/Runtime.h"
#include "NetworkLib/Socket/TcpServer.h"

#ifdef USE_GCC
#include<errno.h>
#include<sys/wait.h>

//#include<sys/sysinfo.h>

#else
#include <sys/timeb.h>
#endif

#include<time.h>
#include<sys/types.h>

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<assert.h>

#ifdef USE_GCC
#define COMPONENT_FUNCTION 		"[" << BaseLib::GetTimeStamp() << "," << Runtime::GetHostInformation().GetComponentName() << ":" << __PRETTY_FUNCTION__ << "," << __LINE__ << "]: "
#define RPC_FUNCTION_STRING 	std::string(std::string(Runtime::GetHostInformation().GetComponentName()) + std::string(".") + std::string(__PRETTY_FUNCTION__))
#else
#define COMPONENT_FUNCTION 		"[" << BaseLib::GetTimeStamp() << "," << Runtime::GetHostInformation().GetComponentName() << ":" << __FUNCSIG__ << "," << __LINE__ << "]: "
#define RPC_FUNCTION_STRING 	std::string(std::string(Runtime::GetHostInformation().GetComponentName()) + std::string(".") + std::string(__FUNCSIG__))
#endif

// -> Moved to MicroMiddleware::MiddlewareSettings.h/cpp
//#define CURRENT_HOST_NAME				TcpServer::GetLocalHostName()
//#define EMPTY_STRING					"\"\""
//#define LOOKUP_SERVER_PORT			5000
//#define SYSTEM_MANAGER_PORT			8888
//#define SETTINGS_MANAGER_PORT			6789
//#define COMPONENT_FACTORY_PORT		49999
//#define EXCEPTION_MULTICAST_PORT		37778
//#define EXCEPTION_MULTICAST_ADDRESS "237.3.4.101"

#define COMPONENT_PROCESS_ID			"--COMPONENT-PROCESS-ID-->"
#define COMPONENT_MAINSTUB_PORT			"--COMPONENT-PORT-NUMBER-->"

// command line argument prefixes
#define PREFIX_LOOKUP_HOST				"--lookup-host" 
#define PREFIX_LOOKUP_PORT				"--lookup-port" 

#define PREFIX_SETTINGS_MANAGER_HOST	"--settings-manager-host" 
#define PREFIX_SETTINGS_MANAGER_PORT	"--settings-manager-port" 

#define PREFIX_INSTANCE_NAME			"--instance-name"
#define PREFIX_HOST_INFO				"--host-info"

#define ALIVE_CHECK_INTERVAL_MS			1000
#define ALIVE_CHECK_INTERVAL_SEC		1
#define REGISTER_ATTEMPTS				10

//#define DEFAULT_STATE_BIT_COUNT 		32
//#define DEFAULT_STATUS_COUNT    		32

//namespace MicroMiddleware
//{
//	namespace DCPS
//	{
//		namespace Core
//		{
//			typedef uint16_t DataRepresentationId;
//			typedef std::vector<DataRepresentationId> DataRepresentationIdSeq;
//			typedef std::vector<std::string> StringSeq;
//			typedef std::vector<uint8_t> ByteSeq;
//			const int32_t LENGTH_UNLIMITED = -1;
//		}
//	}
//}



// -- enable memory leak debugging in visual studio --> Does not really work that good...
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
// -> include this call in your code
//::_CrtDumpMemoryLeaks();

/*template<typename Elem, typename Traits = std::char_traits<Elem> >
struct basic_TeeStream : std::basic_ostream<Elem,Traits>
{
   typedef std::basic_ostream<Elem,Traits> SuperType;

   basic_TeeStream(std::ostream& o1, std::ostream& o2) 
      :  SuperType(o1.rdbuf()), o1_(o1), o2_(o2) { }

   basic_TeeStream& operator<<(SuperType& (__cdecl *manip)(SuperType& ))
   {
      o1_ << manip;
      o2_ << manip;
      return *this;
   }
   
   template<typename T>
   basic_TeeStream& operator<<(const T& t)
   {
      o1_ << t;
      o2_ << t;
      return *this;
   }
   
private:
   std::ostream& o1_;
   std::ostream& o2_;
};*/

#endif // COMMON_DEFINES_H

