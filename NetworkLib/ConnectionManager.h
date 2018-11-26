/***************************************************************************
                          ConnectionManager.h  -  description
                             -------------------
    begin                : Sun Mar 13 2011
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class AbstractSocket;

/**
 * ConnectionManager: Manage/monitor all TCP/UDP, etc, connections that are on a single process.
 *
 * - Use a cache
 * - Manage all connections (sockets) in a process
 */
class DLL_STATE ConnectionManager
{
public:
    //

};

}
