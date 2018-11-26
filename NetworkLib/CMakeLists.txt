cmake_minimum_required(VERSION 3.2)
project(NetworkLib)

add_definitions(-DMAKE_DLL_NetworkLib)

SET(LIB_NAME NetworkLib)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})
SET(EXTRA_LIBS BaseLib RxData DataReactor)

set(SOURCE_FILES
    Address/HostAddress.cpp
    Address/HostAddress.h
    Address/IncludeLibs.h
    Address/NetworkAddressEntry.cpp
    Address/NetworkAddressEntry.h
    Address/NetworkInterface.cpp
    Address/NetworkInterface.h
    Message/Datagram.h
    Message/IncludeLibs.h
    Message/MessageBase.h
    Serialize/ByteArray.cpp
    Serialize/ByteArray.h
    Serialize/InByteArray.cpp
    Serialize/InByteArray.h
    Serialize/IncludeLibs.h
    Serialize/InputArray.h
    Serialize/NetObjectBase.h
    Serialize/OutputArray.h
    Serialize/SerializeBase.h
    Serialize/SerializerController.cpp
    Serialize/SerializerController.h
    Serialize/SocketReader.cpp
    Serialize/SocketReader.h
    Serialize/SocketReaderFunctions.cpp
    Serialize/SocketReaderFunctions.h
    Serialize/SocketWriter.cpp
    Serialize/SocketWriter.h
    Serialize/SocketWriterFunctions.cpp
    Serialize/SocketWriterFunctions.h
    Socket/AbstractSocket.cpp
    Socket/AbstractSocket.h
    Socket/DatagramReader.h
    Socket/IncludeLibs.h
    Socket/IOBase.h
    Socket/MulticastSocketStream.cpp
    Socket/MulticastSocketStream.h
    Socket/Pipe.cpp
    Socket/Pipe.h
    Socket/SocketAcceptor.h
    Socket/SocketObserverBase.h
    Socket/SocketQosPolicy.h
    Socket/SocketReactorBase.h
    Socket/SocketReactorStream.cpp
    Socket/SocketReactorStream.h
    Socket/SocketSelectorPool.h
    Socket/SocketStatus.h
    Socket/SocketStream.cpp
    Socket/SocketStream.h
    Socket/SslServer.cpp
    Socket/SslServer.h
    Socket/SslSocket.cpp
    Socket/SslSocket.h
    Socket/TcpServer.cpp
    Socket/TcpServer.h
    Socket/TcpSocket.cpp
    Socket/TcpSocket.h
    Socket/UdpBase.cpp
    Socket/UdpBase.h
    Socket/UdpReceiver.cpp
    Socket/UdpReceiver.h
    Socket/UdpSender.cpp
    Socket/UdpSender.h
    Socket/UdpSocket.cpp
    Socket/UdpSocket.h
    Utility/EndianConverter.h
    Utility/IncludeLibs.h
    Utility/NATNetworkFunctions.cpp
    Utility/NATNetworkFunctions.h
    Utility/NetworkFunctions.cpp
    Utility/NetworkFunctions.h
    Utility/SslNetworkFunctions.cpp
    Utility/SslNetworkFunctions.h
    Utility/UtilityFunctions.cpp
    Utility/UtilityFunctions.h
    BufferedSocketReader.h
    BufferedSocketWriter.h
    BufferedTcpSocketReader.cpp
    BufferedTcpSocketReader.h
    BufferedTcpSocketWriter.cpp
    BufferedTcpSocketWriter.h
    BufferedUdpSocketReader.cpp
    BufferedUdpSocketReader.h
    BufferedUdpSocketWriter.cpp
    BufferedUdpSocketWriter.h
    CommonDefines.h
    ConnectionManager.cpp
    ConnectionManager.h
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h
    NetAddress.cpp
    NetAddress.h Socket/SocketAlgorithms.cpp Socket/SocketAlgorithms.h)

find_library(OPENSSL_LIB ssl $ENV{OPENSSL_LIB_PATH})

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS} ${OPENSSL_LIB})
