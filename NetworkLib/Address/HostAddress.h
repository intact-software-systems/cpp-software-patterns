#ifndef NetworkLib_HostAddress_h_IsIncluded
#define NetworkLib_HostAddress_h_IsIncluded

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Socket/AbstractSocket.h"

#ifdef USE_GCC
#include <getopt.h>
#include <utime.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#endif

#include"NetworkLib/Export.h"
namespace NetworkLib
{

/**
 *
 */
namespace SpecialAddress
{
/**
 * @brief The Type enum
 */
enum Type
{
    Null = 0,           // The null address object. Equivalent to QHostAddress().
    LocalHost = 2,      // The IPv4 localhost address. Equivalent to QHostAddress("127.0.0.1").
    LocalHostIPv6 = 3,  // The IPv6 localhost address. Equivalent to QHostAddress("::1").
    Broadcast = 1,      // The IPv4 broadcast address. Equivalent to QHostAddress("255.255.255.255").
    AnyIPv4 = 6,        // The IPv4 any-address. Equivalent to QHostAddress("0.0.0.0"). A socket bound with this address will listen only on IPv4 interaces.
    AnyIPv6 = 5,        // The IPv6 any-address. Equivalent to QHostAddress("::"). A socket bound with this address will listen only on IPv6 interaces.
    Any = 4             // The dual stack any-address. A socket bound with this address will listen on both IPv4 and IPv6 interfaces.
};
}

/**
 * @brief The HostName class
 */
class DLL_STATE HostAddress
{
public:
    /**
     * @brief HostAddress
     */
    HostAddress();

    /**
     * @brief HostAddress
     * @param address
     */
    HostAddress(std::string address);

    /**
     * @brief HostAddress
     * @param ip4Addr
     */
    HostAddress(uint32 ip4Addr);

    /**
     * @brief HostAddress
     * @param sockaddr
     */
    HostAddress(const sockaddr *sockaddr);

    /**
     * @brief HostAddress
     * @param address
     */
    HostAddress(SpecialAddress::Type address);

    ~HostAddress()
    {}

    CLASS_TRAITS(HostAddress)

public:

    /**
     * @brief SetAddress
     * @param address
     * @return
     */
    bool SetAddress(const std::string &address);

    /**
     * @brief SetAddress
     * @param ip4Addr
     * @return
     */
    bool SetAddress(uint32 ip4Addr);

    /**
     * @brief SetAddress
     * @param sockaddr
     * @return
     */
    bool SetAddress(const sockaddr *sockaddr);

    /**
     * @brief SetAddress
     * @param address
     * @return
     */
    bool SetAddress(SpecialAddress::Type address);

public:
    /**
     * @brief toIPv4Address
     * @return
     */
    uint32 toIPv4Address() const;

    /**
     * @brief toString
     * @return
     */
    std::string toString() const;

    /**
     * @brief isInSubnet
     * @param subnet
     * @param netmask
     * @return
     */
    bool isInSubnet(const HostAddress &subnet, int netmask) const;

    /**
     * @brief isInSubnet
     * @param subnet
     * @return
     */
    bool isInSubnet(const std::pair<HostAddress, int> & subnet) const;

    /**
     * @brief isLoopBack
     * @return
     */
    bool isLoopBack() const;

    /**
     * @brief isNull
     * @return
     */
    bool isNull() const;

    /**
     * @brief isEmpty
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief protocol
     * @return
     */
    AbstractSocket::NetworkLayerProtocol protocol() const;

    /**
     * @brief operator <<
     * @param ostr
     * @param hostAddress
     * @return
     */
    friend std::ostream& operator<<(std::ostream &ostr, const HostAddress &hostAddress)
    {
        ostr << hostAddress.address_;
        return ostr;
    }

    /**
     * @brief operator <<
     * @param ostr
     * @param hostAddresses
     * @return
     */
    friend std::ostream& operator<<(std::ostream &ostr, const std::list<HostAddress> &hostAddresses)
    {
        typedef std::list<HostAddress>      HostAddresses;

        for(HostAddresses::const_iterator it = hostAddresses.begin(), it_end = hostAddresses.end(); it != it_end; ++it)
        {
            ostr << *it << ", ";
        }

        return ostr;
    }

    /**
     * @brief operator ==
     * @param other
     * @return
     */
    bool operator==(const HostAddress &other) const
    {
        if(this->address_ == other.address_)
            return true;

        return false;
    }

    /**
     * @brief operator !=
     * @param other
     * @return
     */
    bool operator!=(const HostAddress &other) const
    {
        return !(*this == other);
    }

private:
    /**
     * @brief address_
     */
    std::string address_;

    /**
     * @brief specialAddress_
     */
    SpecialAddress::Type specialAddress_;
};

}

#endif // NetworkLib_HostAddress_h_IsIncluded
