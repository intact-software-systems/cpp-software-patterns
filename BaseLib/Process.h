/***************************************************************************
                          Process.h  -  description
                             -------------------
    begin                : Sun May 9 2010
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#ifndef BaseLib_Process_h_Included
#define BaseLib_Process_h_Included

#include <string>
#include <iostream>
#include <limits.h>

#include "BaseLib/Mutex.h"
#include "BaseLib/MutexLocker.h"

using namespace std;

namespace BaseLib
{

/*class ProcessDetails
{
public:
	enum Priority 
	{ 
		IdlePriority = 0, 
		LowestPriority, 
		LowPriority, 
		NormalPriority, 
		HighPriority,
		HighestPriority,
		TimeCriticalPriority,
		InheritPriority 
	};

	enum State 
	{ 
		NoState = 0, 
		RunState, 
		StopState, 
		CancelState, 
		SystemEndState,
		PrintState,
		CheckState
	};

	friend std::ostream& operator<<(std::ostream& ostr, const ProcessDetails::State &status);

public:
	ProcessDetails() 
		: processId_(-1)
		, processRunning_(false)
		, processPriority_(ProcessDetails::InheritPriority)
		, processState_(ProcessDetails::NoState)
	{ }
	~ProcessDetails() 
	{ }

	pprocess_attr_t&			processAttributes() 		{ MutexLocker lock(&mutex_); return processAttributes_; }
	unsigned long int			processId()			const	{ MutexLocker lock(&mutex_); return processId_; }
	bool						processRunning()	const	{ MutexLocker lock(&mutex_); return processRunning_; }
	ProcessDetails::Priority 	processPriority()	const 	{ MutexLocker lock(&mutex_); return processPriority_; }
	ProcessDetails::State		processState()		const	{ MutexLocker lock(&mutex_); return processState_; }
	std::string					processName()		const 	{ MutexLocker lock(&mutex_); return processName_; }

	//void processAttributes(const pprocess_attr_t &attr)  	{ processAttributes_ = attr; }
	void processId(unsigned long int id)				{ MutexLocker lock(&mutex_); processId_ = id; }
	void processRunning(bool run)						{ MutexLocker lock(&mutex_); processRunning_ = run; }
	void processPriority(ProcessDetails::Priority p)	{ MutexLocker lock(&mutex_); processPriority_ = p; }
	void processState(ProcessDetails::State s)			{ MutexLocker lock(&mutex_); processState_ = s; }
	void processName(std::string name)					{ MutexLocker lock(&mutex_); processName_ = name; }

private:
	pprocess_attr_t 		processAttributes_;
	unsigned long int	processId_;
	bool				processRunning_;
	Priority			processPriority_;
	State				processState_;
	string				processName_;
	mutable Mutex		mutex_;
};*/

//-----------------------------------------------------------------------
//					class Process
//-----------------------------------------------------------------------
/*class Process
{
public:
	Process(std::string name = string(""));
	~Process();
	
	//static void* dispatch(void *process_obj);
	void start(ProcessDetails::Priority priority = ProcessDetails::InheritPriority); 
	virtual void exit();
	virtual void terminate();
	
	bool 					isFinished() const;
	bool 					isRunning() const;
	ProcessDetails::Priority priority() const;
	std::string				processName() const;
	unsigned long int		processId() const;
	
	void 					setPriority(ProcessDetails::Priority priority);
	void					setProcessName(const std::string &name);

	ProcessDetails&			getProcessDetails()		{ return processDetails_; }

public:
	static Process*			currentProcess();
	static unsigned long 	currentProcessId();
	static bool				checkProcessState();
	static int				processId();

protected:
	ProcessDetails		processDetails_;
	mutable Mutex		processMutex_;
};
*/
}; // namespace BaseLib

//typedef boost::shared_ptr<Process> ProcessPtr;

#endif //PLANETLAB_SYSTEM_THREAD_KHV



