#ifndef IntactObjects_ObjectSerializer_h_IsIncluded
#define IntactObjects_ObjectSerializer_h_IsIncluded

#include <string>

#include"IntactObjects/IncludeExtLibs.h"
#include"IntactObjects/Export.h"
namespace IntactObjects
{
class DLL_STATE ObjectSerializer
{
public:
	typedef std::map<std::string, MicroMiddleware::GroupInformation>		MapStringGroupInformation;
	typedef std::map<std::string, MicroMiddleware::HostInformation>			MapStringHostInformation;
	typedef std::set<MicroMiddleware::GroupHandle>							SetGroupHandle;

	static void WriteMapStringGroupInformation(NetworkLib::SerializeWriter *writer, MapStringGroupInformation &mapStringInformation);
	static void WriteMapStringHostInformation(NetworkLib::SerializeWriter *writer, MapStringHostInformation &mapStringInformation);
	static void WriteSetGroupHandle(NetworkLib::SerializeWriter *writer, SetGroupHandle &setGroupHandle);

	static void ReadMapStringGroupInformation(NetworkLib::SerializeReader *reader, MapStringGroupInformation &mapStringInformation);
	static void ReadMapStringHostInformation(NetworkLib::SerializeReader *reader, MapStringHostInformation &mapStringInformation);
	static void ReadSetGroupHandle(NetworkLib::SerializeReader *reader, SetGroupHandle &setGroupHandle);
};

} // namespace IntactObjects

#endif //


