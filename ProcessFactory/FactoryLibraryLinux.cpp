#include "ProcessFactory/FactoryLibrary.h"
#include "ProcessFactory/ProcessFactory.h"

#ifdef MACX
#include<sys/sysctl.h>
#include<sys/types.h>
#elif USE_GCC
#include<sys/sysinfo.h>
#endif

#ifdef USE_GCC
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<unistd.h>
//#include<sensors/sensors.h>
#endif

#include<stdlib.h>

using namespace std;

namespace ProcessFactoryLibrary {
namespace Linux
{

static int errSz = 1024;
static char msgBuf[1024];

// for the node
int GetNumberOfFileDescriptors()
{
    // man sysconf
    // Get Limits: Max open files allowed
    //int maxFD = sysconf(_SC_OPEN_MAX);
    //cout << "Open fd max: " << maxFD << endl;
    // Try: STREAM_MAX maximum number of streams that a process can have open at any time.
    //int maxStreams = sysconf(_SC_STREAM_MAX);
    //cout << "Open streams max: " << FOPEN_MAX << " == " << maxStreams << "?" << endl;

    // The number of files presently opened on the node
    // Three numbers: <number of allocated file handles> <number of free file handles> <maximum number of file handles>
    // /proc/sys/fs/file-nr
    int numFd = 0;
    stringstream directory;
    directory << "/proc/sys/fs/file-nr";
    ifstream cmdFile(directory.str().c_str());
    if(cmdFile.is_open())
    {
        char line[4096];
        if(cmdFile.eof() == false)
        {
            cmdFile.getline(line, 4096);
            if(line != NULL)
            {
                //cerr << line << endl;
                vector<string> delimLine = SplitString(string(line),"\t");
                //for(vector<string>::iterator it = delimLine.begin(), it_end = delimLine.end(); it != it_end; ++it)
                //	cerr << *it << ":";
                //cerr << endl;
                if(delimLine.size() > 2)
                {
                    //cerr << "NumFd: " << atoi(delimLine[0].c_str()) - atoi(delimLine[1].c_str()) << endl;
                    numFd = max(0, atoi(delimLine[0].c_str()) - atoi(delimLine[1].c_str()));
                }
            }
        }
        cmdFile.close();
    }

    //cout << "Linux::GetNumberOfFileDescriptors(): Number of /proc/sys/fs/file-nr " << numFd << endl;
    return numFd;
}

int GetNumberOfSockets()
{
    // Number of sockets open:
    // /proc/net/sockstat
    int numSockets = 0;
    stringstream directory;
    directory << "/proc/net/sockstat";
    ifstream cmdFile(directory.str().c_str());
    if(cmdFile.is_open())
    {
        char line[4096];
        while(cmdFile.eof() == false)
        {
            cmdFile.getline(line, 4096);
            if(line == NULL) continue;

            vector<string> delimLine = SplitString(string(line)," ");
            if(delimLine.size() < 3) continue;

            if(strncmp(delimLine[0].c_str(), "TCP", 3) == 0)
                numSockets += atoi(delimLine[2].c_str());
            else if(strncmp(delimLine[0].c_str(), "UDP", 3) == 0)
                numSockets += atoi(delimLine[2].c_str());
        }
        cmdFile.close();
    }

    //cout << "Linux::GetNumberOfSockets(): Number of sockets " << numSockets << endl;
    return numSockets;
}

int GetNumberOfThreads(int pid)
{
    // get current thread id;
    //int getttid();
    stringstream directory;
    directory << "/proc/" << pid << "/stat";

    ifstream cmdFile(((directory).str()).c_str());
    if(!cmdFile.is_open()) return 0;

    stringbuf line;
    cmdFile.get(line);

    vector<string> delimLine = SplitString(string(line.str()), " ");
    if(delimLine.size() < 19) return 0;

    int numThreads = atoi(delimLine[19].c_str());
    cmdFile.close();

    //cout << "Linux::GetNumberOfThreads(" << pid << "): Number of threads " << numThreads << endl;
    return numThreads;
}

#if 0

int GetNumberOfOpenFileDescriptors(int pid)
{
    // Example:
    //# ls /proc/12489/fd | wc -l
    //# 106

    stringstream command;
    command << "ls " << "/proc/" << pid << "/fd | wc -l";

    int numFd = 0;
    FILE *file = createProcessPopen(command.str());
    {
        static bool showErrorMsg;
        if(file == NULL)
        {
            if(showErrorMsg)
                cout << "GetNumberOfOpenFileDescriptors(" << pid << "): ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " popen() failed for cmd: " << command.str() << endl;
            showErrorMsg = false;
            return numFd;
        }
        else showErrorMsg = true;
    }

    int fd = fileno(file);
    time_t start_time = time(NULL);
    while(true)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval tv = {0,100000};
        int ret = select(fd+1, &rfds, NULL, NULL, &tv);
        if(ret <= -1)
        {
            cout << "Linux::GetNumberOfOpenFileDescriptors(pid): Select Error: " << STRERROR_S(errno, msgBuf, errSz) << endl;
            if(errno == EBADF) break;
            continue;
        }

        time_t end_time = time(NULL) - start_time;
        if(ret == 0 || end_time >= 1)
        {
            cout << "GetNumberOfOpenFileDescriptors(" << pid << "): WARNING! Command: " << command.str() << " returned nothing! " << endl;
            break;
        }

        // -- data is available --
        char line[1096];
        char *context = NULL;
        if(fgets(line, sizeof(line), file))
        {
            if(line == NULL) continue;
            else cerr << line << endl;
            numFd = atoi(line) - 1;
            break;
        }
        else
        {
            cout << "GetNumberOfOpenFileDescriptors(" << pid << "): WARNING! Could not get a line! " << strerror(errno) << endl;
            break;
        }
    }

    int ret = pclose(file);
    //if(ret == -1)
    //	cout << "Linux::GetNumberOfFileDescriptor(): pclose(file) returned ERROR! " << STRERROR_S(errno, msgBuf, errSz) << endl;

    //cout << "GetNumberOfOpenFileDescriptors(" << pid << "): Number of open file descriptors: " << numFd << endl;
    return numFd;
}

#else

int GetNumberOfOpenFileDescriptors(int pid)
{
    // Example:
    //# ls /proc/12489/fd | wc -l
    //# 106

    stringstream tempFile;
    tempFile << "/tmp/temp" << rand(); // << "." << Thread::GetProcessId();
    stringstream command;
    command << "ls " << "/proc/" << pid << "/fd | wc -l > " << tempFile.str() << " 2> /dev/null";

    // NB! when signal(SIGCHLD, SIG_IGN); is defined system call returns -1 no matter what!
    int ret = system(command.str().c_str());
    //if(ret == -1)
    //	cout << "Linux::GetCPUTemperatures(): system returned -1 : " << strerror(errno) << endl;

    int numFd = 0;
    char line[4096];

    ifstream infile(tempFile.str().c_str());
    if(infile.is_open())
    {
        while(!infile.eof() && infile.is_open())
        {
            infile.getline(line, 4096);
            if(line == NULL) continue;
            //if(infile.gcount() <= 0) break;
            numFd = atoi(line);
            break;
        }
        infile.close();
    }

    remove(tempFile.str().c_str());

    //if(numFd == 0)
    //	cout << "GetNumberOfOpenFileDescriptors(" << pid << "): WARNING! Could not get a line! " << strerror(errno) << endl;
    //else
    //	cout << "GetNumberOfOpenFileDescriptors(" << pid << "): Number of open file descriptors: " << numFd << endl;
    return numFd;
}

#endif

#if 0
map<string, int> GetCPUTemperatures()
{
    HostResources::MapStringInt32 mapCPUTemperature;
    sensors_chip_name chip_name;
    //int nr = 0;
    for(int nr = 0; nr < 10; nr++)
    {
        const sensors_chip_name *chip_names = sensors_get_detected_chips(NULL, &nr);
        if(chip_names == NULL)
        {
            cout << "Chip name is NULL!!" << endl;
        }
        else
        {
            //sensors_feature_type type;
            double chipTemp = 0;
            int ret = sensors_get_value(chip_names, 2, &chipTemp);
            cout << "Name = " << chip_names->prefix << " temp value " << chipTemp << endl;
        }
    }
    return mapCPUTemperature;
}

#elif 0

map<string, int> GetCPUTemperatures()
{
    stringstream command;
    command << "sensors";

    HostResources::MapStringInt32 mapTemperature;

    FILE *file = createProcessPopen(command.str());
    {
        static bool showErrorMsg;
        if(file == NULL)
        {
            if(showErrorMsg)
                cout << "Linux::GetCPUTemperatures(): ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " popen() failed for cmd: " << command.str() << endl;
            showErrorMsg = false;
            return mapTemperature;
        }
        else showErrorMsg = true;
    }

    int fd = fileno(file);
    time_t start_time = time(NULL);
    while(true)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval tv = {0,100000};
        int ret = select(fd+1, &rfds, NULL, NULL, &tv);
        if(ret <= -1)
        {
            cout << "Linux::GetCPUTemperatures(): Select Error: " << STRERROR_S(errno, msgBuf, errSz) << endl;
            if(errno == EBADF) break;
            continue;
        }

        time_t end_time = time(NULL) - start_time;
        if(ret == 0 || end_time >= 1)
        {
            cout << "Linux::GetCPUTemperatures(): WARNING! Command: " << command.str() << " returned no temperatures! " << endl;
            break;
        }

        // -- data is available --
        char line[8096];
        char *context = NULL;
        assert(sizeof(line) >= 8096);
        if(fgets(line, sizeof(line), file))
        {
            if(line == NULL) continue;
            //else cerr << line << endl;

            char *coreId = STRTOK_S(line, ":", &context);
            if(coreId == NULL) continue;
            //cout << "Core Id :" << coreId << endl;

            if(strncmp(coreId, "Core ", 5) == 0)
            {
                char *temp = STRTOK_S(NULL, " C", &context);
                if(temp == NULL) continue;

                //cout << "Temperature(ascii) " << temp << endl;
                int iTemp = atoi(temp);
                //cout << "Core Id : " << coreId << " Temperature(int) " << iTemp << endl;
                mapTemperature[coreId] = iTemp; //.insert(pair<string, int>(string(coreId),iTemp));
            }
            else if(strncmp(coreId, "No sensors found!", 15) == 0)
            {
                //IWARNING()  << " No sensors found!" ;
                break;
            }
        }
        else
        {
            //IWARNING()  << " Could not get a line! " << strerror(errno);
            break;
        }
    }

    int ret = pclose(file);
    //if(ret == -1)
    //	cout << "Linux::GetCPUTemperatures(): pclose(file) returned ERROR! " << STRERROR_S(errno, msgBuf, errSz) << endl;

    return mapTemperature;
}

#else

map<string, int> GetCPUTemperatures()
{
    stringstream tempFile;

    tempFile << "/tmp/temp" << rand(); // << "." << Thread::GetProcessId();
    stringstream command;
    command << "sensors > " << tempFile.str() << " 2> /dev/null"; //" 2>&1" ;

    // -- debug --
    //com::kongsberg::SensorSystems::ProcessFactory::ResourceMonitor::initTemperatureString();
    //ofstream ofile(tempFile.str().c_str());
    //assert(ofile.is_open());
    //ofile << com::kongsberg::SensorSystems::ProcessFactory::ResourceMonitor::tempString.str();
    //ofile.close();
    //cout << "Created temp file : " << tempFile.str() << endl;
    // -- end debug --

    // NB! when signal(SIGCHLD, SIG_IGN); is defined system call returns -1 no matter what!
    int ret = system(command.str().c_str());
    //if(ret == -1)
    //	cerr << "Linux::GetCPUTemperatures(): system returned -1 : " << strerror(errno) << endl;

    HostResources::MapStringInt32 mapTemperature;
    char line[4096];
    char *context = NULL;

    ifstream infile(tempFile.str().c_str());
    if(infile.is_open())
    {
        while(!infile.eof() && infile.is_open())
        {
            infile.getline(line, 4096);
            if(line == NULL) continue;
            //if(infile.gcount() <= 0) break;

            char *chCoreId = STRTOK_S(line, ":", &context);
            if(chCoreId == NULL) continue;

            string coreId(chCoreId);
            //cout << "Core Id :" << coreId << endl;

            if(strncmp(coreId.c_str(), "Core ", 5) == 0)
            {
                char *temp = STRTOK_S(NULL, " C", &context);
                if(temp == NULL) continue;

                //cout << "Temperature(ascii) " << temp << endl;
                int iTemp = atoi(temp);
                //cout << coreId << " Temperature(int) " << iTemp << endl;

                mapTemperature[coreId] = iTemp;
            }
            else if(strncmp(coreId.c_str(), "temp", 4) == 0)
            {
                char *temp = STRTOK_S(NULL, "C", &context);
                if(temp == NULL) continue;

                //IDEBUG() << temp ;
                int iTemp = atoi(temp);

                mapTemperature[coreId] = iTemp;
            }
            else if(strncmp(coreId.c_str(), "No sensors found!", 15) == 0)
            {
                //IWARNING()  << " No sensors found!" ;
                break;
            }
            //cerr << "." ;
        }

        infile.close();
    }
    else // -> tempFile not found!
    {
        static bool showErrorMsg;
        if(showErrorMsg)
            IDEBUG() << " ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " system(" << command.str() << ") failed! Is lm-sensors 'sensors' installed?";
        showErrorMsg = false;
        return mapTemperature;
    }

    remove(tempFile.str().c_str());

    //cout << "New temps : " << mapTemperature << endl;
    return mapTemperature;
}

#endif

vector<string> SplitString(string str, string delimiters)
{
    vector<string> tokens;
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

    return tokens;
}

vector<int> findChildZombies(const int &parentProcessId)
{
    vector<int> childZombies;

#ifdef USE_GCC
    stringstream stream;
    stream << "ps ax -o ppid,pid,stat | grep " << parentProcessId << " | grep Z";

    FILE *file = createProcessPopen(stream.str());
    {
        static bool showErrorMsg;
        if(file == NULL)
        {
            if(showErrorMsg)
                IDEBUG() << " ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " popen() failed " << stream.str();
            showErrorMsg = false;
            return childZombies;
        }
        else showErrorMsg = true;
    }

    stringstream parentProcessIdStr;
    parentProcessIdStr << parentProcessId;

    int fd = fileno(file);
    time_t start_time = time(NULL);
    while(true)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval tv = {0,100000};
        int ret = select(fd+1, &rfds, NULL, NULL, &tv);
        if(ret <= -1)
        {
            IDEBUG() << " Select Error: " << STRERROR_S(errno, msgBuf, errSz);
            if(errno == EBADF) break;
            continue;
        }

        time_t end_time = time(NULL) - start_time;
        if(ret == 0 || end_time >= 1)
        {
            //cout << "Linux::findChildZombies(ppid): ERROR! Command: " << stream.str() << " returned no child process id! " << endl;
            break;
        }

        // -- data is available --
        char line[8096];
        char *context = NULL;
        if(fgets(line, sizeof(line), file))
        {
            if(line == NULL) continue;

            vector<string> delimLine = SplitString(string(line), " ");
            if(delimLine.size() <= 0) continue;

            if(strcmp(delimLine[0].c_str(), parentProcessIdStr.str().c_str()) == 0)
            {
                int childPid = atoi(delimLine[1].c_str());
                childZombies.push_back(childPid);
            }
        }
        else
        {
            //IWARNING()  << " Could not get a line! ";
            break;
        }
    }

    int ret = pclose(file);
    //if(ret == -1)
    //	cout << "Linux::findChildZombies(ppid): pclose(file) returned ERROR! " << STRERROR_S(errno, msgBuf, errSz) << endl;
#endif

    return childZombies;
}

#if 1
int findChildProcessId(const int &parentProcessId, const string &executableName)
{
    int childProcessId = -1;

    int maximumPid = max(65000, parentProcessId);

    ifstream maxPidFile("/proc/sys/kernel/pid_max");
    if(maxPidFile.is_open())
    {
        stringstream stream;
        stream << maxPidFile.rdbuf();
        if(stream.str().empty() == false)
        {
            maximumPid = atoi(stream.str().c_str());
            if(maximumPid < 32768 || maximumPid > 65000)
                maximumPid = max(65000, parentProcessId);
            //else cerr << "Linux::findChildProcessId(): Found maximumPid : " << maximumPid << endl;
        }
        maxPidFile.close();
    }

    for(int pid = parentProcessId+1; pid < maximumPid; pid++)
    {
        stringstream directory;
        directory << "/proc/" << pid << "/stat";

        ifstream cmdFile(((directory).str()).c_str());
        if(!cmdFile.is_open()) continue;

        stringbuf line;
        cmdFile.get(line);

        vector<string> delimLine = SplitString(string(line.str()), " ");
        if(delimLine.size() < 4) continue;

        int parentId = atoi(delimLine[3].c_str());
        if(parentId == parentProcessId)
        {
            //cerr << "(Linux::findChildProcessId(),240): Found " << parentProcessId << "'s child pid " << pid << endl;
            childProcessId = pid;
            break;
        }
        cmdFile.close();
    }

    // backup: rarely happens. But, happens when pid wraps around max-pid.
    if(childProcessId == -1)
    {
        for(int pid = 1; pid < parentProcessId; pid++)
        {
            stringstream directory;
            directory << "/proc/" << pid << "/stat";

            ifstream cmdFile(((directory).str()).c_str());
            if(!cmdFile.is_open()) continue;

            stringbuf line;
            cmdFile.get(line);

            vector<string> delimLine = SplitString(string(line.str()), " ");
            if(delimLine.size() < 4) continue;

            int parentId = atoi(delimLine[3].c_str());
            if(parentId == parentProcessId)
            {
                //cerr << "(Linux::findChildProcessId(),267): Found " << parentProcessId << "'s child pid " << pid << endl;
                childProcessId = pid;
                break;
            }
            cmdFile.close();
        }
    }

    // failsafe: return parent id if all fails!
    if(childProcessId == -1)
    {
        cout << "Linux::findChildProcessId(ppid, exec): WARNING! Could not find process id for " << executableName << "! Using parent xterm's pid!" << endl;
        return parentProcessId;
    }

    return childProcessId;
}


#else
/*
int findChildProcessId(const int &parentProcessId, const string &executableName)
{
#ifdef USE_GCC
    stringstream stream;
    stream << "ps -o ppid,pid,command ax | grep " << parentProcessId ;

    FILE *file = createProcessPopen(stream.str());
    {
        static bool showErrorMsg;
        if(file == NULL)
        {
            if(showErrorMsg)
                IDEBUG() << " ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " popen() failed " << stream.str();
            showErrorMsg = false;
            return parentProcessId;
        }
        else showErrorMsg = true;
    }

    int childProcessId = -1;

    int fd = fileno(file);
    time_t start_time = time(NULL);
    while(childProcessId == -1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval tv = {0,100000};
        int ret = select(fd+1, &rfds, NULL, NULL, &tv);
        if(ret <= -1)
        {
            IDEBUG() << " Select Error: " << STRERROR_S(errno, msgBuf, errSz);
            if(errno == EBADF) break;
            continue;
        }

        time_t end_time = time(NULL) - start_time;
        if(ret == 0 || end_time >= 1)
        {
            cout << "Linux::findChildProcessId(ppid, exec): Timeout! " << end_time << " Command: " << stream.str() << " returned no child process id! " << endl;
            break;
        }

        // -- data is available --
        char line[8096];
        char *context = NULL;
        if(fgets(line, sizeof(line), file))
        {
            if(line == NULL) continue;

            //cout << line << endl;
            vector<string> delimLine = SplitString(string(line), " ");
            if(delimLine.size() <= 0) continue;

            stringstream parentProcessIdStr;
            parentProcessIdStr << parentProcessId;
            if(strcmp(delimLine[0].c_str(), parentProcessIdStr.str().c_str()) == 0)
            {
                if(executableName.size() > 0)
                {
                        if(strncmp(executableName.c_str(), line, executableName.size()) == 0)
                        {
                            IDEBUG() << " Exeuctable " << line << " == " << executableName;
                            childProcessId = atoi(delimLine[1].c_str());
                            break;
                        }
                        else
                        {
                            IDEBUG() << " " << executableName << " != " << line;
                        }
                    //}
                }
                else
                {
                    childProcessId = atoi(delimLine[1].c_str());
                    break;
                }
            }
        }
    }

    int ret = pclose(file);
    //if(ret == -1)
    //	cout << "Linux::findChildProcessId(ppid, exec): pclose(file) returned ERROR! " << STRERROR_S(errno, msgBuf, errSz) << endl;

    if(childProcessId <= -1)
    {
        //cout << "Linux::findChildProcessId(ppid, exec): ERROR! Could not find child process id!" << endl;
        return parentProcessId;
    }

    return childProcessId;
#else
    return parentProcessId;
#endif
}*/

#endif

// -------------------------------------------------------
//	read data from a file descriptor
// -------------------------------------------------------
#ifdef USE_GCC
int readProcessIdFromFD(FILE *file)
{
    if(file == NULL)
    {
        IDEBUG() << "ERROR! file == NULL ";
        return -1;
    }

    int fd = fileno(file);
    time_t start_time = time(NULL);
    while(true)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval tv = {4,0};
        int ret = select(fd+1, &rfds, NULL, NULL, &tv);
        if(ret <= -1)
        {
            IDEBUG() << " Select Error: " << STRERROR_S(errno, msgBuf, errSz);
            if(errno == EBADF) break;

            continue;
        }

        time_t end_time = time(NULL) - start_time;
        if(ret == 0 || end_time >= 5)
        {
            IDEBUG() << " Select timed out with no process id read! " << end_time;
            break;
        }

        // -- data is available --
        char line[8192];
        char *context = NULL;
        if(fgets(line, sizeof(line), file))
        {
            IDEBUG() << line;

            char *c = STRTOK_S(line, " ", &context);
            if(c == NULL) continue;

            while(c != NULL)
            {
                if(strcmp(c, COMPONENT_PROCESS_ID) == 0)
                {
                    char *tok = STRTOK_S(NULL, " ", &context);
                    return atoi(tok);
                }

                c = STRTOK_S(NULL, " ", &context);
            }
        }
    }
    return -1;
}
#endif
// -------------------------------------------------------
//	get  process or host resources
// -------------------------------------------------------
bool getProcessResources(int pid, HostResources &res)
{
#ifdef USE_GCC
    stringstream directory;
    directory << "/proc/" << pid ;

    ifstream procFile(((directory).str()).c_str());
    if(!procFile.is_open())
    {
        //cout << "FactoryLibraryLinux::getProcessResources(pid): Couldn't find process id : " << pid << endl;
        return false;
    }
    procFile.close();

    directory << "/stat";
    ifstream cmdFile(directory.str().c_str());
    if(cmdFile.is_open())
    {
        if(!cmdFile.eof())
        {
            stringbuf line;
            cmdFile.get(line);

            vector<string> delimLine = SplitString(string(line.str()), " ");
            if(delimLine.size() >= 23)
            {
                res.SetResourceView(HostResources::PROCESS);

                long long totalUsed = ATOLL(delimLine[13].c_str()) + ATOLL(delimLine[14].c_str());
                //long long totalCPUCycles = Linux::getTotalCPUCycles();
                //if(totalCPUCycles <= 0) totalCPUCycles = 1;
                //int cpuUsagePercent = (int) (totalUsed/totalCPUCycles);
                //cerr << "CpuUsage : " << totalUsed << "/" << totalCPUCycles << " = " << cpuUsagePercent << endl;

                res.SetCpuUsage((int)totalUsed); //atoi(delimLine[13].c_str()) + atoi(delimLine[14].c_str()));

                //int pageSize = getpagesize();
                //if(pageSize <= 0) pageSize = 4096;	// -> failsafe
                //res.SetMemoryUsage(ATOLL(delimLine[23].c_str()) * pageSize);

                res.SetExecutionTime(atoi(delimLine[21].c_str()));
            }
        }
        cmdFile.close();
    }

    stringstream statm;
    statm << "/proc/" << pid << "/statm";
    ifstream fileMem(statm.str().c_str());
    if(fileMem.is_open())
    {
        while(!fileMem.eof() && fileMem.is_open())
        {
            string line;
            getline(fileMem, line);
            if(line.size() == 0) continue;

            vector<string> lineParts = SplitString(line, " ");
            //cout << "Memory : " << lineParts[1] << endl;
            if(lineParts.size() <= 1)
            {
                cout << "getProcessResources(): WARNING! Could not retrieve resident set size from /proc/pid/statm! " << endl;
                continue;
            }

            int pageSize = getpagesize();
            if(pageSize <= 0) pageSize = 4096;	// -> failsafe

            // -- resident set size
            res.SetMemoryUsage(ATOLL(lineParts[1].c_str()) * pageSize);
            break;
        }
        fileMem.close();
    }

#endif

    return true;
}


double getCPUUsagePercent(int pid, int64 &totalUsedPrev, int64 &totalCPUUsedPrev, int64 &totalCPUUsedCurr)
{
    stringstream directory;
    directory << "/proc/" << pid ;

    ifstream procFile(((directory).str()).c_str());
    if(!procFile.is_open()) return false;

    procFile.close();

    int64 totalUsedCurr = 0;
    directory << "/stat";
    ifstream cmdFile(directory.str().c_str());
    if(cmdFile.is_open())
    {
        if(!cmdFile.eof())
        {
            stringbuf line;
            cmdFile.get(line);

            vector<string> delimLine = SplitString(string(line.str()), " ");
            if(delimLine.size() >= 23)
            {
                totalUsedCurr = (int64) (ATOLL(delimLine[13].c_str()) + ATOLL(delimLine[14].c_str()));
                //cerr << line.str() << endl;
                //cerr << "Getting numbers : " << delimLine[13].c_str() << " + " << delimLine[14].c_str() << endl;
            }
        }
        cmdFile.close();
    }

    int64 diff_cpu_total = totalCPUUsedCurr - totalCPUUsedPrev;
    if(diff_cpu_total <= 0)	diff_cpu_total = 1;

    int64 diff_pid_total = (totalUsedCurr - totalUsedPrev);

    double usageCPUPercent = 100 * (double)((double)diff_pid_total/(double)diff_cpu_total);

    //cerr << "diff_pid_total = " << totalUsedCurr << " - " << totalUsedPrev << endl;
    //cerr << "diff_cpu_total = " << totalCPUUsedCurr << " - " << totalCPUUsedPrev << endl;
    //cerr << "Total CPU: " << diff_pid_total << "/" << diff_cpu_total << " = " << usageCPUPercent << endl;

    totalUsedPrev = totalUsedCurr;

    return usageCPUPercent;
}

bool getDiskPartitions(vector<string> &vectorPartitions)
{
    // get partition name
    ifstream partFile("/proc/partitions");
    if(!partFile.is_open())
    {
        //cout << "getDiskTraffic(): ERROR! partition file is not found! " << STRERROR_S(errno, msgBuf, errSz) << endl;
        return false;
    }

    //cout << "Partitions: " ;
    while(!partFile.eof() && partFile.is_open())
    {
        string line;
        getline(partFile, line);
        if(line.size() == 0) continue;

        vector<string> lineParts = SplitString(line, " ");
        if(lineParts[3] == "name") continue;

        //cout << lineParts[3] << " " ;
        vectorPartitions.push_back(lineParts[3]);
    }
    partFile.close();
    //cout << endl;

    return true;
}

bool getDiskTraffic(int monitorInterval, vector<string> &vectorPartitions, MetricStateWrapper32 &diskTransfer, MetricStateWrapper32 &diskRead, MetricStateWrapper32 &diskWrite)
{
    if(vectorPartitions.empty())
    {
        bool ret = getDiskPartitions(vectorPartitions);
        if(ret == false)
        {
            //cout << "getDiskTraffic(): ERROR! Could not identify disk partitions! " << endl;
            return false;
        }
    }

    ifstream diskStats("/proc/diskstats");
    if(!diskStats.is_open())
    {
        //cout << "getDiskTraffic(): ERROR! Could not find /proc/diskstats : " << STRERROR_S(errno, msgBuf, errSz) << endl;
        return false;
    }

    while(!diskStats.eof() && diskStats.is_open())
    {
        string line;
        getline(diskStats, line);
        if(line.size() == 0) continue;

        vector<string> lineParts = SplitString(line, " ");

        bool foundPart = false;
        string partName = lineParts[2];
        for(vector<string>::iterator it = vectorPartitions.begin(), it_end = vectorPartitions.end(); it != it_end; ++it)
            if(*it == partName)
            {
                foundPart = true;
                break;
            }

        if(foundPart == false) continue;

        // calculate stats
        /*
        Field 1 -- # of reads issued
        Field 2 -- # of reads merged
        Field 3 -- # of sectors read
        Field 4 -- # of milliseconds spent reading
        Field 5 -- # of writes completed
        Field 6 -- # of writes merged
        Field 7 -- # of sectors written
        Field 8 -- # of milliseconds spent writing
        Field 9 -- # of I/Os currently in progress
        Field 10 -- # of milliseconds spent doing I/Os
        Field 11 -- weighted # of milliseconds spent doing I/Os
        */

        int avgNumDiskTransfers = 0, avgWriteSec = 0, avgReadSec = 0;
        int currRead = 0, currWrite = 0;
        if(lineParts.size() >= 5)
        {
            currRead = atoi(lineParts[4].c_str());
            int prevRead = 0;
            if(diskRead.prevState.count(partName) <= 0)
                diskRead.prevState.insert(pair<string, int>(partName, currRead));

            prevRead = diskRead.prevState[partName];

            avgReadSec = ((currRead - prevRead)/monitorInterval) * 512;
            diskRead.prevState[partName] = currRead;

            if(diskRead.metricState.count(partName) <= 0)
                diskRead.metricState.insert(pair<string, int>(partName, avgReadSec));
            else
                diskRead.metricState[partName] = avgReadSec;
        }

        if(lineParts.size() >= 10)
        {
            currWrite = atoi(lineParts[9].c_str());
            int prevWrite = 0;
            if(diskWrite.prevState.count(partName) <= 0)
                diskWrite.prevState.insert(pair<string, int>(partName, currWrite));

            prevWrite = diskWrite.prevState[partName];

            avgWriteSec = ((currWrite - prevWrite)/monitorInterval) * 512;
            diskWrite.prevState[partName] = currWrite;

            if(diskWrite.metricState.count(partName) <= 0)
                diskWrite.metricState.insert(pair<string, int>(partName, avgWriteSec));
            else
                diskWrite.metricState[partName] = avgWriteSec;
        }

        if(lineParts.size() >= 5)
        {
            int currDiskTransfers = currRead + currWrite;
            int prevDiskTransfers = 0;
            if(diskTransfer.prevState.count(partName) <= 0)
                diskTransfer.prevState.insert(pair<string, int>(partName, currDiskTransfers));

            prevDiskTransfers = diskTransfer.prevState[partName];

            avgNumDiskTransfers = ((currDiskTransfers - prevDiskTransfers)/monitorInterval);
            diskTransfer.prevState[partName] = currDiskTransfers;

            if(diskTransfer.metricState.count(partName) <= 0)
                diskTransfer.metricState.insert(pair<string, int>(partName, avgNumDiskTransfers));
            else
                diskTransfer.metricState[partName] = avgNumDiskTransfers;
        }

        //cout << partName << " read " << avgReadSec << " write " << avgWriteSec << " transfers " << avgNumDiskTransfers << endl;
    }

    diskStats.close();

    return true;
}

bool getNetworkTraffic(MetricStateWrapper64 &sentState, MetricStateWrapper64 &recvState)
{
    stringstream devFile;
    devFile << "/proc/net/dev";

    ifstream cmdFile(devFile.str().c_str());
    if(!cmdFile.is_open()) return false;

    map<string, map<string, long long> > mapRecvState, mapSentState;

    vector<string> recvVector, sentVector;
    while(!cmdFile.eof() && cmdFile.is_open())
    {
        string line;
        getline(cmdFile, line);
        if(line.size() == 0)
            break;
        //cerr << line << endl;

        if(strncmp(line.c_str(), "Inter-", 5) == 0)
            continue;
        else if(strncmp(line.c_str(), " face ", 5) == 0)
        {
            vector<string> delimLine = SplitString(string(line), "|");

            //cerr << "Receive string : " << delimLine[1] << endl;
            //cerr << "Sent string : " << delimLine[2] << endl;

            recvVector = SplitString(string(delimLine[1]), " ");
            sentVector = SplitString(string(delimLine[2]), " ");

            //cerr << "Receive vector : " ;
            //for(size_t i = 0; i < recvVector.size(); i++)
            //	cerr << recvVector[i]  << " " ;

            //cerr << endl << "Sent vector : " ;
            //for(size_t i = 0; i < sentVector.size(); i++)
            //	cerr << sentVector[i]  << " " ;
            //cerr << endl;
        }
        else
        {
            vector<string> delimLine = SplitString(string(line), ":");
            //cerr << "Delim size :: " << delimLine.size() << endl;

            string dev = delimLine[0];
            vector<string> devVector = SplitString(dev, " ");
            dev = devVector[0];

            //cerr << "Device: " << dev << endl;
            map<string, long long> recvState, sentState;

            delimLine = SplitString(delimLine[1], " ");

            //cerr << "Recv bytes : "  << endl;
            for(size_t i = 0; i < delimLine.size(); i++)
            {
                if(i >= recvVector.size()) break;
                //cerr << atoi(delimLine[i].c_str()) << " ";
                recvState.insert(pair<string, long long>(recvVector[i], ATOLL(delimLine[i].c_str())));
            }

            //cerr << endl << "Sent bytes : "  << endl;
            for(size_t i = recvVector.size(); i < delimLine.size(); i++)
            {
                //cerr << atoi(delimLine[i].c_str()) << " ";
                sentState.insert(pair<string, long long>(sentVector[i-recvVector.size()], ATOLL(delimLine[i].c_str())));
            }
            //cerr << endl;

            mapRecvState[dev] = recvState;
            mapSentState[dev] = sentState;
        }
    }

    cmdFile.close();

    for(map<string, map<string, long long> >::iterator it = mapRecvState.begin(), it_end = mapRecvState.end(); it != it_end; ++it)
    {
        string devName = it->first;
        map<string, long long> &mapStats = it->second;

        long long totalBytesRecv = 0;
        if(mapStats.count("bytes") > 0)
            totalBytesRecv = mapStats["bytes"];

        long long prevTotalBytesRecv = 0;
        if(recvState.prevState.count(devName))
            prevTotalBytesRecv = recvState.prevState[devName];

        long long bytesRecv = 0;
        if(totalBytesRecv >= prevTotalBytesRecv)
            bytesRecv = totalBytesRecv - prevTotalBytesRecv;

        recvState.prevState[devName] = totalBytesRecv;
        recvState.metricState[devName] = bytesRecv;
    }

    for(map<string, map<string, long long> >::iterator it = mapSentState.begin(), it_end = mapSentState.end(); it != it_end; ++it)
    {
        string devName = it->first;
        map<string, long long> &mapStats = it->second;

        long long totalBytesSent = 0;
        if(mapStats.count("bytes") > 0)
            totalBytesSent = mapStats["bytes"];

        long long prevTotalBytesSent = 0;
        if(sentState.prevState.count(devName))
            prevTotalBytesSent = sentState.prevState[devName];

        long long bytesSent = 0;
        if(totalBytesSent >= prevTotalBytesSent)
            bytesSent = totalBytesSent - prevTotalBytesSent;

        sentState.prevState[devName] = totalBytesSent;
        sentState.metricState[devName] = bytesSent;
    }

    return true;
}

#ifdef USE_GCC
size_t getTotalSystemMemory()
{
#ifdef MACX
    int mib[2];
    size_t len;

    int64_t totalphys64 = 0;

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE; /* gives a 64 bit int */
    len = sizeof(totalphys64);
    sysctl(mib, 2, &totalphys64, &len, NULL, 0);
    return totalphys64;
#else
    int64_t pages = sysconf(_SC_PHYS_PAGES);
    int64_t page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
#endif
}
#endif

/*
 *
 *
int main(void)
{
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    u_int namelen = sizeof(mib) / sizeof(mib[0]);
    uint64_t size;
    size_t len = sizeof(size);

    if (sysctl(mib, namelen, &size, &len, NULL, 0) < 0)
    {
        perror("sysctl");
    }
    else
    {
        printf("HW.HW_MEMSIZE = %llu bytes\n", size);
    }
    return 0;
}
 */


#ifdef USE_GCC
/*
 * getloadavg() -- Get system load averages.
 *
 * Put `nelem' samples into `loadavg' array.
 * Return number of samples retrieved, or -1 on error.
 */
/*int getloadavg(double loadavg[], int nelem)
{
    struct loadavg loadinfo;
    int i, mib[2];
    size_t size;

    mib[0] = CTL_VM;
    mib[1] = VM_LOADAVG;
    size = sizeof(loadinfo);
    if (sysctl(mib, 2, &loadinfo, &size, NULL, 0) < 0)
        return (-1);

    nelem = MIN(nelem, sizeof(loadinfo.ldavg) / sizeof(fixpt_t));
    for (i = 0; i < nelem; i++)
        loadavg[i] = (double) loadinfo.ldavg[i] / loadinfo.fscale;
    return (nelem);
}*/
#endif

void getNodeResources(HostResources &res)
{
#ifdef MACX
    /*
The closest equivalent to sysinfo in Mac OS X is sysctl / MIB. It doesn't return a sysinfo struct directly, but most of the values in that structure are available as sysctl keys. For instance:
uptime is approximated by kern.boottime (although that reflects the actual boot time, not the running time)
loads is available as vm.loadavg
totalram = hw.memsize (in bytes)
freeram, sharedram, and bufferram are complicated, as the XNU memory manager works differently from Linux's. I'm not sure if the closest equivalent values ("active" and "inactive" memory) are exposed.
totalswap and freeswap are reflected in vm.swapusage. (But note that OS X allocates swap space dynamically.)
procs doesn't appear to have any equivalent.
totalhigh and freehigh are specific to i386 Linux.
    */

    res.SetResourceView(HostResources::NODE);

    // Get the execution time
    {
        struct timeval boottime = {0, 0};
        size_t len = sizeof(boottime);
        int mib[2] = { CTL_KERN, KERN_BOOTTIME };
        if( sysctl(mib, 2, &boottime, &len, NULL, 0) < 0 )
        {
            IWARNING() << "Failed to get boottime!";
        }
        else // ok
        {
            time_t bsec = boottime.tv_sec, csec = time(NULL);
            double executionTime = difftime(csec, bsec);
            res.SetExecutionTime((int) executionTime);
        }
    }

    // Get the memory usage
    {
        double loadSample[1];
        loadSample[0] = 0x0f;

        int nelem = 1;

        int numSamples = getloadavg(loadSample, nelem);
        if(numSamples < 0)
        {
            IWARNING() << "Failed to get load average";
        }
        else
        {
            res.SetMemoryUsage((int) loadSample[0]);
        }
    }

    // Get the CPU usage
    {
        // TODO: Find out how this is obtained in Mac OS X
        res.SetCpuUsage(0.00f);
    }

#elif USE_GCC
    struct sysinfo s_info;
    int ret = sysinfo(&s_info);

    res.SetResourceView(HostResources::NODE);
    res.SetCpuUsage(s_info.loads[0]);
    res.SetMemoryUsage(s_info.totalram - s_info.freeram); //s_info.bufferram);
    res.SetExecutionTime(s_info.uptime);

    //printf("code error = %d\n", error);
    /*printf("Uptime = %lds\tLoad: 1 min %lu / 5 min %lu / 15 min %lu\t"
           "RAM: total %lu / free %lu / shared %lu\t"
           "Memory in buffers = %lu\tSwap: total %lu / free %lu\t"
           "Number of processes = %d\n",
     s_info.uptime, s_info.loads[0],
     s_info.loads[1], s_info.loads[2],
     s_info.totalram, s_info.freeram,
     s_info.sharedram, s_info.bufferram,
     s_info.totalswap, s_info.freeswap,
     s_info.procs);*/
#endif
}

double getCPUUsagePercent(int64 &prev_total, int64 &prev_idle)
{
    stringstream directory;
    directory << "/proc/stat";

    ifstream cmdFile(directory.str().c_str());
    {
        static bool showErrorMsg;
        if(!cmdFile.is_open())
        {
            if(showErrorMsg)
                IDEBUG() << ": Couldn't find " << directory.str() << " :" << STRERROR_S(errno, msgBuf, errSz);
            showErrorMsg = false;
            return 0;
        }
        else showErrorMsg = true;
    }

    stringbuf line;
    cmdFile.get(line);

    string cpuLine = line.str();
    cpuLine.erase(0, 5);
    vector<string> delimLine = SplitString(cpuLine, " ");
    if(delimLine.size() < 6) return 0;
    //cerr << cpuLine << endl;

    int64 idle_time = atoi(delimLine[3].c_str());
    int64 total_time = 0;
    for(int i = 0; i < 4; i++)
        total_time = total_time + atoi(delimLine[i].c_str());

    double diff_idle = idle_time - prev_idle;
    double diff_total = max(total_time - prev_total, (int64)1);
    //double diff_usage = (double)(1000 * (double)(diff_total - diff_idle)/diff_total + 5)/10;
    double diff_usage = 100 * (diff_total - diff_idle)/diff_total; // + 5)/10;

    //cerr << "Usage : " << diff_usage << " Total : " << total_time <<  " idle " << idle_time << endl;
    prev_total = total_time;
    prev_idle = idle_time;

    cmdFile.close();

    return diff_usage;
}


bool getCPUUsagePercent(map<string, pair<int64, int64> > &prevStats, map<string, double> &currentCPUStats)
{
    stringstream directory;
    directory << "/proc/stat";

    ifstream cmdFile(directory.str().c_str());
    {
        static bool showErrorMsg;
        if(!cmdFile.is_open())
        {
            if(showErrorMsg)
                IDEBUG() << ": Couldn't find " << directory.str() << " :" << STRERROR_S(errno, msgBuf, errSz);
            showErrorMsg = false;
            return false;
        }
        else showErrorMsg = true;
    }

    while(!cmdFile.eof() && cmdFile.is_open())
    {
        string line;
        getline(cmdFile, line);
        if(line.size() == 0)
            break;

        string cpuLine = line;
        vector<string> delimLine = SplitString(cpuLine, " ");
        string cpuId = delimLine[0];
        if(strncmp(cpuId.c_str(), "cpu", 3) != 0) break;

        //cout << "Cpu id : " << cpuId << endl;
        int64 prev_idle = 0, prev_total = 0;
        if(prevStats.count(cpuId) > 0)
        {
            prev_idle = prevStats[cpuId].first;
            prev_total = prevStats[cpuId].second;
        }

        //cout << cpuLine << endl;
        if(delimLine.size() < 6) continue;

        int64 idle_time = atoi(delimLine[4].c_str());
        int64 total_time = 0;
        for(int i = 1; i < 5; i++)
            total_time = total_time + atoi(delimLine[i].c_str());

        double diff_idle = idle_time - prev_idle;
        double diff_total = (double)max(total_time - prev_total, (int64)1);
#if 1
        //double diff_usage = (int64)(1000 * (diff_total - diff_idle)/diff_total + 5)/10;
        double diff_usage = 100 * (diff_total - diff_idle)/diff_total; // + 5)/10;
#else
        int64 diff_usage = (int64)(1000 * (diff_total - diff_idle)/diff_total);
        int64 diff_usage_units = diff_usage/10;
        int64 diff_usage_decimal = diff_usage % 10;
        stringstream cpu_diff_usage;
        cpu_diff_usage << diff_usage_units << "." << diff_usage_decimal;
        double diff_usage_double = (double) atof(cpu_diff_usage.str().c_str());
#endif
        //cerr << "Usage : " << diff_usage << " Total : " << total_time <<  " idle " << idle_time << endl;

        if(currentCPUStats.count(cpuId) > 0)
            currentCPUStats[cpuId] = diff_usage;
        else
            currentCPUStats.insert(pair<string, double>(cpuId, diff_usage));

        pair<int64, int64> pairStats(idle_time, total_time);
        if(prevStats.count(cpuId) > 0)
            prevStats[cpuId] = pairStats;
        else
            prevStats.insert(pair<string, pair<int64, int64> >(cpuId, pairStats));
    }

    cmdFile.close();

    return true;
}

string createXtermArgumentsString(const string &executableName, const string& componentName, const string &componentArgs)
{
    stringstream command;

    command << "xterm -geometry 50x8 -title " << componentName;
    if(ProcessFactoryConfig::display != "")
        command << " -display " << ProcessFactoryConfig::display;
    //if (ProcessFactoryConfig::debugLevel > 0)
    //	command << " -hold";

    if(executableName.rfind(".jar") != string::npos) 	// -> start java -jar
    {
        command << " -e java -jar " << executableName << " ";
        command << componentArgs << " &" ;
    }
    else
    {
        command << " -e ./" << executableName << " ";
        command << componentArgs << " &" ;
    }

    return command.str();
}

string createArgumentsString(const string &executableName, const string &componentName, const string &componentArgs)
{
    stringstream command;
    if(ProcessFactoryConfig::debugTool == "valgrind")
        command << "valgrind --tool=callgrind ";

    if(executableName.rfind(".jar") != string::npos) 	// -> start java -jar
        command << "java -jar ";

    command << executableName << " " << componentArgs << " 2>&1";

    if(ProcessFactoryConfig::debugLevel > 0)
    {
        command << " | tee - ";
        if(ProcessFactoryConfig::logDir.length() > 0)
            command << ProcessFactoryConfig::logDir;
        //command << executableName << "." << componentName << ".debug ";
        command << componentName << ".debug ";
    }
    else
    {
        command << " /dev/null";
    }

    command << " &";

    return command.str();
}

char** createXtermArgumentsCharV(const string &executableName, const string& componentName, const string &componentArgs)
{
#ifdef USE_GCC
    int cnt = 0;
    const char **args;
    args = new const char*[100];

    args[cnt++] = "xterm";
    args[cnt++] = "-geometry";
    args[cnt++] = "50x8";
    args[cnt++] = "-title";
    args[cnt++] = (char*)componentName.c_str();

    if(ProcessFactoryConfig::display != "")
    {
        args[cnt++] = "-display";
        args[cnt++] = (char*)ProcessFactoryConfig::display.c_str();
    }
    //if(ProcessFactoryConfig::debugLevel > 0)
    //	args[cnt++] = "-hold";

    args[cnt++] = "-e";

    if(executableName.rfind(".jar") != string::npos) 	// -> start java -jar
    {
        args[cnt++] = "java";
        args[cnt++] = "-jar";
    }

    char* execute = new char[executableName.length()+1];
    strcpy(execute, executableName.c_str());
    args[cnt++] = execute;

    //cerr << " Found bits : " ;
    string bit;
    for(string::const_iterator it = componentArgs.begin(), it_end = componentArgs.end(); it != it_end; ++it)
    {
        if(*it == ' ' && !bit.empty())
        {
            //cerr << bit << " " ;
            char *myarg = new char[bit.size() + 1];
            strcpy(myarg, bit.c_str());
            args[cnt++] = myarg;
            bit = string();
        }
        else if(*it != ' ')
            bit.push_back(*it);
    }

    if(!bit.empty())
    {
        char *myarg = new char[bit.size() + 1];
        strcpy(myarg, bit.c_str());
        args[cnt++] = myarg;
        bit = string();
    }

    args[cnt++]	= "&";
    args[cnt++] = NULL;

    // -- start debug --
    cerr << endl << "Command: ";
    for(int i = 0; i < cnt - 1; i++)
        cerr << args[i] << " ";
    cerr << endl;
    // -- end debug --

    return (char**)args;
#endif

    return NULL;
}

char** createArgumentsCharV(const string &executableName, const string& componentName, const string &componentArgs)
{
#ifdef USE_GCC
    int cnt = 0;
    const char **args;
    args = new const char*[100];

    if(strcmp(ProcessFactoryConfig::display.c_str(), "noX") != 0)
    {
        args[cnt++] = "xterm";
        args[cnt++] = "-geometry";
        args[cnt++] = "50x8";
        args[cnt++] = "-title";
        args[cnt++] = (char*)componentName.c_str();

        if(ProcessFactoryConfig::display != "")
        {
            args[cnt++] = "-display";
            args[cnt++] = (char*)ProcessFactoryConfig::display.c_str();
        }
        //if(ProcessFactoryConfig::debugLevel > 0)
        //	args[cnt++] = "-hold";
        args[cnt++] = "-e";
    }

    char* execute = new char[executableName.length()+1];
    strcpy(execute, executableName.c_str());
    args[cnt++] = execute;

    //cerr << " Found bits : " ;
    string bit;
    for(string::const_iterator it = componentArgs.begin(), it_end = componentArgs.end(); it != it_end; ++it)
    {
        if(*it == ' ' && !bit.empty())
        {
            //cerr << bit << " " ;
            char *myarg = new char[bit.size() + 1];
            strcpy(myarg, bit.c_str());
            args[cnt++] = myarg;
            bit = string();
        }
        else if(*it != ' ')
            bit.push_back(*it);
    }

    if(!bit.empty())
    {
        char *myarg = new char[bit.size() + 1];
        strcpy(myarg, bit.c_str());
        args[cnt++] = myarg;
        bit = string();
    }

    if(ProcessFactoryConfig::debugLevel > 0)
    {
        // executable | tee logfile.debug -
        args[cnt++] = "|";
        args[cnt++] = "tee";
        args[cnt++] = "-";

        stringstream log_ss;
        if(ProcessFactoryConfig::logDir.length() > 0)
            log_ss << ProcessFactoryConfig::logDir;
        log_ss << componentName << ".debug";

        char *execute = new char[log_ss.str().length() + 1];
        strcpy(execute, log_ss.str().c_str());

        args[cnt++] = execute;
        //command << " | tee - " << executableName << "." << componentName << ".debug ";
    }

    args[cnt++]	= "&";
    args[cnt++] = NULL;

    // -- start debug --
    cerr << endl << "Command: ";
    for(int i = 0; i < cnt - 1; i++)
        cerr << args[i] << " ";
    cerr << endl;
    // -- end debug --

    return (char**)args;
#endif

    return NULL;
}

// -------------------------------------------------------
//	creates a process using popen
// -------------------------------------------------------
int createProcessPopenReadPid(const string &executableName, const string &componentName, const string &componentArgs, FILE* &file)
{
    string command = Linux::createArgumentsString(executableName, componentName, componentArgs);

#ifdef USE_GCC
    file = Linux::createProcessPopen(command);
    {
        static bool showErrorMsg;
        if(file == NULL)
        {
            if(showErrorMsg)
                IDEBUG() << " ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " popen() failed " << command;
            showErrorMsg = false;
            return -1;
        }
        else showErrorMsg = true;
    }

    return readProcessIdFromFD(file);
#endif
    return -1;
}

FILE* createProcessPopen(const string &command)
{
#ifdef USE_GCC
    return popen(command.c_str(), "r");
#endif
    return NULL;
}

int createProcessXtermForkExec(const string &executableName, const string& componentName, const string &componentArgs)
{
    char **args = Linux::createXtermArgumentsCharV(executableName, componentName, componentArgs);

    int ppid = createProcessForkExec(args[0], args);
    delete []args;

    int pid = ppid;
    for(int i = 0; i < 3; i++)
    {
        pid = Linux::findChildProcessId(ppid, executableName);
        if(pid != ppid) break;
        //IDEBUG() << " sleeping!";
        //nanosleep(100000000);
        //Thread::msleep(100);
    }

    return pid;
}

int createProcessForkExec(const string &executableName, const string& componentName, const string &componentArgs)
{
    char **args = Linux::createArgumentsCharV(executableName, componentName, componentArgs);

    int pid = createProcessForkExec(args[0], args);
    delete []args;

    return pid;
}

int createProcessForkExec(const char *executable, char *const args[])
{
#ifdef USE_GCC
    pid_t pID = fork();
    if(pID == 0) // child
    {
        //cerr << "Executing : " << executable << " args : " << args << endl;
        int ret = execvp(executable, args);

        // -- on success child process never reaches this point --
        if(ret == -1) // execution failed
            cerr << "ERROR: " << STRERROR_S(errno, msgBuf, errSz) << " executing: " << executable << " " << args << endl;
        exit(0);
    }
    else if(pID < 0) // failed to fork
    {
        cerr << "ERROR! " << STRERROR_S(errno, msgBuf, errSz) << " Failed to fork/create component! " << executable << " Arg: " << args << endl;
    }
    else // parent
    {
        return (int) pID;
    }
#endif
    return -1;
}

int createProcessForkExecPipe(const char *executable, char *const args[], int *infp, int *outfp)
{
    int pid = -1;
#ifdef USE_GCC
    int p_stdin[2], p_stdout[2];
    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);

        execvp(executable, args);
        perror("execvp");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[1]);
    else
        *infp = p_stdin[1];

    if (outfp == NULL)
        close(p_stdout[0]);
    else
        *outfp = p_stdout[0];

    close(p_stdin[0]);
    close(p_stdout[1]);

#endif
    return pid;
}

} // namespace Linux
} // namespace ProcessFactoryLibrary
