#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/CommonDefines.h"

#include"NetworkLib/Export.h"

namespace NetworkLib {

/**
 TODO:
 Let classes TcpServer, TcpSocket, SslServer, SslSocket share this as a base class
		-> let each of these classes implement their own read/write functions (etc)
 		-> let each of these classes reimplement whatever is necessary

 TODO:
    - Use HostAddress
    - Connection strategies (NAT traversal)
*/

class DLL_STATE AbstractSocket
{
public:
    enum SocketType
    {
        SocketError       = -1,
        UnknownSocketType = 0,
        TcpSocket,
        UdpSocket
        // TODO: Any way of figuring out this?
        //UdpUnicastSocket,
        //UdpBroadcastSocket,
        //UdpMulticastSocket
    };

    enum SocketMode
    {
        UnencryptedMode = 0,
        SslClientMode,
        SslServerMode
    };

    /**
     * @brief The NetworkLayerProtocol enum
     */
    enum NetworkLayerProtocol
    {
        IPv4Protocol                = 0,              // IPv4
        IPv6Protocol                = 1,              // IPv6
        AnyIPProtocol               = 2,              // Either IPv4 or IPv6
        UnknownNetworkLayerProtocol = -1    // Other than IPv4 and IPv6
    };

    /**
     * @brief The SocketState enum
     */
    enum SocketState
    {
        UnconnectedState = 0,    // The socket is not connected.
        HostLookupState  = 1,    // The socket is performing a host name lookup.
        ConnectingState  = 2,    // The socket has started establishing a connection.
        ConnectedState   = 3,    // A connection is established.
        BoundState       = 4,    // The socket is bound to an address and port.
        ClosingState     = 6,    // The socket is about to close (data may still be waiting to be written).
        ListeningState   = 5    // For internal use only.
    };

public:
    AbstractSocket(SocketMode mode = AbstractSocket::UnencryptedMode);
    AbstractSocket(int sock, std::string hostName, int port, SocketMode type = AbstractSocket::UnencryptedMode);
    AbstractSocket(int sock, SocketMode mode = AbstractSocket::UnencryptedMode);

    virtual ~AbstractSocket();

    CLASS_TRAITS(AbstractSocket)

    // --------------------------------------------
    // TODO: Implement these here
    //       Decide a connection strategy as an input enumerated value, or as a template
    // --------------------------------------------
    //virtual void	connectToHost(const QString & hostName, quint16 port, OpenMode openMode = ReadWrite, NetworkLayerProtocol protocol = AnyIPProtocol);
    //virtual void	connectToHost(const QHostAddress & address, quint16 port, OpenMode openMode = ReadWrite);
    //virtual void	disconnectFromHost();

public:
    // --------------------------------------------
    // virtual functions that may be reimplemented
    // --------------------------------------------

    virtual int Write(const char* bytes, const int& length);

    virtual int Read(char* bytes, const int& length);

    // --------------------------------------------
    // virtual functions that must be reimplemented to give any functionality!
    // To enable UdpSockets sending on a hostAddress
    // --------------------------------------------

    virtual int ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port);

    virtual int WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port);

    // --------------------------------------------
    // API
    // --------------------------------------------

    virtual void disconnectFromHost();
    virtual void close();
    virtual bool flush();
    virtual int state() const;
    virtual int64 bytesAvailable() const;
    virtual bool waitForReadyRead(unsigned int milliseconds);
    virtual AbstractSocket::SocketType socketType() const;

    virtual int GetReceiveBufferSize();
    virtual int GetSendBufferSize();

    virtual bool isValid();
    virtual bool isBound();
    virtual bool setSocketDescriptor(int socket);

    static std::string GetLocalHostName();

    std::string peerAddress();
    int peerPort();


    bool operator==(const AbstractSocket& other)
    {
        return socketDescriptor_ == other.socketDescriptor_;
    }

    bool operator!=(const AbstractSocket& other)
    {
        return !(*this == other);
    }

    bool operator<(const AbstractSocket& other)
    {
        return socketDescriptor_ < other.socketDescriptor_;
    }

    bool operator<=(const AbstractSocket& other)
    {
        return socketDescriptor_ <= other.socketDescriptor_;
    }

    bool operator>(const AbstractSocket& other)
    {
        return socketDescriptor_ > other.socketDescriptor_;
    }

    bool operator>=(const AbstractSocket& other)
    {
        return socketDescriptor_ >= other.socketDescriptor_;
    }

    // --------------------------------------------
    // inline getters
    // --------------------------------------------

    inline int GetPortNumber() const { return portNumber_; }

    inline std::string GetHostName() const { return hostName_; }

    inline int GetSocket() const { return socketDescriptor_; }

    inline int socketDescriptor() const { return socketDescriptor_; }

    std::string errorString() const { return std::string(); }

    AbstractSocket::SocketMode mode() const { return socketMode_; }

public:

    friend std::ostream& operator<<(std::ostream& ostr, const std::shared_ptr<AbstractSocket>& socket)
    {
        if(!socket)
        {
            ostr << "NULL";
            return ostr;
        }

        ostr << "AbstractSocket(fd: " << socket->socketDescriptor_ << ", port:" << socket->portNumber_ << " host:" << socket->hostName_ << ")";
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const AbstractSocket& socket)
    {
        ostr << "(fd:" << socket.socketDescriptor_ << ", port:" << socket.portNumber_ << " host:" << socket.hostName_ << ")";
        return ostr;
    }


protected:
    int         portNumber_;
    std::string hostName_;
    int         socketDescriptor_;
    SocketMode  socketMode_;
};

typedef BaseLib::Collection::ISet<std::shared_ptr<AbstractSocket> > SetSocket;

}
