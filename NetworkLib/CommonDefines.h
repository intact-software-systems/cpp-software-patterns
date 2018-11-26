/*
 * CommonDefines.h
 *
 *  Created on: Mar 11, 2012
 *      Author: knuthelv
 */
#pragma once

#include"BaseLib/CommonDefines.h"

namespace NetworkLib {

/*-------------------------------------------------------
Enumeration used to describe the communication protocol in use
---------------------------------------------------------*/
namespace InternetProtocol {
enum Type
{
    RPC,    // Remote Procedure Call
    RTPS,   // Real-Time Publisher Subscriber
    RTP     // Real-Time Transport Protocol
};
}

} // namespace NetworkLib

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  0
#endif
