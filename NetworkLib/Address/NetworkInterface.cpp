#include "NetworkLib/Address/NetworkInterface.h"
#include "NetworkLib/Utility/NetworkFunctions.h"
#include "NetworkLib/Utility/UtilityFunctions.h"

namespace NetworkLib {

NetworkInterface::NetworkInterface()
{
}

NetworkInterface::~NetworkInterface()
{
}

std::list<NetworkAddressEntry> NetworkInterface::addressEntries() const
{
    return std::list<NetworkAddressEntry>();
}

std::string NetworkInterface::hardwareAddress() const
{
    return interfaceName_;
}

std::string NetworkInterface::humanReadableName() const
{
    return interfaceName_;
}

int NetworkInterface::index() const
{
    return 0;
}

bool NetworkInterface::isValid() const
{
    if(interfaceName_.empty()) return false;
    return !addresses_.empty();

}

std::string NetworkInterface::name() const
{
    return interfaceName_;
}

std::list<HostAddress> NetworkInterface::allAddresses()
{
    typedef std::list<std::string> ListAddresses;
    typedef std::list<HostAddress> HostAddresses;

    ListAddresses addresses        = NetworkFunctions::getLocalIPs(false);

    HostAddresses               hostAddresses;
    for(ListAddresses::iterator it = addresses.begin(), it_end = addresses.end(); it != it_end; ++it)
    {
        HostAddress address(*it);
        hostAddresses.push_back(address);
    }

    return hostAddresses;
}

NetworkInterface::ListNetworkInterface NetworkInterface::allInterfaces()
{
    return NetworkInterface::getNetworkInterfacesAndAddresses();
}

NetworkInterface NetworkInterface::interfaceFromName(const std::string& name)
{
    ListNetworkInterface interfaces = NetworkInterface::getNetworkInterfacesAndAddresses(name);

    if(!interfaces.empty())
    {
        ASSERT(interfaces.size() == 1);

        *(interfaces.begin());
    }

    return NetworkInterface();
}

// --------------------------------------------------------------------------------
// Private static helper functions
// --------------------------------------------------------------------------------

/**
 * @brief NetworkInterface::getNetworkInterfacesAndAddresses
 *
 * @param fromName can be empty, if not empty then only retrieve addresses from given interface
 * @return list of network interfaces and addresses
 */
NetworkInterface::ListNetworkInterface NetworkInterface::getNetworkInterfacesAndAddresses(std::string fromName)
{
    ListNetworkInterface interfaces;

#ifdef USE_GCC
    struct ifaddrs* ifa, * results;

    if(getifaddrs(&results) < 0)
    {
        IDEBUG() << "getifaddrs returned error!" << strerror(errno);
        return interfaces;
    }

    typedef std::map<std::string, NetworkInterface> MapInterface;

    MapInterface mapInterfaces;

    // -------------------------------------------------------------
    // Loop and get each interface the system has, one by one...
    // Some interfaces are the same because several IPs belong to same interface
    // -------------------------------------------------------------
    for(ifa = results; ifa; ifa = ifa->ifa_next)
    {
        // no ip address from interface that is down
        if((ifa->ifa_flags & IFF_UP) == 0)
        {
            continue;
        }

        if(ifa->ifa_addr == NULL)
        {
            continue;
        }

        if(ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
        {
            continue;
        }

        // --------------------------------------
        // If fromName not empty then check if ifName == fromName
        // --------------------------------------
        if(!fromName.empty())
        {
            std::string ifName = ifa->ifa_name;

            if(fromName.compare(ifName) != 0)
            {
                continue;
            }
        }

        bool isAdded = NetworkInterface::getInterfaceAddress(ifa, mapInterfaces);
        if(!isAdded)
        {
            IWARNING() << "Couldn't find any addresses for interface " << ifa->ifa_name;
        }
    }

    for(MapInterface::iterator it = mapInterfaces.begin(), it_end = mapInterfaces.end(); it != it_end; ++it)
    {
        interfaces.push_back(it->second);
    }

    freeifaddrs(results);

#elif WIN32
    /*
     Check out: http://msdn.microsoft.com/en-us/library/aa365915.aspx
      */
    ULONG               ulOutBufLen;
    DWORD               dwRetVal;
    unsigned int       i;

    IP_INTERFACE_INFO*  pInterfaceInfo;

    pInterfaceInfo = (IP_INTERFACE_INFO *) malloc(sizeof (IP_INTERFACE_INFO));
    ulOutBufLen = sizeof(IP_INTERFACE_INFO);

    if(GetInterfaceInfo(pInterfaceInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(pInterfaceInfo);
        pInterfaceInfo = (IP_INTERFACE_INFO *) malloc(ulOutBufLen);
    }

    if((dwRetVal = GetInterfaceInfo(pInterfaceInfo, &ulOutBufLen)) != NO_ERROR)
    {
        IDEBUG() << "GetInterfaceInfo failed with error: " << dwRetVal;
    }

    //iDebug("GetInterfaceInfo succeeded");
    //iDebug("Num Adapters: %u\n", pInterfaceInfo->NumAdapters);

    for(i = 0; i < (unsigned int) pInterfaceInfo->NumAdapters; i++)
    {
        //iDebug("Adapter Index[%d]: %u\n", i, pInterfaceInfo->Adapter[i].Index);
        //iDebug("Adapter Name[%d]:  %ws\n\n", i, pInterfaceInfo->Adapter[i].Name);

        char name[sizeof(pInterfaceInfo->Adapter[i].Name) + 1];

        sprintf(name, "%ws", pInterfaceInfo->Adapter[i].Name);

        //IDEBUG() << "Interface name " << name;

        //interfaces.push_front(name);
    }

    if(pInterfaceInfo)
    {
        free(pInterfaceInfo);
        pInterfaceInfo = NULL;
    }

#endif
    return interfaces;

}

#ifdef USE_GCC

bool NetworkInterface::getInterfaceAddress(struct ifaddrs* ifa, std::map<std::string, NetworkInterface>& mapInterfaces)
{
    ASSERT(ifa);

    typedef std::map<std::string, NetworkInterface> MapInterface;

    std::string ifName = ifa->ifa_name;

    // ---------------------------------------
    // Get addresses from ifName
    // ---------------------------------------
    MapInterface::iterator entry = mapInterfaces.find(ifName);

    if(entry == mapInterfaces.end())
    {
        NetworkInterface networkInterface;
        networkInterface.interfaceName_ = ifName;

        mapInterfaces.insert(std::pair<std::string, NetworkInterface>(ifName, networkInterface));
    }

    NetworkAddressEntry networkAddress = NetworkInterface::getInterfaceAddress(ifa);

    if(networkAddress.isEmpty())
    {
        return false;
    }

    mapInterfaces[ifName].addresses_.push_back(networkAddress);
    return true;
}

NetworkAddressEntry NetworkInterface::getInterfaceAddress(struct ifaddrs* ifa)
{
    ASSERT(ifa);

    NetworkAddressEntry networkAddress;

    // -----------------
    // IP
    // -----------------
    if(ifa->ifa_addr)
    {
        HostAddress newIp(UtilityFunctions::GetAddress(ifa->ifa_addr));
        networkAddress.setIp(newIp);
    }

    // -----------------
    // Netmask
    // -----------------
    if(ifa->ifa_netmask)
    {
        HostAddress newNetmask(UtilityFunctions::GetAddress(ifa->ifa_netmask));
        networkAddress.setNetmask(newNetmask);
    }

    // -----------------
    // Broadcast or P2P address
    // -----------------
    if(ifa->ifa_dstaddr)
    {
        HostAddress newBroadcast(UtilityFunctions::GetAddress(ifa->ifa_dstaddr));
        networkAddress.setBroadcast(newBroadcast);
    }

    return networkAddress;
}

#endif

}
