#include "IntactMiddleware/IntactSettings/HostSettings.h"
#include<time.h>

#ifdef USE_GCC
#include<sys/time.h>
#else
#include<sys/timeb.h>
#endif

namespace IntactMiddleware
{
// --------------------------------------------------------
//			operators
// --------------------------------------------------------
ostream& operator<<(ostream &str, const HostSettings &pub)
{
	str << pub.GetHostInformation() << endl << pub.GetConfSettings() << endl;
	return str;
}

}

