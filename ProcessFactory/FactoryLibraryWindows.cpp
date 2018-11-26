#include "ProcessFactory/FactoryLibrary.h"
#include "ProcessFactory/ProcessFactory.h"
#include "BaseLib/CommonDefines.h"

#ifndef USE_GCC
#include<process.h>
#include<fcntl.h>
#include<time.h>
#include<windows.h>
#include<tlhelp32.h>
#include<stdio.h>
#include<psapi.h>
#include<io.h>
#endif

using namespace std;

namespace ProcessFactoryLibrary {
namespace Windows 
{
#ifndef USE_GCC
// -------------------------------------------------------
//	check if process is running
// -------------------------------------------------------
bool isProcessRunning(HANDLE hProcess)
{
	DWORD lpExitCode;
	if(GetExitCodeProcess(hProcess, &lpExitCode))
		if(lpExitCode != STILL_ACTIVE) 		// Process is not running!
			return false;
		else 
			return true;
	return true;
}

size_t getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}

// -------------------------------------------------------
//	read data from pipe functions
// -------------------------------------------------------
void readDataFromPipe(HANDLE hChildStdoutRdDup, int *pid, int *port)
{
	char *context = NULL;
	int processId = -1;
	int portNumber = -1;

	while(true)
	{
		char line[4096];
		int64 bytesRead;
		int ret = ReadFile(hChildStdoutRdDup, line, 4096, &bytesRead, NULL);
		if(!ret || bytesRead == 0)
		{
			DWORD err = GetLastError();
			if(err == ERROR_BROKEN_PIPE)
				cerr << "ERROR! Pipe is broken!" << endl;

			cerr << "Could not read any more bytes from child process " << bytesRead << endl;
			break;
		}
		
		char *c = STRTOK_S(line, " ", &context);
		if(c == NULL) continue;

		while(c != NULL)
		{
			if(strcmp(c, COMPONENT_MAINSTUB_PORT) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				portNumber = atoi(tok);
				cerr << "FOUND PORTNUMBER : " << portNumber << endl;
				break;
			}
			else if(strcmp(c, COMPONENT_PROCESS_ID) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				processId = atoi(tok);
				cerr << "Found processId : " << processId << endl;
				break;
			}

			c = STRTOK_S(NULL, " ", &context);
		}

		if(portNumber > -1 && processId > -1) break;
	}

	if(pid != NULL) *pid = processId;
	if(port != NULL) *port = portNumber;
}

void readDataFromPipe(FILE *file, int *pid, int *port)
{
	// -- data is available --
	int processId = -1;
	int portNumber = -1;
	char line[4098];
	char *context = NULL;
	while(fgets(line, sizeof(line), file))
	{
		if(line == NULL) continue;

		char *c = STRTOK_S(line, " ", &context);
		if(c == NULL) continue;

		while(c != NULL)
		{
			if(strcmp(c, COMPONENT_MAINSTUB_PORT) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				portNumber = atoi(tok);
				cerr << "Found port : " << portNumber << endl;
				break;
			}
			else if(strcmp(c, COMPONENT_PROCESS_ID) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				processId = atoi(tok);
				cerr << "Found processId : " << processId << endl;
				break;
			}

			c = STRTOK_S(NULL, " ", &context);
		}
		
		if(portNumber > -1 && processId > -1) break;
	}

	if(pid != NULL) *pid = processId;
	if(port != NULL) *port = portNumber;
}

void readDataFromPipe(int fdStdOutPipe, int *pid, int *port)
{
	// -- read data from pipe --
	int processId = -1;
	int portNumber = -1;
	char line[4098];
	char *context = NULL;
	while(_read(fdStdOutPipe, line, 4098))
	{
		if(line == NULL) continue;

		char *c = STRTOK_S(line, " ", &context);
		if(c == NULL) continue;

		while(c != NULL)
		{
			if(strcmp(c, COMPONENT_MAINSTUB_PORT) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				portNumber = atoi(tok);
				cerr << "Found port : " << portNumber << endl;
				break;
			}
			else if(strcmp(c, COMPONENT_PROCESS_ID) == 0)
			{
				char *tok = STRTOK_S(NULL, " ", &context);
				processId = atoi(tok);
				cerr << "Found processId : " << processId << endl;
				break;
			}

			c = STRTOK_S(NULL, " ", &context);
		}
		
		if(portNumber > -1 && processId > -1) break;
	}

	if(pid != NULL) *pid = processId;
	if(port != NULL) *port = portNumber; 
}

// -------------------------------------------------------
//			create process for MS Windows functions
// -------------------------------------------------------
int createProcess(const string &commandLine, const string& title) 
{
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

	int processId = (int)processInfo.dwProcessId;

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	return processId;
}

int createProcessSpawnPipe(const char *executable, char *const args[], int *stdOutPipe)
{
	int nExitCode = STILL_ACTIVE;
	HANDLE hProcess;
	int fdStdOut;
	int fdStdOutPipe[2];

	if(_pipe(fdStdOutPipe, 512, O_NOINHERIT) == -1) 			// Create the pipe
		return -1;
	
	fdStdOut = _dup(_fileno(stdout)); 							// Duplicate stdout file descriptor (next line will close original)
	if(_dup2(fdStdOutPipe[1], _fileno(stdout)) != 0) 			// Duplicate write end of pipe to stdout file descriptor
		return -1;
	_close(fdStdOutPipe[1]); 									// Close original write end of pipe

	hProcess = (HANDLE)_spawnvp(P_NOWAIT, executable, args); 	// Spawn process

	if(_dup2(fdStdOut, _fileno(stdout)) != 0)					// Duplicate copy of original stdout back into stdout
		return -1;
	_close(fdStdOut);											// Close duplicate copy of original stdout

	if(!hProcess) return -1;

	if(stdOutPipe != NULL) *stdOutPipe = fdStdOutPipe[0];

	return 1;
}

/*int createProcessSpawnPipe(const char *executable, char *const args[])
{
    int pid = -1;
#ifndef USE_GCC
	int fdpipe[2];
	
	setvbuf(stdout, NULL, _IONBF, 0);

    if (_pipe(fdpipe, 4096, O_BINARY) != -1)
        return -1;

	if( (pid = _spawnv(P_NOWAIT, executable, args) ) == -1)
		return -1;
#endif
    return pid;
}*/

int createProcessPopen(const string &commandLine, const string &title, FILE *file)
{
	if(file == NULL) return -1;

	file = _popen(commandLine.c_str(), "rt");
	if(file == NULL)
	{
		cerr << "ERROR: " << GetLastError() << " _popen() failed " << commandLine << endl;
		return -1;
	}
	return 1;
}

int createProcessPipe(const string &commandLine, const string& title, HANDLE &hChildStdoutRdDup, HANDLE &hChildStdinWrDup)
{
	HANDLE hChildStd_OUT_Wr = NULL;
	HANDLE hChildStd_OUT_Rd = NULL;
	HANDLE hSaveStdOut = NULL;
	//HANDLE hChildStdoutRdDup = NULL;
	
	HANDLE hChildStd_IN_Wr = NULL;
	HANDLE hChildStd_IN_Rd = NULL;
	HANDLE hSaveStdIn = NULL;
	//HANDLE hChildStdinWrDup = NULL;
		
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// 1. save current STDOUT, to be restored
	// 2. create anonymous pipe to be STDOUT for child process
	// 3. set STDOUT of the parent process to be write handle to the pipe, so it is inherited by the child
	// 4. Create a noninheritable duplicate of the write handle and close the inheritable write handle
	hSaveStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if( ! CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
		throw MiddlewareException("Could not create a pipe for the child process's STDOUT");

	if( ! SetStdHandle(STD_OUTPUT_HANDLE, hChildStd_OUT_Wr))
		throw MiddlewareException("Could not redirect child STDOUT");

	BOOL fSuccess = DuplicateHandle(GetCurrentProcess(), 
			hChildStd_OUT_Rd, GetCurrentProcess(), 
			&hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if(!fSuccess) throw MiddlewareException("DuplicateHandle failed");
	CloseHandle(hChildStd_OUT_Rd);

	// redirect child process's STDIN
	// save current STDIN
	hSaveStdIn = GetStdHandle(STD_INPUT_HANDLE);
	if( ! CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
		throw MiddlewareException("Could not create a pipe for the child process's STDIN");
	if( ! SetStdHandle(STD_INPUT_HANDLE, hChildStd_IN_Rd))
		throw MiddlewareException("Could not redirect child STDIN");
	
	fSuccess = DuplicateHandle(GetCurrentProcess(), 
			hChildStd_IN_Wr, GetCurrentProcess(), 
			&hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if(!fSuccess) throw MiddlewareException("DuplicateHandle failed");
	CloseHandle(hChildStd_IN_Wr);

	// -- Start CREATE Child PROCESS --
	STARTUPINFO startupInfo;
	ZeroMemory( &startupInfo, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.lpTitle = (LPSTR)title.c_str(); 
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;// | STARTF_USESHOWWINDOW;
	startupInfo.hStdOutput = hChildStd_OUT_Wr;  // TODO: Add pipe here!
	startupInfo.hStdError = hChildStd_OUT_Wr;

	PROCESS_INFORMATION processInfo;

	BOOL success = CreateProcess(
		NULL,						// lpApplicationName
		(LPSTR)commandLine.c_str(),	// lpCommandLine
		NULL,						// lpProcessAttributes
		NULL,						// lpThreadAttributes
		TRUE,						// bInheritHandles
		CREATE_NEW_CONSOLE,			// DETACHED_PROCESS dwCreationFlags
		NULL,						// lpEnvironment
		NULL,						// lpCurrentDirectory
		&startupInfo,				// lpStartupInfo
		&processInfo				// lpProcessInformation
	);

    if(!success) throw MiddlewareException("Could not create process. Be sure that the following executable is located in the path: " + commandLine); 
	
	int processId = (int)processInfo.dwProcessId;

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	// -- END CREATE Child PROCESS --

	// Restore the saved STDOUT
	if( ! SetStdHandle(STD_INPUT_HANDLE, hSaveStdIn) )
		throw MiddlewareException("Re-direnting Stdin failed");
	if( ! SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdOut) )
		throw MiddlewareException("Re-direnting Stdout failed");

	// -- read from child stdout pipe --
	// close the write end of the pipe before reading from the read end of the pipe
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if(!CloseHandle(hChildStd_OUT_Wr))
		throw MiddlewareException("Could not close hChildStd_OUT_Wr handle ");

	//CloseHandle(hChildStd_OUT_Rd);
	//CloseHandle(hChildStd_OUT_Wr);
	//CloseHandle(hChildStd_OUT_Rd);
	//CloseHandle(hChildStdoutRdDup);
	//CloseHandle(hChildStd_IN_Wr);
	//CloseHandle(hChildStd_IN_Rd);
	//CloseHandle(hChildStdinWrDup);

	return processId;
}

int createProcessPipe(const string &commandLine, const string& title, HANDLE &hChildStd_OUT_Rd) 
{
	HANDLE hChildStd_OUT_Wr = NULL;
	//HANDLE hChildStd_OUT_Rd = NULL;
	
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if( ! CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
	{
		cerr << "Error: "  << GetLastError() << endl;
		throw MiddlewareException("Could not create a pipe for the child process's STDOUT");
	}

	if( ! SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		throw MiddlewareException("Could not set handle information HANDLE_FLAG_INHERIT for STDOUT pipe");

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
	startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_SHOWMINIMIZED;  
	startupInfo.cbReserved2 = 0;  
	startupInfo.lpReserved2 = 0;  
	startupInfo.hStdInput = 0;  
	startupInfo.hStdOutput = hChildStd_OUT_Wr;
	startupInfo.hStdError = hChildStd_OUT_Wr;

	PROCESS_INFORMATION processInfo;

	BOOL success = CreateProcess(
		0,							// lpApplicationName
		(LPSTR)commandLine.c_str(),	// lpCommandLine
		0,							// lpProcessAttributes
		0,							// lpThreadAttributes
		TRUE,						// bInheritHandles
		CREATE_NEW_CONSOLE,			// DETACHED_PROCESS dwCreationFlags
		0,							// lpEnvironment
		0,							// lpCurrentDirectory
		&startupInfo,				// lpStartupInfo
		&processInfo				// lpProcessInformation
	);

    if(!success) throw MiddlewareException("Could not create process. Be sure that the following executable is located in the path: " + commandLine); 
	
	int processId = (int)processInfo.dwProcessId;

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	if(!CloseHandle(hChildStd_OUT_Wr))
		throw MiddlewareException("Could not close hChildStd_OUT_Wr handle ");

	//CloseHandle(hChildStd_OUT_Rd);
	return processId;
}

#endif

} // namespace Windows
} // namespace ProcessFactoryLibrary



/*bool PdhCounterClass::init()
{
	PDH_STATUS status;

	if((status=PdhOpenQuery(NULL, 0, &hquery)) != ERROR_SUCCESS)
		return false;
	if((status=PdhAddCounter(hquery, "\\Processor(_Total)\\% Processor Time",0, &hcountercpu)) != ERROR_SUCCESS)
		return false;
	if((status=PdhAddCounter(hquery,"\\Memory\\Available Bytes",0, &hcounterram)) != ERROR_SUCCESS)
		return false;

	if((status=PdhAddCounter(hquery,"\\Network Interface(*)\\Bytes Received/sec",0, &hcounternetin)) != ERROR_SUCCESS)
	{
		//hcounternetin
		cout << "PdhCounterClass::init(): ERROR! \\Network Interface\\Bytes Received/sec " << endl;
		return false;
	}
	
	if((status=PdhAddCounter(hquery,"\\Network Interface(*)\\Bytes Sent/sec",0, &hcounternetout)) != ERROR_SUCCESS)
	{
		cout << "PdhCounterClass::init(): ERROR! \\Network Interface\\Bytes Sent/sec " << endl;
		return false;
	}
	
	if((status=PdhAddCounter(hquery, "\\Process(winlogon)\\Elapsed Time",0, &hcountertime)) != ERROR_SUCCESS)
		return false;
		
	if((status=PdhCollectQueryData(hquery)) != ERROR_SUCCESS)
		return false;

	return true;
}*/

// -------------------------------------------------------
//	Process resources
// -------------------------------------------------------
/*HostResources getProcessResources(int processId)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	//cerr << "Process resources for : " << processId << endl;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
	if(NULL == hProcess) return HostResources();

	//if(BOOL b = ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	//	//SIZE_T sz = pmc.PagefileUsage;
	HostResources res = HostResources();
	res.SetProcessId(processId);
	res.SetResourceView(HostResources::PROCESS);

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
    {
        printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
        printf( "\tPeakWorkingSetSize: 0x%08X\n", pmc.PeakWorkingSetSize );
        printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
        printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakPagedPoolUsage );
        printf( "\tQuotaPagedPoolUsage: 0x%08X\n", pmc.QuotaPagedPoolUsage );
        printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakNonPagedPoolUsage );
        printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", pmc.QuotaNonPagedPoolUsage );
        printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
		printf( "\tPeakPagefileUsage: 0x%08X\n", pmc.PeakPagefileUsage );

		res.SetMemoryUsage(pmc.WorkingSetSize);
    }


	//FILETIME idleTime;
	//FILETIME kernelTime;
	//FILETIME userTime;
	//BOOL res = GetSystemTimes(&idleTime, &kernelTime, &userTime);

	FILETIME creationTime, exitTime, kernelTime, userTime;
	if(GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime))
	{
		//SYSTEMTIME sysUserTime;
		//if(FileTimeToSystemTime(&userTime, &sysUserTime))
		//	res.cpuUsage(sysUserTime.

		//int user = userTime - lastUserTime;
		//int kernel = kernelTime - lastKernelTime;
		//int idle = exitTime
		
	}
	return HostResources();
}*/

// -- first memory --
//MEMORYSTATUS memoryStatus;
//GlobalMemoryStatus(&memoryStatus);
// -- alternate --
// PERFORMANCE_INFORMATION pinfo;
// GetPerformanceInfo(&pinfo, sizeof(pinfo));

// -------------------------------------------------------
//	Node resources
// -------------------------------------------------------
/*HostResources getNodeResources()
{
	if(PdhCounterClass::isInitialized == false)
		PdhCounterClass::isInitialized = PdhCounterClass::init();

	PDH_STATUS status;
	PDH_FMT_COUNTERVALUE countervalcpu;
	PDH_FMT_COUNTERVALUE countervalram;
	PDH_FMT_COUNTERVALUE countervaltime;
	
	PDH_FMT_COUNTERVALUE countervalnetout;
	PDH_FMT_COUNTERVALUE countervalnetin;

	MEMORYSTATUSEX memstat;

	memstat.dwLength = sizeof (memstat);

	if((status=PdhCollectQueryData(PdhCounterClass::hquery))!=ERROR_SUCCESS)
	{
		//printf("PdhCollectQueryData %lx\n", status);    
		return HostResources();
	}
	if(GlobalMemoryStatusEx(&memstat)==0)
	{
		//printf("GlobalMemoryStatusEx %d\n", GetLastError());
		return HostResources();
	}

	if((status=PdhGetFormattedCounterValue(PdhCounterClass::hcountercpu, PDH_FMT_LONG, 0, &countervalcpu))!=ERROR_SUCCESS)
	{
		//printf("PdhGetFormattedCounterValue(cpu) %lx\n", status);    
		return HostResources();
	}
	if((status=PdhGetFormattedCounterValue(PdhCounterClass::hcounterram, PDH_FMT_LONG, 0, &countervalram))!=ERROR_SUCCESS)
	{
		//printf("PdhGetFormattedCounterValue(ram) %lx\n", status);    
		return HostResources();
	}
	

	for(vector<PDH_HCOUNTER>::iterator it = PdhCounterClass::vectorHCounterNetIn.begin(), it_end = PdhCounterClass::vectorHCounterNetIn.end(); it != it_end; ++it)
	{
		if((status=PdhGetFormattedCounterValue(*it, PDH_FMT_LONG, 0, &countervalnetin))!=ERROR_SUCCESS)
		{
			printf("PdhGetFormattedCounterValue(network in) %lx\n", status);
		}
		break;
	}

	for(vector<PDH_HCOUNTER>::iterator it = PdhCounterClass::vectorHCounterNetOut.begin(), it_end = PdhCounterClass::vectorHCounterNetOut.end(); it != it_end; ++it)
	{
		if((status=PdhGetFormattedCounterValue(*it, PDH_FMT_LONG, 0, &countervalnetout))!=ERROR_SUCCESS)
		{
			printf("PdhGetFormattedCounterValue(network out) %lx\n", status);    
		}	
		break;
	}

	if((status=PdhGetFormattedCounterValue(PdhCounterClass::hcountertime, PDH_FMT_LONG, 0, &countervaltime))!=ERROR_SUCCESS)
	{
		//printf("PdhGetFormattedCounterValue(time) %lx\n", status);    
		return HostResources();
	}	

	printf("cpu %3d%% \tmem %3d%% \tavail (%d mb) \tin (%d b) \tout (%d b)\n", 
		countervalcpu.longValue, 
		memstat.dwMemoryLoad, 
		countervalram.longValue/(1024*1024),
		countervalnetin.longValue,
		countervalnetout.longValue);

	HostResources res;
	res.SetCpuUsage(countervalcpu.longValue);
	res.SetMemoryUsage(countervalram.longValue);
	//res.memoryUsage(memstat.dwMemoryLoad);
	res.SetExecutionTime(countervaltime.longValue);
	res.SetResourceView(HostResources::NODE);
	//res.SetAmountReceived(countervalnetin.longValue);
	//res.SetAmountSent(countervalnetout.longValue);

	return res;
}*/
