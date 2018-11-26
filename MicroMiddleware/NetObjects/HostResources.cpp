#include "MicroMiddleware/NetObjects/HostResources.h"

namespace MicroMiddleware
{
// --------------------------------------------------------
//			operators
// --------------------------------------------------------
ostream& operator<<(ostream &str, const HostResources &hr)
{
	if(!hr.GetComponentName().empty())
		str << "-N " << hr.GetComponentName();
	if(!hr.GetHostName().empty())
		str << " -H " << hr.GetHostName();
	if(!hr.GetExecutableName().empty())
		str << " -E " << hr.GetExecutableName();

	str << " -I " << hr.GetProcessId()
		<< " -D " << hr.GetHostDescription()
		<< " -V " << hr.GetResourceView()
		<< " -C " << hr.GetCpuUsage()
		<< " -M " << hr.GetMemoryUsage()
		<< " -S " << hr.GetMapNIAmountSent() 
		<< " -R " << hr.GetMapNIAmountReceived()
		<< " -U " << hr.GetMapCPUUse()
		<< " -T " << hr.GetMapCPUTemperature()
		<< " -DT " << hr.GetMapDiskTransfers()
		<< " -DR " << hr.GetMapDiskRead()
		<< " -DW " << hr.GetMapDiskWrite()
		<< " -FD " << hr.GetNumFileDescriptors()
		<< " -Th " << hr.GetNumThreads()
		<< " -SC " << hr.GetNumSockets();

	return str;
}

ostream& operator<<(ostream &str, const HostResources::ResourceView &rv)
{
#define PRINTOUT( name ) case name: str << #name; break;
	switch(rv)
	{
		PRINTOUT(HostResources::NO_RESOURCE_VIEW)
		PRINTOUT(HostResources::PROCESS)
		PRINTOUT(HostResources::NODE)
		default:
			cerr << "HostResources::ResourceView is undefined " << endl;
			break;
	}
#undef PRINTOUT
	return str;
}

ostream& operator<<(ostream &str, const HostResources::MapStringInt32 &mapStringInt)
{
	for(HostResources::MapStringInt32::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
	{
		str << "(" << it->first << "," << it->second << ") ";
	}
	return str;
}

ostream& operator<<(ostream &str, const HostResources::MapStringInt64 &mapStringInt)
{
	for(HostResources::MapStringInt64::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
	{
		str << "(" << it->first << "," << it->second << ") ";
	}
	return str;
}

ostream& operator<<(ostream &str, const HostResources::MapStringDouble &mapStringDouble)
{
	for(HostResources::MapStringDouble::const_iterator it = mapStringDouble.begin(), it_end = mapStringDouble.end(); it != it_end; ++it)
	{
		str << "(" << it->first << "," << it->second << ") ";
	}
	return str;
}

namespace HostResourcesSpace
{
const char *ResourceViewChar[] = {
	"HostResources::NO_RESOURCE_VIEW",
	"HostResources::NODE",
	"HostResources::PROCESS",
	"NULL"
};

HostResources::ResourceView GetEnumResourceView(const char *resource)
{
	for(int i = 0; strcmp(ResourceViewChar[i], "NULL") != 0; i++)
		if(strcmp(ResourceViewChar[i], resource) == 0)
			return (HostResources::ResourceView)i;

	return (HostResources::ResourceView)0;
}

/*bool ParseHostResourcesFromString(istream &infile, vector<HostResources> &vectorHostResources)
{
	char line[4096];
	char *context = NULL;

	while(!infile.eof())
	{
		infile.getline(line, 4096);
		if(line == NULL) break;

		HostResources info;
		char *c = STRTOK_S(line, " ", &context);
		if(c == NULL) break;

		while(c != NULL)
		{
			if(strcmp(c, "-N") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetComponentName(tok);
			}
			else if(strcmp(c, "-H") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetHostName(tok);
			}
			else if(strcmp(c, "-V") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetResourceView(GetEnumResourceView(tok));
			}
			else if(strcmp(c, "-I") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetProcessId(atoi(tok));
			}
			else if(strcmp(c, "-C") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetCpuUsage(atoi(tok));
			}
			else if(strcmp(c, "-M") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetMemoryUsage(atoi(tok));
			}
			else if(strcmp(c, "-T") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				//cerr << "Execution time : " << tok << " == " << atoi(tok) << endl;
				info.SetExecutionTime(atoi(tok));
			}
			else if(strcmp(c, "-D") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetHostDescription(HostInformationSpace::GetEnumHostDescription(tok));
			}
			else if(strcmp(c, "-E") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetExecutableName(tok);
			}

			c = STRTOK_S(NULL, " ", &context);
		}		

		vectorHostResources.push_back(info);
	}

	return true;
}
*/

/*bool ParseHostResourcesFromString(istream &infile, vector<HostResources> &vectorHostResources)
{
	while(!infile.eof())
	{
		char line[4096];
		infile.getline(line, 4096);
		if(line == NULL) break;

		HostResources info;
		vector<string> vectorString;
		HostInformationSpace::TokenizeString(line, vectorString);
		
		for(size_t i = 0; i < vectorString.size(); i++)
		{
			const char *c = vectorString[i].c_str();
			if(c == NULL) break;

			const char *tok = vectorString[++i].c_str();
			if(tok == NULL) break;
			//cout << "cmd: " << c << " arg " <<  tok << endl;

			if(strcmp(c, "-N") == 0)
			{
				info.SetComponentName(HostInformationSpace::getArgument(vectorString, i, string("-H")));
			}
			else if(strcmp(c, "-H") == 0)
			{
				info.SetHostName(tok);
			}
			else if(strcmp(c, "-V") == 0)
			{
				info.SetResourceView(GetEnumResourceView(tok));
			}
			else if(strcmp(c, "-I") == 0)
			{
				info.SetProcessId(atoi(tok));
			}
			else if(strcmp(c, "-C") == 0)
			{
				info.SetCpuUsage(atoi(tok));
			}
			else if(strcmp(c, "-M") == 0)
			{
				info.SetMemoryUsage(atoi(tok));
			}
			else if(strcmp(c, "-T") == 0)
			{
				info.SetExecutionTime(atoi(tok));
			}
			else if(strcmp(c, "-D") == 0)
			{
				info.SetHostDescription(HostInformationSpace::GetEnumHostDescription(tok));
			}
			else if(strcmp(c, "-E") == 0)
			{
				info.SetExecutableName(tok);
			}
		}	

		if(!info.GetComponentName().empty())
			vectorHostResources.push_back(info);
	}
	return true;
}*/

}; // end namespace HostResourcesSpace

} // namespace end


