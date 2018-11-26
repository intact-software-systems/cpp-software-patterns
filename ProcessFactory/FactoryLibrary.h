#ifndef ProcessFactoryLibrary_h_IS_INCLUDED
#define ProcessFactoryLibrary_h_IS_INCLUDED

#include "ProcessFactory/IncludeExtLibs.h"

#include <list>
#include <map>
#include <vector>

// -------------------------------------------------------
//			ProcessFactoryLibrary
// -------------------------------------------------------
namespace ProcessFactoryLibrary
{
	int isProcessRunning(int64 pid, string executableName = string(""));
	void killProcess(const HostInformation& info);
	void createProcess(const string &commandLine, const string& title);

namespace Linux
{
	struct MetricStateWrapper64
	{
		HostResources::MapStringInt64 prevState;
		HostResources::MapStringInt64 metricState;	
	};

	struct MetricStateWrapper32
	{
		HostResources::MapStringInt32 prevState;
		HostResources::MapStringInt32 metricState;	
	};

	int GetNumberOfThreads(int pid);
	int GetNumberOfOpenFileDescriptors(int pid);
	int GetNumberOfSockets();
	int GetNumberOfFileDescriptors();

	HostResources::MapStringInt32 GetCPUTemperatures();
	
	vector<string>	SplitString(string line, string delim);
	int				findChildProcessId(const int &parentProcessId, const string &executableName);
	vector<int>		findChildZombies(const int &parentProcessId);

	bool getProcessResources(int processId, HostResources &);
	double getCPUUsagePercent(int pid, int64 &totalUsedPrev, int64 &totalCPUUsedPrev, int64 &totalCPUUsedCurr);

	bool getDiskPartitions(vector<string> &vectorPartitions);
	bool getDiskTraffic(int measureInterval, vector<string> &vectorPartitions, MetricStateWrapper32 &diskTransfer, MetricStateWrapper32 &diskRead, MetricStateWrapper32 &diskWrite);
	
#ifdef USE_GCC
    size_t getTotalSystemMemory();
#endif

	void getNodeResources(HostResources &);
	double getCPUUsagePercent(int64 &prev_total, int64 &prev_idle);
	bool getNetworkTraffic(MetricStateWrapper64 &sentState, MetricStateWrapper64 &recvState);
	
	bool getCPUUsagePercent(map<string, pair<int64, int64> > &prevStats, map<string, double> &currentCpuStatus);

#ifdef USE_GCC
	int			readProcessIdFromFD(FILE *file);
#endif

	string createXtermArgumentsString(const string &executableName, const string& componentName, const string &componentArgs);
	char** createXtermArgumentsCharV(const string &executableName, const string& componentName, const string &componentArgs);
	char** createArgumentsCharV(const string &executableName, const string& componentName, const string &componentArgs);
	string createArgumentsString(const string &executableName, const string &componentName, const string &componentArgs);

	int createProcessPopenReadPid(const string &executableName, const string &componentName, const string &componentArgs, FILE* &fd);
	int createProcessXtermForkExec(const string &executableName, const string& componentName, const string &componentArgs);
	int createProcessForkExec(const string &executableName, const string& componentName, const string &componentArgs);

	FILE* createProcessPopen(const string &command);
	int createProcessForkExec(const char *executable, char *const args[]);
	int createProcessForkExecPipe(const char *executable, char *const args[], int *infp, int *outfp);
}

namespace Windows
{
#ifndef USE_GCC
	bool isProcessRunning(HANDLE hProcess);
	
    size_t getTotalSystemMemory();

	void readDataFromPipe(HANDLE hChildStdoutRdDup, int *pid, int *port);
	void readDataFromPipe(int fdStdOutPipe, int *pid, int *port);
	void readDataFromPipe(FILE *file, int *pid, int *port);

	int createProcess(const string &command, const string& title);
	int createProcessSpawnPipe(const char *executable, char *const args[], int *stdOutPipe);
	int createProcessPopen(const string &commandLine, const string &title, FILE *file);
	int createProcessPipe(const string &commandLine, const string& title, HANDLE &hChildStdoutRdDup, HANDLE &hChildStdinWrDup);
	int createProcessPipe(const string &commandLine, const string& title, HANDLE &hChildStd_OUT_Rd);
#endif
}

}; // namesapce ProcessFactoryLibrary

#endif // ProcessFactoryLibrary_h_IS_INCLUDED
