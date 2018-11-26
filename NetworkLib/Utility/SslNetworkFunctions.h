/***************************************************************************
                          SslNetworkFunctions.h  -  description
                             -------------------
    begin                : Wed Mar 21 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifdef SSL_SUPPORT

#pragma once


#include "BaseLib/CommonDefines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <string>

#ifdef USE_GCC
#include <getopt.h>
#include <utime.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

namespace NetworkLib { namespace SslNetworkFunctions 
{

int 		SslLibraryInit();
SSL_CTX* 	CreateCTX(); 
void 		LoadCertificates(SSL_CTX* ctx, const char *cFile, const char *kFile);
int 		acceptSSLFull(SSL* &ssl, SSL_CTX* ctx, int listen_socket, sockaddr_in &addr);
int 		acceptSSLInit(SSL* &ssl, SSL_CTX* ctx, int conn_sock);
int 		connectSSL(SSL* &ssl, SSL_CTX *ctx, const char *name, int port);
void 		freeSSL(SSL *ssl);
void 		freeCTX(SSL_CTX*ctx);
int 		connectSSL(SSL *ssl);
int 		getSSLSocket(SSL *ssl);
const SSL_CIPHER* getSSLCipher(SSL *ssl);
void 		setSSLServerMode(SSL *ssl);
void 		setSSLClientMode(SSL *ssl);
int 		mySSLread(SSL *ssl, char *buf, int length);
int 		mySSLwrite(SSL *ssl, const char *buf, int length);
void 		closeSSL(SSL* ssl, int sock);

}}

#endif

