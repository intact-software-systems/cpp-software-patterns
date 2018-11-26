/*
 * Locator.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_Locator_h_IsIncluded
#define RTPS_Elements_Locator_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type Locator_t

struct Locator_t
{
	long kind;
	int64 port;
	octet[16] address;
};

typedef Locator_t Locator;

If the Locator_t kind is LOCATOR_KIND_UDPv4, the address contains an IPv4 address. In this
case the leading 12 octets of the address must be zero. The last 4 octets are used to store the
IPv4 address. The mapping between the dot-notation “a.b.c.d” of an IPv4 address and its
representation in the address field of a Locator_t is:
address = (0,0,0,0,0,0,0,0,0,0,0,0,a,b,c,d}
If the Locator_t kind is LOCATOR_KIND_UDPv6, the address contains an IPv6 address. IPv6
addresses typically use a shorthand hexadecimal notation that maps one-to-one to the 16
octets in the address field. For example the representation of the IPv6 address
“FF00:4501:0:0:0:0:0:32” is:
address = (0xff,0,0x45,0x01,0,0,0,0,0,0,0,0,0,0,0,0x32}

Mapping of the reserved values:
---------------------------------------------------------------------------------------------*/
//#define LOCATOR_INVALID 				{LOCATOR_KIND_INVALID, LOCATOR_PORT_INVALID, LOCATOR_ADDRESS_INVALID}
#define LOCATOR_KIND_INVALID 			-1
//#define LOCATOR_ADDRESS_INVALID		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define LOCATOR_PORT_INVALID 			0
//#define LOCATOR_KIND_RESERVED 		0
//#define LOCATOR_KIND_UDPv4 			1
//#define LOCATOR_KIND_UDPv6 			2

class DLL_STATE Locator : public NetworkLib::NetObjectBase
{
public:
	Locator(long kind, int64 port, const std::string &address)
		: kind_(kind)
		, port_(port)
		, address_(address)
	{}
	Locator()
		: kind_(-1)
		, port_(-1)
	{}
	virtual ~Locator()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt64(kind_);
		writer->WriteInt64(port_);
		writer->WriteString(address_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		kind_ = reader->ReadInt64();
		port_ = reader->ReadInt64();
		address_ = reader->ReadString();
	}

public:
	long			GetKind() 		const { return kind_; }
	int64 	GetPort() 		const { return port_; }
	std::string		GetAddress()	const { return address_; }

	void 			SetKind(long kind)					{ kind_ = kind; }
	void			SetPort(int64 port)			{ port_ = port; }
	void 			SetAddress(const std::string &a)	{ address_ = a; }

public:
	bool operator==(const Locator &other) const
	{
		return (this->kind_ == other.kind_) && (this->port_ == other.port_) && (this->address_ == other.address_);
	}
	
	bool operator<(const Locator &other) const
	{
		return (this->kind_ < other.kind_ || this->port_ < other.port_ || this->address_.length() < other.address_.length());
	}

	bool operator!=(const Locator &other) const
	{
		return !(*this == other);
	}

	unsigned int GetSize() const
	{
		return 4 + 4 + address_.length();
	}

public:
	static Locator INVALID()
	{
		static Locator locator(LOCATOR_KIND_INVALID, LOCATOR_PORT_INVALID, std::string(""));
		return locator;
	}

private:
	long 		kind_;
	int64 		port_;
	std::string	address_;
	//TODO: Replace address string with:
	// ByteSeq				address_;	// default size == 16
};

} // namespace RTPS

#endif // RTPS_Elements_Locator_h_IsIncluded
