/***************************************************************************
                          InterruptHandler.cpp  -  description
                             -------------------
    begin                : Wed Apr 25 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "BaseLib/InterruptHandler.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/LogDebug.h"
#include "BaseLib/Utility.h"
#include "BaseLib/Debug.h"

#include <iostream>
#include <fstream>
#include <signal.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#ifdef USE_GCC
	#ifndef NO_CPP_DEMANGLE
	#include <cxxabi.h>
	#endif

#ifndef MACX
    #include <ucontext.h>
#endif

#include <dlfcn.h>
#include <execinfo.h>
#endif

#if defined(REG_RIP)
# define SIGSEGV_STACK_IA64
# define REGFORMAT "%016lx"
#elif defined(REG_EIP)
# define SIGSEGV_STACK_X86
# define REGFORMAT "%08x"
#else
# define SIGSEGV_STACK_GENERIC
# define REGFORMAT "%x"
#endif

//#ifdef __cplusplus
//extern "C"
//#endif

using namespace std;

namespace BaseLib {

namespace Interrupt
{
    string abortLogFile = string("abort_log.txt");
}

/*---------------------------------------------------------------------------------
				Interrupt Handling
---------------------------------------------------------------------------------*/

std::string InterruptHandling::getStackTrace()
{
    stringstream result;

    size_t size = 0;
    char** strings = NULL;

#ifdef USE_GCC
    void* array[200];
    size    = backtrace(array, 200);
    strings = backtrace_symbols(array, size);
#else
    // TODO!
    IDEBUG() << " Please implement stack trace on windows!" ;
#endif
    if(strings)
    {
        result << (int) size << " frames" << endl;
        //result.sprintf("%d frames\n", (int)size);
        for(size_t i = 0; i < size; ++i)
        {
            if(strings[i])
            {
#ifdef USE_GCC
                fprintf(stderr, "%d: %s\n", (int) i, strdup(strings[i]));
#endif
                //QString tmp;
                //tmp.sprintf("%s\n", strings[i]);
                result << strings[i] << endl;
                //result += tmp;
            }
        }
        if(strings) free(strings);
    }

    return result.str();
}

void InterruptHandling::InitSignalHandlers()
{
    signal(SIGFPE, HandleInterrupt);
    signal(SIGSEGV, HandleInterrupt);
    signal(SIGINT, HandleInterrupt);
    signal(SIGABRT, HandleInterrupt);
    signal(SIGILL, HandleInterrupt);

#ifdef USE_GCC
    signal(SIGBUS, HandleInterrupt);
    signal(SIGALRM, HandleInterrupt);
    signal(SIGUSR1, HandleInterrupt);
    signal(SIGUSR2, HandleInterrupt);
    signal(SIGKILL, HandleInterrupt);
    signal(SIGQUIT, HandleInterrupt);
    signal(SIGPIPE, SIG_IGN);
#endif
}

void InterruptHandling::HandleInterrupt(int sig)
{
    std::stringstream bugStream;
    bugStream << "---------------------------------- Intact Bugreport -------------------------------" << endl;
    bugStream << "Local time:   \t" << Utility::PrintCurrentTimeMsec() << endl;
    bugStream << "Signal caught:\t";

    switch(sig)
    {
#define CASE(t) case t: bugStream << #t << endl; break;
        CASE(SIGTERM)
        CASE(SIGFPE)
        CASE(SIGSEGV)
        CASE(SIGABRT)
        CASE(SIGILL)
        CASE(SIGINT)
#ifdef USE_GCC
        //CASE(SIGBUS)
        CASE(SIGALRM)
        CASE(SIGUSR1)
        CASE(SIGUSR2)
        CASE(SIGQUIT)
#endif
#undef CASE
        default:
            IDEBUG() << " signal " << sig ;
            exit(0);
            break;
    }

    bugStream << "Stack trace:   \t" << getStackTrace();
    bugStream << "---------------------------------------------------------------------------------" << endl;
    //BugreportDialog *dialog = new BugreportDialog(bugReport);
    //int ret = dialog->exec();

    ofstream file(Interrupt::abortLogFile.c_str(), ios::out | ios::app);
    if(file.is_open())
    {
        file << bugStream.str() << endl;
        file << *DebugReport::GetOrCreate() << endl;
        file.close();
    }

    exit(sig);
}

namespace Interrupt {

#ifdef USE_GCC

static void signal_segv(int signum, siginfo_t* info, void* ptr)
{
    // -- start my stuff --
    ASSERT(info);
    ASSERT(ptr);
    IWARNING()  << " SIGSEGV " << signum;

    ofstream outfile(abortLogFile.c_str(), ios::out | ios::app);
    outfile << PRETTY_FUNCTION << " SIGSEGV received: " << endl;
    outfile.close();

    FILE* out = ::fopen(abortLogFile.c_str(), "a");
    // -- end my stuff --

    size_t i;

/*#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)
    int f = 0;
    Dl_info dlinfo;
    void **bp = 0;
    void *ip = 0;
#else*/
    void* bt[20];
    char** strings;
    size_t sz;
//#endif
#ifndef MACX
    ucontext_t* ucontext = (ucontext_t*) ptr;
    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};
    // -- start my stuff --
    fprintf(out, "Segmentation Fault!\n");
    fprintf(out, "info.si_signo = %d\n", signum);
    fprintf(out, "info.si_errno = %d\n", info->si_errno);
    fprintf(out, "info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    fprintf(out, "info.si_addr  = %p\n", info->si_addr);
    for(i = 0; i < NGREG; i++)
        fprintf(out, "reg[%02d]       = 0x" REGFORMAT "\n", i, ucontext->uc_mcontext.gregs[i]);
    // -- end my stuff --

    fprintf(stderr, "Segmentation Fault!\n");
    fprintf(stderr, "info.si_signo = %d\n", signum);
    fprintf(stderr, "info.si_errno = %d\n", info->si_errno);
    fprintf(stderr, "info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    fprintf(stderr, "info.si_addr  = %p\n", info->si_addr);
    for(i = 0; i < NGREG; i++)
        fprintf(stderr, "reg[%02d]       = 0x" REGFORMAT "\n", i, ucontext->uc_mcontext.gregs[i]);

/*#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)
# if defined(SIGSEGV_STACK_IA64)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];
# elif defined(SIGSEGV_STACK_X86)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];
# endif

    fprintf(stderr, "Stack trace:\n");
    while(bp && ip) {
        if(!dladdr(ip, &dlinfo))
            break;

        const char *symname = dlinfo.dli_sname;
#ifndef NO_CPP_DEMANGLE
        int status;
        char *tmp = ::__cxa_demangle(symname, NULL, 0, &status);

        if(status == 0 && tmp)
            symname = tmp;
#endif
        fprintf(stderr, "% 2d: %p <%s+%u> (%s)\n",
                ++f,
                ip,
                symname,
                (unsigned)((char*)ip - (char*)dlinfo.dli_saddr),
                dlinfo.dli_fname);

#ifndef NO_CPP_DEMANGLE
        if(tmp)
            free(tmp);
#endif

        if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
            break;

        ip = bp[1];
        bp = (void**)bp[0];
    }
#else*/

#endif // not defined MACX

    // -- start my stuff --
    fprintf(out, "Stack trace (non-dedicated):\n");
    sz      = backtrace(bt, 20);
    strings = backtrace_symbols(bt, sz);

    for(i = 0; i < sz; ++i)
    {
        fprintf(out, "%s\n", strings[i]);
    }
    fprintf(out, "End of stack trace\n");
    // -- end my stuff --

    fprintf(stderr, "Stack trace (non-dedicated):\n");
    sz      = backtrace(bt, 20);
    strings = backtrace_symbols(bt, sz);

    for(i = 0; i < sz; ++i)
    {
        fprintf(stderr, "%s\n", strings[i]);
    }
//#endif
    fprintf(stderr, "End of stack trace\n");


    fflush(out);
    fclose(out);

    exit(-1);
}

int setup_sigsegv()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = signal_segv;
    action.sa_flags     = SA_SIGINFO;
    if(sigaction(SIGSEGV, &action, NULL) < 0)
    {
        perror("sigaction");
        return 0;
    }

    return 1;
}

#ifndef SIGSEGV_NO_AUTO_INIT

static void __attribute((constructor)) init(void)
{
    setup_sigsegv();
}

#endif

#endif

/*---------------------------------------------------------------------------------
				Interrupt Handling in PlanetLab Simulator
---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------
	Standard procedures:
 
	- set signal masking set
		sigset_t mask_set;
	
	- store old mask set
	    sigset_t old_set;	   
	
	-  mask any further signals while we're inside the handler. 
    	sigfillset(&mask_set);
    	sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	-  re-set the signal handler again to catch_int, for next time 
		signal(SIGSEGV, catch_segv); 		

	- no need to restore the old signal mask - this is done automatically, 
		by the operating system, when a signal handler returns.              
---------------------------------------------------------------------------------*/

/*
void InitializeInterruptHandlers()
{
	//ctrl_c_counter = 0;
	signal(SIGINT, catch_int);
	//signal(SIGSEGV, catch_segv);
#ifdef USE_GCC
	setup_sigsegv();
#endif
	signal(SIGABRT, catch_abrt);
	//signal(SIGSTP, catch_suspend);
	//signal(SIGPIPE, SIG_IGN); //catch_pipe);
}
*/

/*void InitializeInterruptHandlers()
{
	struct sigaction sa, osa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &CatchInterrupts;
	sigaction(SIGSEGV, &sa, &osa);
	sigaction(SIGTERM, &sa, &osa);
	sigaction(SIGABRT, &sa, &osa);
}
string PrintCallStack(int frames)
{
	void *bt[frames];

	size_t sz = backtrace(bt, frames);
	char **strings = backtrace_symbols(bt, sz);
	
	stringstream stream;
	for(int i = 0; i < sz; ++i)
		stream << strings[i] << endl;

	cout << stream.str();
	return stream.str();
}*/

/*---------------------------------------------------------------------------------
						catching: pipe signal error
---------------------------------------------------------------------------------*/
/*void catch_pipe(int sig_num)
{
    sigset_t mask_set;	
    sigset_t old_set;

    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	IWARNING()  << " SIGPIPE received: error: " << strerror(errno) ;

	ofstream outfile(abortLogFile.c_str(), ios::out | ios::app);
	outfile << PRETTY_FUNCTION << " SIGPIPE received: error: " << strerror(errno) << endl;
	outfile.close();
	
	exit(EXIT_SUCCESS);
}*/

/*---------------------------------------------------------------------------------
						catching: Ctrl + c
---------------------------------------------------------------------------------*/
/*void catch_int(int sig_num)
{
    sigset_t mask_set;	
    sigset_t old_set;
	
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	IWARNING()  << " Final Ctrl-c caught " ;
	ofstream outfile(abortLogFile.c_str(), ios::out | ios::app);
	outfile << PRETTY_FUNCTION << " SIGINT received:  " << endl;
	outfile.close();
	
	exit(EXIT_SUCCESS);
	
	//ctrl_c_counter++;	
} */

/*---------------------------------------------------------------------------------
					catching: segmentation faults
---------------------------------------------------------------------------------*/
/*void catch_segv(int sig_num)
{
    sigset_t mask_set;
    sigset_t old_set;   
	
    sigfillset(&mask_set);     		
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	IWARNING()  << " SIGSEGV received: " ;

	ofstream outfile(abortLogFile.c_str(), ios::out | ios::app);
	outfile << PRETTY_FUNCTION << " SIGSEGV received:  " << endl;
	outfile.close();
	
	exit(EXIT_SUCCESS);
}*/

/*---------------------------------------------------------------------------------
			catching: 
					ASSERT(bool), defined in CommonDefines.h,
				   	and assert(bool)
---------------------------------------------------------------------------------*/
/*void catch_abrt(int sig_num)
{
	sigset_t mask_set;	 
    sigset_t old_set;	
		
	signal(SIGABRT, catch_abrt);
    sigfillset(&mask_set); 
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);

	IWARNING()  << " SIGABRT received:  " ;

	ofstream outfile(abortLogFile.c_str(), ios::out | ios::app);
	outfile << PRETTY_FUNCTION << " SIGABRT received:   " << endl;
	outfile.close();

	exit(EXIT_SUCCESS);
}*/

}

}








