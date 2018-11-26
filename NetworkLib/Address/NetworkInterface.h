#pragma once

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Address/NetworkAddressEntry.h"
#include"NetworkLib/Address/HostAddress.h"

#include"NetworkLib/Export.h"

namespace NetworkLib {

namespace InterfaceFlag {
enum Type
{
    IsUp           = 0x1,    //the network interface is active
    IsRunning      = 0x2,    //the network interface has resources allocated
    CanBroadcast   = 0x4,    //the network interface works in broadcast mode
    IsLoopBack     = 0x8,    //the network interface is a loopback interface: that is, it's a virtual interface whose destination is the host computer itself
    IsPointToPoint = 0x10,   //the network interface is a point-to-point interface: that is, there is one, single other address that can be directly reached by it.
    CanMulticast   = 0x20    //the network interface supports multicasting
};
}

class DLL_STATE NetworkInterface
{
public:
    NetworkInterface();

    ~NetworkInterface();

    std::list<NetworkAddressEntry> addressEntries() const;
    //InterfaceFlags	flags() const;

    std::string hardwareAddress() const;
    std::string humanReadableName() const;

    int index() const;

    bool isValid() const;

    std::string name() const;

    friend std::ostream& operator<<(std::ostream& ostr, const NetworkInterface& networkInterface)
    {
        ostr << "(" << networkInterface.interfaceName_ << ", " << networkInterface.addresses_ << ")";
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::list<NetworkInterface>& networkInterfaces)
    {
        typedef std::list<NetworkInterface> NetworkInterfaces;

        for(NetworkInterfaces::const_iterator it = networkInterfaces.begin(), it_end = networkInterfaces.end(); it != it_end; ++it)
        {
            ostr << *it << ", ";
        }

        return ostr;
    }

public:
    typedef std::list<HostAddress>         ListHostAddress;
    typedef std::list<NetworkAddressEntry> ListNetworkAddressEntry;
    typedef std::list<NetworkInterface>    ListNetworkInterface;

public:
    static ListHostAddress allAddresses();

    static ListNetworkInterface allInterfaces();

    static NetworkInterface interfaceFromName(const std::string& name);

private:
    static ListNetworkInterface getNetworkInterfacesAndAddresses(std::string fromName = std::string(""));

#ifdef USE_GCC
    static bool getInterfaceAddress(ifaddrs* ifa, std::map<std::string, NetworkInterface>& mapInterfaces);

    static NetworkAddressEntry getInterfaceAddress(ifaddrs* ifa);
#endif

private:
    std::string             interfaceName_;
    ListNetworkAddressEntry addresses_;
};

}
