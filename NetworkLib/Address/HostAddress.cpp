#include"NetworkLib/Address/HostAddress.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"

namespace NetworkLib
{

/**
 * @brief HostAddress::HostAddress
 */
HostAddress::HostAddress()
    : address_()
    , specialAddress_(SpecialAddress::Null)
{
}

/**
 * @brief HostAddress::HostAddress
 * @param address
 */
HostAddress::HostAddress(std::string address)
    : address_(address)
    , specialAddress_(SpecialAddress::Null)
{
}

/**
 * @brief HostAddress::HostAddress
 * @param ip4Addr
 */
HostAddress::HostAddress(uint32 ip4Addr)
    : address_()
    , specialAddress_(SpecialAddress::Null)
{
    SetAddress(ip4Addr);
}

/**
 * @brief HostAddress::HostAddress
 * @param sockaddr
 */
HostAddress::HostAddress(const sockaddr *sockaddr)
    : address_()
    , specialAddress_(SpecialAddress::Null)
{
    SetAddress(sockaddr);
}

/**
 * @brief HostAddress::HostAddress
 * @param address
 */
HostAddress::HostAddress(SpecialAddress::Type address)
    : address_()
    , specialAddress_(address)
{
}


/**
 * @brief HostAddress::SetAddress
 * @param address
 * @return
 */
bool HostAddress::SetAddress(const std::string &address)
{
    //NetworkFunctions::isIPAddressOnHost(address);

    this->address_ = address;

    return true;
}

/**
 * @brief HostAddress::SetAddress
 * @param ip4Addr
 * @return
 */
bool HostAddress::SetAddress(uint32 ip4Addr)
{
    struct in_addr addr;
    addr.s_addr = ip4Addr;

    char addr_as_string[INET_ADDRSTRLEN+1];

    if(inet_ntop (AF_INET, &addr, addr_as_string, INET_ADDRSTRLEN) == NULL)
        return false;

    address_ = std::string(addr_as_string);

    return true;
}

/**
 * @brief HostAddress::SetAddress
 * @param sockaddr
 * @return
 */
bool HostAddress::SetAddress(const sockaddr *addr)
{
    address_ = UtilityFunctions::GetAddress(addr);

    if(address_.empty())
        return false;

    return true;
}

/**
 * @brief HostAddress::SetAddress
 * @param address
 * @return
 */
bool HostAddress::SetAddress(SpecialAddress::Type address)
{
    return true;
}

/**
 * @brief HostAddress::toIPv4Address
 * @return
 */
uint32 HostAddress::toIPv4Address() const
{
    return 0;
}

/**
 * @brief HostAddress::toString
 * @return
 */
std::string HostAddress::toString() const
{
    return address_;
}

/**
 * @brief HostAddress::isInSubnet
 * @param subnet
 * @param netmask
 * @return
 */
bool HostAddress::isInSubnet(const HostAddress &subnet, int netmask) const
{
    return true;
}

/**
 * @brief HostAddress::isInSubnet
 * @param subnet
 * @return
 */
bool HostAddress::isInSubnet(const std::pair<HostAddress, int> &subnet) const
{
    return true;
}

/**
 * @brief HostAddress::isLoopBack
 * @return
 */
bool HostAddress::isLoopBack() const
{
    return true;
}

/**
 * @brief HostAddress::isNull
 * @return
 */
bool HostAddress::isNull() const
{
    return true;
}

/**
 * @brief HostAddress::isEmpty
 * @return
 */
bool HostAddress::isEmpty() const
{
    return address_.empty();
}

/**
 * @brief HostAddress::protocol
 * @return
 */
AbstractSocket::NetworkLayerProtocol HostAddress::protocol() const
{
    return AbstractSocket::IPv4Protocol;
}

}
