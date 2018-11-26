/*
 * LocatorList.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_LocatorList_h_IsIncluded
#define RTPS_Elements_LocatorList_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/Locator.h"
#include"RTPS/Export.h"
namespace RTPS
{

typedef std::vector<Locator>	LocatorVector;

class DLL_STATE LocatorList : public NetworkLib::NetObjectBase
{
public:
	LocatorList(const LocatorVector &locator)
		: locatorVector_(locator)
	{}
	LocatorList(const Locator &locator)
	{
		locatorVector_.push_back(locator);
	}
	LocatorList()
	{}
	virtual ~LocatorList()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt64((int64)locatorVector_.size());
		for(LocatorVector::const_iterator it = locatorVector_.begin(), it_end = locatorVector_.end(); it != it_end; ++it)
		{
			const Locator& locator = *it;
			locator.Write(writer);
		}
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		int64 num = (int64)reader->ReadInt64();

		for(int64 i = 0; i < num; i++)
		{
			Locator locator;
			locator.Read(reader);
			locatorVector_.push_back(locator);
		}
	}

public:
	const LocatorVector& GetLocatorVector()             const { return locatorVector_; }

	void SetLocatorVector(const LocatorVector &locator)	    { locatorVector_ = locator; }
	void AddLocator(const Locator &locator)                 { locatorVector_.push_back(locator); }

public:
	unsigned int GetSize() const
	{
		unsigned int size = 0;
		for(LocatorVector::const_iterator it = locatorVector_.begin(), it_end = locatorVector_.end(); it != it_end; ++it)
		{
			const Locator &locator = *it;
			size += locator.GetSize();
		}
		return size;
	}

public:
	static LocatorList INVALID()
	{
		static LocatorList locatorList;

		return locatorList;
	}

private:
	LocatorVector locatorVector_;
};

} // namespace RTPS

#endif // RTPS_Elements_LocatorList_h_IsIncluded
