#ifdef SSL_SUPPORT

#include"NetworkLib/Utility/SslNetworkFunctions.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"
#include"NetworkLib/IncludeExtLibs.h"

#include <memory.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
//#include <stderr>
//#include <stdlib>

namespace NetworkLib { namespace SslNetworkFunctions
{

int SslLibraryInit()
{
	static Mutex mutex;
	MutexLocker lock(&mutex);
	
	static int isInitialized = 0;

	if(isInitialized == 0)
		return SSL_library_init();
	
	return 0;
}

SSL_CTX* CreateCTX() 
{
	// Load algorithms and error strings.
	OpenSSL_add_all_algorithms();   			// TODO: SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();

	// Compatible with SSLv2, SSLv3 and TLSv1
	// The method describes which SSL protocol we will be using.
	const SSL_METHOD *method = SSLv23_server_method();

	// Create new context from method.
	SSL_CTX* ctx = SSL_CTX_new((SSL_METHOD *)method);
	if(ctx == NULL) 
	{
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SSL_CTX_new(method) ERROR!");
		return NULL;
	}
	return ctx;
}


// Load the certification files, ie the public and private keys. 
void LoadCertificates(SSL_CTX* ctx, const char *cFile, const char *kFile) 
{
	if(SSL_CTX_use_certificate_chain_file(ctx, cFile) <= 0) 
	{
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SSL load certificate error!");
		//_exit(1);
	}

	if(SSL_CTX_use_PrivateKey_file(ctx, kFile, SSL_FILETYPE_PEM) <= 0) 
	{
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SSL load certificate error!");
		//_exit(1);
	}

	// Verify that the two keys goto together.
	if(!SSL_CTX_check_private_key(ctx) ) 
	{
		fprintf(stderr, "Private key is invalid.\n");
		throw std::runtime_error("SSL load certificate error!");
		//_exit(1);
	}
}


int acceptSSLFull(SSL* &ssl, SSL_CTX* ctx, int listen_socket, sockaddr_in &addr)
{
    int conn_sock = NetworkFunctions::my_accept(listen_socket, addr);
	if(conn_sock == -1) return conn_sock;

	return acceptSSLInit(ssl, ctx, conn_sock);
}

int acceptSSLInit(SSL* &ssl, SSL_CTX* ctx, int conn_sock)
{
    ssl = SSL_new(ctx);
    int ret = SSL_set_fd(ssl, conn_sock);
	if(ret == -1)
		IWARNING()  << " ERROR! Could not set SSL_set_fd(ssl, conn_sock)" << endl;
/*
       SSL_accept: The following return values can occur:

       1.  The TLS/SSL handshake was successfully completed, a TLS/SSL connection has been established.

       2.  The TLS/SSL handshake was not successful but was shut down controlled and by the specifications of the TLS/SSL protocol. Call SSL_get_error() with the return
           value ret to find out the reason.

       3.  <0

           The TLS/SSL handshake was not successful because a fatal error occurred either at the protocol level or a connection failure occurred. The shutdown was not
           clean. It can also occur of action is need to continue the operation for non-blocking BIOs. Call SSL_get_error() with the return value ret to find out the
           reason.
*/
	ret = SSL_accept(ssl);
	if(ret != 1) 
	{
		if(ret == 2) 
		{
			cerr << "SSL_accept(ssl): The TLS/SSL handshake was not successful but was shut down controlled and by the specifications of the TLS/SSL protocol. ";
			cerr << "Call SSL_get_error() with the return  value ret to find out the reason. " << endl;
		}
		if(ret < 0)
		{
			cerr << "SSL_accept(ssl): The TLS/SSL handshake was not successful because a fatal error occurred either at the protocol level or a connection failure occurred. ";
			cerr << "The shutdown was not clean. It can also occur of action is need to continue the operation for non-blocking BIOs. ";
			cerr << "Call SSL_get_error() with the return value ret to find out the reason." << endl;
		}
		else
		{
			cerr << "SSL_accept(ssl): Returned " << ret << endl;
		}
		
		//cerr << "ERROR: " << SSL_get_error(ssl, ret) << endl;

		ERR_print_errors_fp(stderr);
		return -1;
	}

	// Get client's certificate (note: beware of dynamic allocation) - opt
	X509* client_cert = SSL_get_peer_certificate (ssl);
	if(client_cert != NULL) 
	{
		printf ("Client certificate:\n");

		char *str = X509_NAME_oneline (X509_get_subject_name(client_cert), 0, 0);
		ASSERT(str);
		printf ("\t subject: %s\n", str);
		OPENSSL_free (str);

		str = X509_NAME_oneline (X509_get_issuer_name(client_cert), 0, 0);
		ASSERT(str);
		printf ("\t issuer: %s\n", str);
		OPENSSL_free(str);

		// We could do all sorts of certificate verification stuff here before deallocating the certificate.
		X509_free(client_cert);
	} 
	else printf ("Client does not have certificate.\n");

	return conn_sock;
}

int connectSSL(SSL* &ssl, SSL_CTX *ctx, const char *name, int port)
{
	int sock = NetworkFunctions::connect(name, port);
	if(sock == -1) return -1;

	// Now we have TCP conncetion. Start SSL negotiation.
	ssl = SSL_new(ctx);
	ASSERT(ssl);

	SSL_set_fd(ssl, sock);
	int ret = SSL_connect(ssl);                     
	if(ret != 1)
	{
		if(ret == 2) 
		{
			cerr << "The TLS/SSL handshake was not successful but was shut down controlled and by the specifications of the TLS/SSL protocol. ";
			cerr << "Call SSL_get_error() with the return  value ret to find out the reason. " << endl;
		}
		if(ret < 0)
		{
			cerr << "The TLS/SSL handshake was not successful because a fatal error occurred either at the protocol level or a connection failure occurred. ";
			cerr << "The shutdown was not clean. It can also occur of action is need to continue the operation for non-blocking BIOs. ";
			cerr << "Call SSL_get_error() with the return value ret to find out the reason." << endl;
		}

		ERR_print_errors_fp(stderr);
		return -1;
	}

	// Following two steps are optional and not required for data exchange to be successful.
	// Get the cipher - opt
	printf ("SSL connection using %s\n", SSL_get_cipher(ssl));

	// Get server's certificate (note: beware of dynamic allocation) - opt
	X509* server_cert = SSL_get_peer_certificate(ssl);       
	if(server_cert == NULL)
	{
		IWARNING()  << "ERROR! SSL_get_peer_certificate(ssl) returned NULL!" << endl;
		return -1;
	}
	printf ("Server certificate:\n");

	char *str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
	ASSERT(str);

	printf ("\t subject: %s\n", str);
	OPENSSL_free(str);

	str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
	ASSERT(str);

	printf ("\t issuer: %s\n", str);
	OPENSSL_free(str);

	// We could do all sorts of certificate verification stuff here before deallocating the certificate.
	X509_free(server_cert);

	return sock;
}

void freeSSL(SSL *ssl)
{
	SSL_free(ssl);
}

void freeCTX(SSL_CTX*ctx)
{
	SSL_CTX_free(ctx);
}


int connectSSL(SSL *ssl)
{
/*
       The following return values can occur:

       1.  The TLS/SSL handshake was successfully completed, a TLS/SSL connection has been established.

       2.  The TLS/SSL handshake was not successful but was shut down controlled and by the specifications of the TLS/SSL protocol. Call SSL_get_error() with the return
           value ret to find out the reason.

       3.  <0

           The TLS/SSL handshake was not successful, because a fatal error occurred either at the protocol level or a connection failure occurred. The shutdown was not
           clean. It can also occur of action is need to continue the operation for non-blocking BIOs. Call SSL_get_error() with the return value ret to find out the
           reason.

*/
	int ret = SSL_connect(ssl);
	if(ret != 1)
	{
		if(ret == 2) 
		{
			cerr << "The TLS/SSL handshake was not successful but was shut down controlled and by the specifications of the TLS/SSL protocol. ";
			cerr << "Call SSL_get_error() with the return  value ret to find out the reason. " << endl;
		}
		if(ret < 0)
		{
			cerr << "The TLS/SSL handshake was not successful because a fatal error occurred either at the protocol level or a connection failure occurred. ";
			cerr << "The shutdown was not clean. It can also occur of action is need to continue the operation for non-blocking BIOs. ";
			cerr << "Call SSL_get_error() with the return value ret to find out the reason." << endl;
		}

		ERR_print_errors_fp(stderr);
		return -1;
	}
	return ret;
}

int getSSLSocket(SSL *ssl)
{
	if(ssl == NULL) return -1;

	return SSL_get_fd(ssl);
}

const SSL_CIPHER* getSSLCipher(SSL *ssl)
{
	char cipdesc[128];
	const SSL_CIPHER *sslciph = SSL_get_current_cipher((SSL *)ssl);
	if(!sslciph)
		throw std::runtime_error("SSL_get_current_cipher(SSL*) failed!");
	
	cout << "Encryption Description: " << endl;
	cout << SSL_CIPHER_description(sslciph, cipdesc, sizeof(cipdesc)) << endl;
	return sslciph;
}

void setSSLServerMode(SSL *ssl)
{
	SSL_set_accept_state(ssl);
}

void setSSLClientMode(SSL *ssl)
{
	SSL_set_connect_state(ssl);
}

//int mySSLRead(SSL *ssl, char *buff, int size)
//{
//	int bytes = SSL_read(ssl, buff, sizeof(buff));
//	buff[bytes] = '\0';	// TODO: Do I need this?
//
//	return bytes;
//}

int mySSLread(SSL *ssl, char *buf, int length)
{
	ASSERT(length > 0);
	int fail_safe = 0;
	int bread = 0, remains = length;
	while(bread < length)
	{
		int r = SSL_read(ssl, (char*)buf + bread, remains);
		if(r > 0) bread = bread + r;
		
		if(r <= 0)
		{
			int ret = SSL_get_error(ssl, r);

			switch(ret)
			{
				case SSL_ERROR_NONE:
					IWARNING()  << "WARNING! strange no error!" << endl;
					break;
				case SSL_ERROR_ZERO_RETURN:
					IWARNING()  << " SSL connection is closed!" << endl;
					return -1;
					break;
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
					IWARNING()  << " waiting for new data to read!" << endl;
					break;
				case SSL_ERROR_SSL:
					IWARNING()  << " SSL ERROR in library! Investigate in SSL error queue!" << endl;
					return -1;
					break;
				default:
					IWARNING()  << " read undefined error : " << ret << endl;
					//return -1;
					break;
			}
		}
		else if(bread < length) 
			remains = length - bread;
		else if(bread == length)
			break;

        Thread::usleep(1000);
		// -- debug --
		//cerr << "%" ;
		ASSERT(bread <= length);
		ASSERT(bread >= 0);
		if(fail_safe++ > 100) 
		{
			IWARNING()  << " fail safe kicked in: failed to read on SSL" << endl;
			return -2;
		}
		// -- debug end --
	}

	ASSERT(bread == length);
	return bread;
}

//int mySSLWrite(SSL *ssl, char *buf, int size)
//{
//	return SSL_write(ssl, buf, size); //strlen(buf));
//}

int mySSLwrite(SSL *ssl, const char *buf, int length)
{
	ASSERT(length > 0);
	int fail_safe = 0;
	int bread = 0, remains = length;
	while(bread < length)
	{
		int r = SSL_write(ssl, (char*)buf + bread, remains);
		if(r > 0) bread = bread + r;
		
		if(r <= 0)
		{
			int ret = SSL_get_error(ssl, r);

			switch(ret)
			{
				case SSL_ERROR_NONE:
					IWARNING()  << "WARNING! strange no error!" << endl;
					break;
				case SSL_ERROR_ZERO_RETURN:
					IWARNING()  << " SSL connection is closed!" << endl;
					return -1;
					break;
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
					IWARNING()  << " waiting to write!" << endl;
					break;
				case SSL_ERROR_SSL:
					IWARNING()  << " SSL ERROR in library! Investigate in SSL error queue!" << endl;
					return -1;
					break;
				default:
					IWARNING()  << " read undefined error : " << ret << endl;
					//return -1;
					break;
			}
		}
		else if(bread < length) 
			remains = length - bread;
		else if(bread == length)
			break;

        Thread::usleep(1000);
		// -- debug --
		//cerr << "%" ;
		ASSERT(bread <= length);
		ASSERT(bread >= 0);
		if(fail_safe++ > 100) 
		{
			IWARNING()  << " fail safe kicked in: failed to write on SSL" << endl;
			return -2;
		}
		// -- debug end --
	}

	ASSERT(bread == length);
	return bread;
}

void closeSSL(SSL* ssl, int sock)
{
	SSL_shutdown((SSL *)ssl);

	// We do not wait for a reply, just clear everything.
	SSL_free((SSL *)ssl);

    UtilityFunctions::CloseSocket(sock);
}

}}


#endif
