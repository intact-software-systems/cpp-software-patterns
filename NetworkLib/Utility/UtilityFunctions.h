/***************************************************************************
                          UtilityFunctions.h  -  description
                             -------------------
    begin                : Wed May 30 2012
    copyright            : (C) 2012 by Knut-Helge Vik
    email                : knuthelge.vik@gmail.com
 ***************************************************************************/
#ifndef NetworkLib_UtilityFunctions_h_IsIncluded
#define NetworkLib_UtilityFunctions_h_IsIncluded

#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Export.h"

namespace NetworkLib
{

class DLL_STATE UtilityFunctions
{
public:
    // -- network socket functions --
    static void ShutdownSocket(int socket);
    static void CloseSocket(int socket);
    static int 	SetSocketNonBlocking(int sock);
    static int  SetSocketBlocking(int sock);

    static bool IsValidSocket(int socket);
    static bool IsValidWritePipe(int pipe);
    static bool IsValidReadPipe(int pipe);

    // -- others --
    static void MicroSleep(int64 microsec);

    static std::string MakeRandomTempFile();

    //  static int GetSockOpt();

    static std::string GetAddress(const sockaddr *addr);
    static uint16      GetPortNtohs(const sockaddr *addr);

};

} // namespace NetworkLib

#endif // NetworkLib_UtilityFunctions_h_IsIncluded

