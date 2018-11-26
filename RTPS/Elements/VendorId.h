/*
 * VendorId.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_VendorId_h_IsIncluded
#define RTPS_Elements_VendorId_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type VendorId_t

struct VendorId_t
{
	octet[2] vendorId;
};

typedef VendorId_t VendorId;

//Mapping of the reserved values:
#define VENDORID_UNKNOWN {0,0}
---------------------------------------------------------------------------------------------*/
class DLL_STATE VendorId : public NetworkLib::NetObjectBase
{
public:
	VendorId(const std::vector<octet> &id)
		: vendorId_(id)
	{
		if(vendorId_.size() != VendorId::SIZE())
			throw CriticalException("VendorId vector<octet> has wrong size! Must be made of 2 octets/chars!");

		ASSERT(vendorId_.size() == VendorId::SIZE());
	}
	VendorId()
		: vendorId_(VendorId::SIZE(), 0x00)
	{}
	virtual ~VendorId()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		if(vendorId_.empty()) return;

		// -- debug --
		ASSERT(vendorId_.size() == VendorId::SIZE());
		// -- debug --

		for(unsigned int i = 0; i < VendorId::SIZE(); i++)
			writer->WriteInt8(vendorId_[i]);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		for(unsigned int i = 0; i < VendorId::SIZE(); i++)
			vendorId_[i] = reader->ReadInt8();

		// -- debug --
		ASSERT(vendorId_.size() == VendorId::SIZE());
		// -- debug --
	}

public:
	static VendorId UNKNOWN()
	{
		static VendorId id;
		return id;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 2;
		return sz;
	}

public:
	bool operator==(const VendorId &other) const
	{
		return (vendorId_ == other.vendorId_);
	}

	bool operator!=(const VendorId &other) const
	{
		return !(vendorId_ == other.vendorId_);
	}

public:
	std::vector<octet> 	GetVendorId()			const { return vendorId_; }

	void SetVendorId(const std::vector<octet> &id)
	{
		// -- debug --
		ASSERT(id.size() == VendorId::SIZE());
		// -- debug --

		vendorId_ = id;
	}

private:
	std::vector<octet> vendorId_;
};

} // namespace RTPS

#endif // RTPS_Elements_VendorId_h_IsIncluded
