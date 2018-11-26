#include "ProcessFactory/FactoryLibrary.h"
#include "ProcessFactory/ProcessFactory.h"

#ifdef USE_GCC
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#else
#include<process.h>
#include<fcntl.h>
#include<time.h>
#include<windows.h>
#include<tlhelp32.h>
#include<vdmdbg.h>
#include<wchar.h>
#endif

using namespace std;

namespace ProcessFactoryLibrary
{
// -------------------------------------------------------
//		check if process with pid is running
// -------------------------------------------------------
int isProcessRunning(int64 pid, string executableName)
{
	//if(pid < 0) return 0;

#ifdef USE_GCC
	stringstream directory;
	directory << "/proc/" << pid ;

	ifstream procFile(((directory).str()).c_str());
	if(!procFile.is_open()) 
	{
		//cerr << "isProcessRunning(pid, name): Couldn't find process id : " << pid << endl;
		return 0;
	}
	
	// -- further check if executableName matches /proc/<pid>/cmdline
	if(!executableName.empty())
	{
		directory << "/cmdline";
		ifstream cmdFile(directory.str().c_str());
		if(cmdFile.is_open())
		{
			stringbuf cmdLine;
			while(cmdFile.get(cmdLine))
			{
				string foundExecutable = HostInformationSpace::GetExecutableFileName(cmdLine.str(), executableName.size());
				if(strncmp(executableName.c_str(), foundExecutable.c_str(), executableName.size()) == 0)
				{
					//cerr << "isProcessRunning(): Found " << executableName << " == " << foundExecutable << endl;
					return 1;
				}
			}

			//cerr << "isProcessRunning(): Could not find running instance of " << executableName << " with pid = " << pid << endl;
			return 0;
		}
		else
		{
			cerr << "isProcessRunning(): ERROR! Couldn't open /proc/" << pid << "/cmdline " << endl;
			//return 0;
		}
	}
	return 1;
#else

	int count = 0;
	HANDLE hSnap = NULL;
	PROCESSENTRY32 proc32;

	if((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
		return -1;

	proc32.dwSize=sizeof(PROCESSENTRY32);
	while((Process32Next(hSnap, &proc32)) == TRUE)
	{
		if(proc32.th32ProcessID == pid)
		{
			if(!executableName.empty())
			{
                string cmd(proc32.szExeFile);
				size_t found = cmd.find_last_of("\\");
				cmd.replace(0, found + 1, "");

				if(strcmp(executableName.c_str(), cmd.c_str()) != 0)
				{
					cerr << "No Match " << executableName << " != " << cmd << endl;
					return 0;
				}
				else count++;
			}
			else count++;

			break;
		}
	}

	CloseHandle(hSnap);

	return count;	
#endif
}

// -------------------------------------------------------
//			kill process
// -------------------------------------------------------
#if 0 //#ifndef USE_GCC
BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
	DWORD dwID;
	GetWindowThreadProcessId(hwnd, &dwID);
	if(dwID == (DWORD)lParam)
	{
		cout << "Posting message WM_CLOSE!" << endl;
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}
	return TRUE;
}
#endif

void killProcess(const HostInformation& info) 
{
	if(info.GetProcessId() == -1)
	{
		cerr << "WARNING! killProcess(processID == -1). Not Allowed!" << endl;
		return;
	}
#ifdef USE_GCC
	int sig = 15;
	int ret = kill(info.GetProcessId(), sig);
	if(ret == -1)
	{
		cerr << "killProcess(): WARNING! kill(" << info.GetProcessId() << "," << sig << ") returned " << strerror(errno) << endl;
	}

	// Remove file containing process id.
	stringstream stream;
	stream << "/var/run/sonarproc/" << info.GetProcessId() << "-" << info.GetExecutableName() << "-" << info.GetComponentName();
	remove(stream.str().c_str());
#else

#if 0
	// This is a better cleaner way to terminate a process
	HANDLE processHandle = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, true, info.GetProcessId());
	if(processHandle == NULL)
		cerr << "WARNING! Could not find processhandle for process: " << info << endl;

	EnumWindows((WNDENUMPROC)TerminateAppEnum, info.GetProcessId());

	DWORD dwRet;
	if(WaitForSingleObject(processHandle, 10000) != WAIT_OBJECT_0)
		dwRet = (TerminateProcess(processHandle, 0) ? 2:0);
	else
		dwRet = 1;
#else
	HANDLE processHandle = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, true, info.GetProcessId());
	if(processHandle == NULL)
		cerr << "WARNING! Could not find processhandle for process: " << info << endl;

	TerminateProcess(processHandle, 0);		// TODO: Is there a better and cleaner way to terminate a process?
#endif
	CloseHandle(processHandle);
#endif
}

// -------------------------------------------------------
//			create process
// -------------------------------------------------------
void createProcess(const string &commandLine, const string& title) 
{
#ifdef USE_GCC
	stringstream command;
	if(strcmp(ProcessFactoryConfig::display.c_str(), "noX") != 0) 
	{
		command << "xterm -geometry 100x35 -title " << title;
		if(ProcessFactoryConfig::display != "")
			command << " -display " << ProcessFactoryConfig::display;
		if (ProcessFactoryConfig::debugLevel > 0)
			command << " -hold";
		command << " -e ./" << commandLine << " ";
	}
    command << "./" << commandLine << " &";
    int ret = system(command.str().c_str());
	if(ret == -1)
		cout << "FactoryLibrary::createProcess(): ERROR! system call failed for : " << command.str() << endl;
#else
	STARTUPINFO startupInfo;
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.lpReserved = 0;
	startupInfo.lpDesktop = 0;  
	startupInfo.lpTitle = (LPSTR)title.c_str();  
	startupInfo.dwX = 0;  
	startupInfo.dwY = 0;  
	startupInfo.dwXSize = 0;  
	startupInfo.dwYSize = 0;  
	startupInfo.dwXCountChars = 0;  
	startupInfo.dwYCountChars = 0;  
	startupInfo.dwFillAttribute = 0;  
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;  
	startupInfo.wShowWindow = SW_SHOWMINIMIZED;  
	startupInfo.cbReserved2 = 0;  
	startupInfo.lpReserved2 = 0;  
	startupInfo.hStdInput = 0;  
	startupInfo.hStdOutput = 0;  
	startupInfo.hStdError = 0;

	PROCESS_INFORMATION processInfo;

	BOOL success = CreateProcess(
		0,							// lpApplicationName
		(LPSTR)commandLine.c_str(),		// lpCommandLine
		0,							// lpProcessAttributes
		0,							// lpThreadAttributes
		false,						// bInheritHandles
		CREATE_NEW_CONSOLE,			// dwCreationFlags
		0,							// lpEnvironment
		0,							// lpCurrentDirectory
		&startupInfo,				// lpStartupInfo
		&processInfo				// lpProcessInformation
	);

    if(!success)
        throw MiddlewareException(
            "Could not create process.  Be sure that the following executable "
            "is located in the path: " + commandLine); 

#endif
}

}; // ProcessFactoryLibrary


