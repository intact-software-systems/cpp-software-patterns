#include "NetworkLib/Address/NetworkAddressEntry.h"

namespace NetworkLib {

NetworkAddressEntry::NetworkAddressEntry()
    : prefixLength_(0)
{
}

NetworkAddressEntry::~NetworkAddressEntry()
{
}

HostAddress NetworkAddressEntry::broadcast() const
{
    return broadcast_;
}

HostAddress NetworkAddressEntry::ip() const
{
    return ip_;
}

HostAddress NetworkAddressEntry::netmask() const
{
    return netMask_;
}

int NetworkAddressEntry::prefixLength() const
{
    return prefixLength_;
}

void NetworkAddressEntry::setBroadcast(const HostAddress& newBroadcast)
{
    broadcast_ = newBroadcast;
}

void NetworkAddressEntry::setIp(const HostAddress& newIp)
{
    ip_ = newIp;
}

void NetworkAddressEntry::setNetmask(const HostAddress& newNetmask)
{
    netMask_ = newNetmask;
}

void NetworkAddressEntry::setPrefixLength(int length)
{
    prefixLength_ = length;
}

void NetworkAddressEntry::swap(NetworkAddressEntry& other)
{
    NetworkAddressEntry cp = *this;

    *this = other;

    other = cp;
}

bool NetworkAddressEntry::isEmpty() const
{
    return this->ip_.isEmpty();
}

bool NetworkAddressEntry::operator!=(const NetworkAddressEntry& other) const
{
    return !(*this == other);
}

//NetworkAddressEntry& NetworkAddressEntry::operator =(const NetworkAddressEntry &other)
//{
//    return *this;
//}

bool NetworkAddressEntry::operator==(const NetworkAddressEntry& other) const
{
    return this->ip_ == other.ip_ &&
           this->broadcast_ == other.broadcast_ &&
           this->netMask_ == other.netMask_ &&
           this->prefixLength_ == other.prefixLength_;

}

}
