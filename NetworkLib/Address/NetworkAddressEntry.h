#pragma once

#include"NetworkLib/Address/HostAddress.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE NetworkAddressEntry
{
public:
    NetworkAddressEntry();
    ~NetworkAddressEntry();

    HostAddress broadcast() const;
    HostAddress ip() const;
    HostAddress netmask() const;

    int prefixLength() const;

    void setBroadcast(const HostAddress& newBroadcast);
    void setIp(const HostAddress& newIp);
    void setNetmask(const HostAddress& newNetmask);
    void setPrefixLength(int length);
    void swap(NetworkAddressEntry& other);

    bool isEmpty() const;

    bool operator!=(const NetworkAddressEntry& other) const;
    //NetworkAddressEntry &operator=(const NetworkAddressEntry & other);
    bool operator==(const NetworkAddressEntry& other) const;

    friend std::ostream& operator<<(std::ostream& ostr, const NetworkAddressEntry& networkAddress)
    {
        ostr << "(" << networkAddress.ip_ << "," << networkAddress.netMask_ << "," << networkAddress.broadcast_ << "," << networkAddress.prefixLength_ << ")";
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const std::list<NetworkAddressEntry>& networkAddresses)
    {
        typedef std::list<NetworkAddressEntry> ListNetworkAddress;

        for(ListNetworkAddress::const_iterator it = networkAddresses.begin(), it_end = networkAddresses.end(); it != it_end; ++it)
        {
            ostr << *it << ", ";
        }
        return ostr;
    }

private:
    HostAddress ip_;
    HostAddress netMask_;
    HostAddress broadcast_;
    int         prefixLength_;
};

}
