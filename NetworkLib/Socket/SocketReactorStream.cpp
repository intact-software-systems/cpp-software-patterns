#include "NetworkLib/Socket/SocketReactorStream.h"

namespace NetworkLib
{

SocketReactorStream::SocketReactorStream()
{
}

SocketReactorStream::~SocketReactorStream()
{

}

int SocketReactorStream::Read(char *bytes, const int &length)
{
    IWARNING() << "Not implemented!";
    return length;
}

int SocketReactorStream::ReadDatagram(char *bytes, const int &length, std::string &hostAddress, int &port)
{
    IWARNING() << "Not implemented!";
    return length;
}

int SocketReactorStream::Write(const char *bytes, const int &length)
{
    IWARNING() << "Not implemented!";
    return length;
}

int SocketReactorStream::WriteDatagram(const char *bytes, const int &length, const std::string &hostAddress, const int &port)
{
    IWARNING() << "Not implemented!";
    return length;
}

void SocketReactorStream::Close()
{
    IWARNING() << "Not implemented!";
}

bool SocketReactorStream::Flush()
{
    IWARNING() << "Not implemented!";
    return true;
}

}

