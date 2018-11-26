#include "MicroMiddleware/NetObjects/HostInformation.h"
#include "MicroMiddleware/CommonDefines.h"

#include<time.h>
#ifdef USE_GCC
#include<sys/time.h>
#endif

namespace MicroMiddleware
{

// --------------------------------------------------------
//			operators
// --------------------------------------------------------
ostream& operator<<(ostream &str, const HostInformation &pub)
{
	if(!pub.GetComponentName().empty())		str << "-N " <<	pub.GetComponentName();
	if(!pub.GetHostName().empty())			str << " -L " << pub.GetHostName();
	if(!pub.GetHostIP().empty())			str << " -H " << pub.GetHostIP();
	if(!pub.GetMulticastAddress().empty())	str	<< " -M " << pub.GetMulticastAddress();
	if(!pub.GetExecutableName().empty())	str << " -E " << pub.GetExecutableName();
	
	str << " -P " << pub.GetPortNumber()
		<< " -D " << pub.GetHostDescription()
		<< " -O " << pub.GetOnlineStatus()
		<< " -S " << pub.GetStartDescription()
		<< " -I " << pub.GetProcessId()
		<< " -V " << pub.GetComponentId();

	if(!pub.GetArgumentList().empty()) 		str << " -A " << pub.GetArgumentList();

	return str;
}

ostream& operator<<(ostream &str, const HostInformation::HostDescription &hd)
{
#define PRINTOUT( name ) case name: str << #name; break;
	switch(hd)
	{
		PRINTOUT(HostInformation::NO_HOST_DESCRIPTION)
		PRINTOUT(HostInformation::COMPONENT_FACTORY)
		PRINTOUT(HostInformation::LOOKUP_SERVER)
		PRINTOUT(HostInformation::SYSTEM_MANAGER)
		PRINTOUT(HostInformation::SETTINGS_MANAGER)
		PRINTOUT(HostInformation::COMPONENT)
		PRINTOUT(HostInformation::PUBLISHER)
		PRINTOUT(HostInformation::SUBSCRIBER)
		PRINTOUT(HostInformation::PROXY)
		PRINTOUT(HostInformation::STUB)
		default:
			cerr << "HostDescription is undefined " << endl;
			break;
	}
#undef PRINTOUT
	return str;
}

ostream& operator<<(ostream &str, const HostInformation::OnlineStatus &stat)
{
#define PRINTOUT( name ) case name: str << #name; break;
	switch(stat)
	{
		PRINTOUT(HostInformation::NO_ONLINE_STATUS)
		PRINTOUT(HostInformation::OFFLINE)
		PRINTOUT(HostInformation::ONLINE)
		default:
			cerr << "OnlineStatus is undefined " << endl;
			break;
	}
#undef PRINTOUT
	return str;
}

ostream& operator<<(ostream &str, const HostInformation::StartDescription &start)
{
#define PRINTOUT( name ) case name: str << #name; break;
	switch(start)
	{
		PRINTOUT(HostInformation::NO_START_DESCRIPTION)
		PRINTOUT(HostInformation::STOP)
		PRINTOUT(HostInformation::START)
		PRINTOUT(HostInformation::RESTART)
		default:
			cerr << "StartDescription is undefined " << endl;
			break;
	}
#undef PRINTOUT
	return str;
}


// --------------------------------------------------------
//			HostInformationSpace
// --------------------------------------------------------
namespace HostInformationSpace
{
const char *OnlineStatusChar[] = {
		"HostInformation::NO_ONLINE_STATUS",
		"HostInformation::OFFLINE", 
		"HostInformation::ONLINE",
		"NULL"	// NB! always last
	};
const char *HostDescriptionChar[] = { 
		"HostInformation::NO_HOST_DESCRIPTION",
		"HostInformation::COMPONENT_FACTORY",
		"HostInformation::LOOKUP_SERVER",
		"HostInformation::SYSTEM_MANAGER",
		"HostInformation::SETTINGS_MANAGER",
		"HostInformation::COMPONENT",
		"HostInformation::PUBLISHER",
		"HostInformation::SUBSCRIBER",
		"HostInformation::PROXY",
		"HostInformation::STUB",
		"NULL" // NB! always last
	};
const char *StartDescriptionChar[] = {
	"HostInformation::NO_START_DESCRIPTION",
	"HostInformation::STOP",
	"HostInformation::START",
	"HostInformation::RESTART",
	"NULL" // NB! always last
	};

HostInformation::HostDescription GetEnumHostDescription(const char *description)
{
	for(int i = 0; strcmp(HostDescriptionChar[i], "NULL") != 0; i++)
		if(strcmp(HostDescriptionChar[i], description) == 0)
			return (HostInformation::HostDescription)i;
	
	return (HostInformation::HostDescription)0;
}

HostInformation::OnlineStatus GetEnumOnlineStatus(const char *status)
{
	for(int i = 0; strcmp(OnlineStatusChar[i], "NULL") != 0; i++)
		if(strcmp(OnlineStatusChar[i], status) == 0)
			return (HostInformation::OnlineStatus)i;

	return (HostInformation::OnlineStatus)0;
}

HostInformation::StartDescription GetEnumStartDescription(const char *description)
{
	for(int i = 0; strcmp(StartDescriptionChar[i], "NULL") != 0; i++)
		if(strcmp(StartDescriptionChar[i], description) == 0)
			return (HostInformation::StartDescription)i;

	return (HostInformation::StartDescription)0;
}

// --------------------------------------------------------
//			write/read host information to/from disc
// --------------------------------------------------------
bool WriteHostInformationToDisc(ofstream &outfile, HostInformation info)
{
	if(!outfile.is_open()) 
	{
		//IWARNING()  << "ERROR! Out-file stream is not open" << endl;
		return false;
	}

	if(info.GetOnlineStatus() == HostInformation::ONLINE &&
		info.GetStartDescription() == HostInformation::START)
	{
		info.SetStartDescription(HostInformation::STOP);
	}

	stringstream ss;
	ss << info << endl;

	outfile.write((ss.str()).c_str(), (ss.str()).size());
	outfile.flush();

	return true;
}

bool WriteHostInformationToDisc(string filename, HostInformation info)
{
	ofstream outfile(filename.c_str(), ios::out);
	if(!outfile.is_open()) 
		return false;

	return WriteHostInformationToDisc(outfile, info);
}

bool ParseHostInformationFromDisc(string filename, vector<HostInformation> &vectorHostInformation)
{
	ifstream infile(filename.c_str(), ios::in);
	if(!infile.is_open()) 
		return false;

	return ParseHostInformationFromDisc(infile, vectorHostInformation);
}

/*string getArgument(char *&context, string nextSubstr)
{
	stringstream argumentString;
	char *tok = STRTOK_S(NULL, " ", &context);
	while(tok != NULL)
	{
		if(strcmp(tok, EMPTY_STRING) == 0) break;
		if(strcmp(tok,"\n") == 0) break;
		if(!nextSubstr.empty() && strcmp(nextSubstr.c_str(), tok) == 0) break;

		cout << tok << " ";
		if(argumentString.str().empty())
			argumentString << tok;
		else
			argumentString << " " << tok;

		tok = STRTOK_S(NULL, " ", &context);
	}
	return argumentString.str();
}

bool ParseHostInformationFromDisc(istream &infile, vector<HostInformation> &vectorHostInformation)
{
	char line[4096];
	char *context = NULL;

	while(!infile.eof())
	{
		infile.getline(line, 4096);
		if(line == NULL) break;

		HostInformation info;
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
			else if(strcmp(c, "-M") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetMulticastAddress(tok);
			}
			else if(strcmp(c, "-P") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetPortNumber(atoi(tok));
			}
			else if(strcmp(c, "-L") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetHostName(tok);
			}
			else if(strcmp(c, "-H") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetHostIP(tok);
			}
			else if(strcmp(c, "-D") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetHostDescription(GetEnumHostDescription(tok));
			}
			else if(strcmp(c, "-O") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetOnlineStatus(GetEnumOnlineStatus(tok));
			}
			else if(strcmp(c, "-S") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetStartDescription(GetEnumStartDescription(tok));
			}			
			else if(strcmp(c, "-I") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				info.SetProcessId(atoi(tok));
			}
			else if(strcmp(c, "-E") == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				if(strcmp(tok, EMPTY_STRING) != 0)
					info.SetExecutableName(tok);
			}
			else if(strcmp(c, "-A") == 0)
			{
				info.SetArgumentList(getArgument(context, string("")));
			}

			c = STRTOK_S(NULL, " ", &context);
		}		

		vectorHostInformation.push_back(info);
	}

	return true;
}*/

void TokenizeString(const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

string getArgument(const vector<string> &vectorString, size_t &i, string nextSubstr)
{
	stringstream argumentString;
	
	//cerr << "Getting argument : " << endl;
	for( ; i < vectorString.size(); i++)
	{
		string tok = vectorString[i];

		//if(strcmp(tok.c_str(), EMPTY_STRING) == 0) break;
		if(strcmp(tok.c_str(), "\n") == 0) break;
		if(!nextSubstr.empty() && strcmp(nextSubstr.c_str(), tok.c_str()) == 0) 
		{
			//cerr << "Found next substring ! " << nextSubstr << " == " << tok << endl;
			--i;
			break;
		}

		//cerr << tok << " " ;

		if(argumentString.str().empty())
			argumentString << tok;
		else
			argumentString << " " << tok;
	}
	return argumentString.str();
}

bool ParseHostInformationFromDisc(istream &infile, vector<HostInformation> &vectorHostInformation)
{
	while(!infile.eof())
	{
		string line;
		getline(infile, line);
		if(line.size() == 0) continue;

		//cout << "Line: " << line << endl;
		HostInformation info;
		vector<string> vectorString;
		TokenizeString(line, vectorString, " ");
		
		for(size_t i = 0; i < vectorString.size(); i++)
		{
			const char *c = vectorString[i].c_str();
			if(c == NULL) break;

			if(vectorString.size() < i+1) break;

			const char *tok = vectorString[++i].c_str();
			if(tok == NULL) break;
			//cout << "cmd: " << c << " arg " <<  tok << endl;

			if(strcmp(c, "-N") == 0)
			{
				info.SetComponentName(getArgument(vectorString, i, string("-L")));
			}
			else if(strcmp(c, "-M") == 0)
			{
				info.SetMulticastAddress(tok);
			}
			else if(strcmp(c, "-P") == 0)
			{
				info.SetPortNumber(atoi(tok));
			}
			else if(strcmp(c, "-L") == 0)
			{
				info.SetHostName(tok);
			}
			else if(strcmp(c, "-H") == 0)
			{
				info.SetHostIP(tok);
			}
			else if(strcmp(c, "-D") == 0)
			{
				info.SetHostDescription(GetEnumHostDescription(tok));
			}
			else if(strcmp(c, "-O") == 0)
			{
				info.SetOnlineStatus(GetEnumOnlineStatus(tok));
			}
			else if(strcmp(c, "-S") == 0)
			{
				info.SetStartDescription(GetEnumStartDescription(tok));
			}			
			else if(strcmp(c, "-I") == 0)
			{
				info.SetProcessId(atoi(tok));
			}
			else if(strcmp(c, "-E") == 0)
			{
				info.SetExecutableName(tok);
			}
			else if(strcmp(c, "-V") == 0)
			{
				info.SetComponentId(atoi(tok));
			}
			else if(strcmp(c, "-A") == 0)
			{
				info.SetArgumentList(getArgument(vectorString, i, string("")));
			}
		}

		if(!info.GetComponentName().empty())
			vectorHostInformation.push_back(info);
	}

	return true;
}

string GetExecutableFileName(string cmd, int executableFileNameSize)
{
	size_t found_host_info = cmd.rfind(PREFIX_HOST_INFO);
	string substr = cmd.substr(found_host_info + sizeof(PREFIX_HOST_INFO), cmd.size() - found_host_info);
	
	size_t found = substr.rfind("-E");
	string execFile = substr.substr(found + sizeof("-E"), executableFileNameSize);
	
	return execFile;
}

} // namespace HostInformationSpace

}

