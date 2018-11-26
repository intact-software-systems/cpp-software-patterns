/*
 * GuidPrefix.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_GuidPrefix_h_IsIncluded
#define RTPS_Elements_GuidPrefix_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type GuidPrefix_t
	The PSM maps the GuidPrefix_t to the following structure:

	typedef octet[12] GuidPrefix_t;
	typedef GuidPrefix_t GuidPrefix;

The reserved constant GUIDPREFIX_UNKNOWN defined by the PIM is mapped to:

#define GUIDPREFIX_UNKNOWN {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
---------------------------------------------------------------------------------------------*/

/**
 * @brief The GuidPrefix uniquely identifies a Participant within the Domain.
 */
class DLL_STATE GuidPrefix : public NetworkLib::NetObjectBase
{
public:
	GuidPrefix(const std::vector<char> &guidPrefix)
		: guidPrefix_(guidPrefix)
	{
		if(guidPrefix_.size() != GuidPrefix::SIZE())
			throw CriticalException("GuidPrefix vector<octet> has wrong size! Must be made of 12 octets/chars!");
	}
	GuidPrefix()
		: guidPrefix_(GuidPrefix::SIZE(), 0x00)
	{ }
	virtual ~GuidPrefix()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		for(size_t i = 0; i < GuidPrefix::SIZE(); i++)
			writer->WriteInt8(guidPrefix_[i]);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		for(size_t i = 0; i < GuidPrefix::SIZE(); i++)
			guidPrefix_[i] = reader->ReadInt8();
	}

public:
	static GuidPrefix UNKNOWN()
	{
		static GuidPrefix guid;
		return guid;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 12;
		return sz;
	}

public:
	inline void SetGuidPrefix(const std::vector<char> &guidPrefix) { guidPrefix_ = guidPrefix; }

	inline const std::vector<char>& GetGuidPrefix() const { return guidPrefix_; }

public:
	bool operator==(const GuidPrefix &other) const
	{
		return this->guidPrefix_ == other.guidPrefix_;
	}

	bool operator!=(const GuidPrefix &other) const
	{
		return !(this->guidPrefix_ == other.guidPrefix_);
	}

	bool operator<(const GuidPrefix &other) const
	{
		return this->guidPrefix_ < other.guidPrefix_;
	}

private:
	std::vector<char> guidPrefix_;
};

} // namespace RTPS

#endif // RTPS_Elements_GuidPrefix_h_IsIncluded
