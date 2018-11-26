/***************************************************************************
                          InterruptHandler.h  -  description
                             -------------------
    begin                : Tue Jan 31 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef BaseLib_InterruptHandler_h_IsIncluded
#define BaseLib_InterruptHandler_h_IsIncluded

#include <iostream>
#include <string>

#include "BaseLib/Export.h"
namespace BaseLib 
{

class DLL_STATE InterruptHandling 
{
public:
   static void InitSignalHandlers();

   static std::string getStackTrace();

protected:
   static void HandleInterrupt(int sig);
};

namespace Interrupt
{

//int setup_sigsegv();

//#define CTRL_C_THRESHOLD 	3

//static int ctrl_c_threshold;
//static int ctrl_c_counter;
//static std::string abortLogFile;

//void InitializeInterruptHandlers();
//void catch_pipe(int);
//void catch_int(int);
//void catch_segv(int);
//void catch_abrt(int sig_num);

} // Interrupt

}; // BaseLib

#endif // BaseLib_InterruptHandler_h_IsIncluded


