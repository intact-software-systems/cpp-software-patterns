#ifndef NetworkLib_SocketStream_h_IsIncluded
#define NetworkLib_SocketStream_h_IsIncluded

#include "NetworkLib/IncludeExtLibs.h"
#include "NetworkLib/Socket/AbstractSocket.h"
#include "NetworkLib/Socket/IOBase.h"
#include "NetworkLib/Export.h"

namespace NetworkLib
{

/**
 * @brief The SocketStream class is a thread-safe adapter to read from a socket, this implies that
 * multiple threads may read concurrently from this class.
 */
class DLL_STATE SocketStream : public IOReaderWriter
                             , public ENABLE_SHARED_FROM_THIS(SocketStream)
{
public:
    SocketStream(AbstractSocket::Ptr abstractSocket);
	virtual ~SocketStream();

    CLASS_TRAITS(SocketStream)

    SocketStream::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

public:
    virtual int Read(char* bytes, const int &length);
	virtual int ReadDatagram(char* bytes, const int &length, std::string &hostAddress, int &port);

	virtual int Write(const char* bytes, const int &length);
	virtual int WriteDatagram(const char* bytes, const int &length, const std::string &hostAddress, const int &port);

	virtual void Close();
	virtual bool Flush();

public:
	int Available() const;
    bool IsConnected() const;
	bool IsBound() const;

	int socketDescriptor() const;
	AbstractSocket::SocketType GetSocketType() const;

private:
	int readDefault(char *bytes, const int &length);
	int writeDefault(const char* bytes, const int &length);

	int readDefault(char *bytes, const int &length, std::string &hostAddress, int &port);
	int writeDefault(const char *bytes, const int &length, const std::string &hostAddress, const int &port);

private:
	bool 					isClosed_;
	AbstractSocket::Ptr     abstractSocket_;

private:
    mutable Mutex			mutexLock_;
	mutable Mutex			readMutex_;
	mutable Mutex			writeMutex_;
};

/**
 *  - Make Observable by deserializers
 *  - How to have a list of interceptors that all have to pass for the data to be allowed to enter?
 *  - Authorization pattern? Interceptor pattern?
 *  - Alternative name? SocketReaderRunnable
 */

/**
 * @brief The SocketReaderRunnable class takes as input a thread-safe SocketStream pointer to read
 * data from and a ring-buffer pointer to enqueue data into.
 *
 * The advantage of this implementation is that it is thread-safe which implies that multiple runnables
 * may share the same socket to read from and the same buffer to read into.
 *
 * TODO:
 *  - ReadNumDataPolicy : BYTES_AVAILABLE/STREAM, DATAGRAM_SIZE.
 *  - Policy on enqueue waiting time: FOREVER, MSECS
 *  - Observable? OnBytesRead(int bytesRead)
 *  - Other implementation for UDP? Use ReadDataGram
 */
template <typename STREAM, typename BUFFER>
class SocketReaderRunnable : public Runnable
                           , public ENABLE_SHARED_FROM_THIS_T2(SocketReaderRunnable, STREAM, BUFFER)
{
private:
    typedef std::vector<char>   CharVector;

public:
    SocketReaderRunnable(STREAM socketStream, BUFFER dataBuffer)
        : socketStream_(socketStream)
        , dataBuffer_(dataBuffer)
    {}
    ~SocketReaderRunnable()
    {}

    CLASS_TRAITS(SocketReaderRunnable)

    typename SocketReaderRunnable::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual void run()
    {
        int bytesAvailable = socketStream_->bytesAvailable();
        if(bytesAvailable <= 0) return;

        try
        {
            CharVector dataChunk(bytesAvailable);

            int bytesRead = socketStream_->Read(&dataChunk[0], bytesAvailable);

            // -- debug --
            if(bytesRead != bytesAvailable)
                ICRITICAL() << "Read " << bytesRead << " expected " << bytesAvailable;
            // -- debug --

            if(bytesRead > 0)
            {
                bool isEnqueued = dataBuffer_-> template Enqueue<CharVector>(dataChunk);
                ASSERT(isEnqueued);
            }
        }
        catch(Exception ex)
        {
            IWARNING() << "Exception caught: " << ex.what();
        }
    }

private:
    STREAM   socketStream_;
    BUFFER   dataBuffer_;
};

/**
 * @brief ConcurrentSocketReader takes as input a thread-safe SocketStream pointer to read
 * data from and a ring-buffer pointer to enqueue data into.
 *
 * The advantage of this implementation is that it is thread-safe which implies that multiple runnables
 * may share the same socket to read from and the same buffer to read into.
 */
typedef SocketReaderRunnable<IOReader::Ptr, RingBufferQueue<char, ReadWriteLock>::Ptr>       ConcurrentSocketReader;
typedef SocketReaderRunnable<AbstractSocket::Ptr, RingBufferQueue<char, ReadWriteLock>::Ptr> RawSocketReader;

/**
 * TODO:
 *  - ReadNumDataPolicy : BYTES_AVAILABLE/STREAM, DATAGRAM_SIZE.
 *  - Policy on enqueue waiting time: FOREVER, MSECS
 *  - Observable? OnBytesRead(int bytesRead)
 *  - Other implementation for UDP? Use ReadDataGram
 *  - Use with MulticastSocketStream.h
 */
template <typename STREAM, typename BUFFER>
class SocketWriterRunnable : public Runnable
                           , public ENABLE_SHARED_FROM_THIS_T2(SocketWriterRunnable, STREAM, BUFFER)
{
private:
    /**
     * @brief CharVector
     */
    typedef std::vector<char>   CharVector;

public:
    SocketWriterRunnable(STREAM socketStream, BUFFER dataBuffer)
        : socketStream_(socketStream)
        , dataBuffer_(dataBuffer)
    {}
    ~SocketWriterRunnable()
    {}

    CLASS_TRAITS(SocketWriterRunnable)

    typename SocketWriterRunnable::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual void run()
    {
        CharVector dataChunk = dataBuffer_-> template PeekAll<CharVector>();
        if(dataChunk.empty()) return;

        try
        {
            int bytesWritten = socketStream_->Write(&dataChunk[0], dataChunk.size());

            // -- debug --
            if(bytesWritten != dataChunk.size())
                ICRITICAL() << "Wrote " << bytesWritten << " expected " << dataChunk.size();
            // -- debug --

            dataBuffer_->Pop(bytesWritten);
        }
        catch(Exception ex)
        {
            IWARNING() << "Exception caught: " << ex.what();
        }
    }

private:
    STREAM   socketStream_;
    BUFFER   dataBuffer_;
};

/**
 * @brief ConcurrentSocketWriter
 */
typedef SocketWriterRunnable<IOReader::Ptr, RingBufferQueue<char, ReadWriteLock>::Ptr>       ConcurrentSocketWriter;
typedef SocketWriterRunnable<AbstractSocket::Ptr, RingBufferQueue<char, ReadWriteLock>::Ptr> RawSocketWriter;

}

#endif


