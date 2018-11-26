#pragma once

#include<NetworkLib/IncludeExtLibs.h>
#include<NetworkLib/Serialize/ByteArray.h>
#include<NetworkLib/Message/Datagram.h>
#include<NetworkLib/Socket/AbstractSocket.h>
#include<NetworkLib/Socket/TcpServer.h>
#include<NetworkLib/Socket/SocketSelectorPool.h>
#include<NetworkLib/Socket/TcpSocket.h>
#include<NetworkLib/Export.h>

namespace NetworkLib {

struct DLL_STATE SocketAlgorithms
{
    virtual SetSocket AcceptSockets(SocketSelector& selector, int64 msecs = LONG_MAX)
    {
        SetSocket spawnedSockets;

        for(std::shared_ptr<AbstractSocket> socket : selector.Select(msecs))
        {
            std::shared_ptr<TcpServer> tcpServer = std::dynamic_pointer_cast<TcpServer>(socket);
            ASSERT(tcpServer);

            TcpSocket* tcpSocket = tcpServer->acceptTcpSocket();

            if(tcpSocket)
            {
                spawnedSockets.insert(std::shared_ptr<AbstractSocket>(tcpSocket));
            }
        }

        return spawnedSockets;
    }

    // TODO: Should I return datagrams, or just enter them into buffer?
    template <typename STREAM, typename BUFFER>
    static int Perform(STREAM socketStream, BUFFER dataBuffer, BaseLib::Policy::SizeLimits limits_)
    {
        typedef std::vector<char>                                                                 CharVector;
        typedef NetworkLib::Datagram<NetworkLib::ByteArray::Ptr, NetworkLib::ByteArray::Ptr>::Ptr UDPDatagram;

        try
        {
            int   totalBytesRead = 0;
            int64 datagramSize   = limits_.GetMaxBytesPerInstance();

            while(totalBytesRead < limits_.GetMaxBytes())
            {
                // -------------------------------------
                // Stream and buffer checks
                // -------------------------------------
                {
                    if(socketStream->bytesAvailable() < datagramSize)
                    {
                        IDEBUG() << "Bytes available less than datagram size: " << socketStream->bytesAvailable() << " < " << datagramSize;
                        break;
                    }
                    else if(!dataBuffer->HasCapacity())
                    {
                        IWARNING() << "Backing off. Buffer capacity less than datagram size:" << dataBuffer->Capacity() << " < " << datagramSize;
                        break;
                    }
                }

                // -------------------------------------
                // All prechecks ok, read into buffer.
                // -------------------------------------

                CharVector dataChunk(datagramSize);

                int bytesRead = socketStream->Read(&dataChunk[0], datagramSize);

                if(bytesRead != datagramSize)
                {
                    ICRITICAL() << "Read " << bytesRead << " expected " << datagramSize;
                    ASSERT(bytesRead == datagramSize);

                    break;
                }
                else
                {
                    UDPDatagram datagram;//(new UDPDatagram(dataChunk));
                    ASSERT(datagram);

                    bool isEnqueued = dataBuffer->template Enqueue<UDPDatagram>(datagram);
                    ASSERT(isEnqueued);

                    totalBytesRead += bytesRead;
                }
            }

            return totalBytesRead;
        }
        catch(Exception ex)
        {
            IWARNING() << "Exception caught: " << ex.what();
        }

        return -1;
    }

};

}
