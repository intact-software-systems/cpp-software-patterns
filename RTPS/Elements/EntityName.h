/*
 * EntityName.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_EntityName_h_IsIncluded
#define RTPS_Elements_EntityName_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type EntityName_t

struct EntityName_t
{
	std::string name;
};

typedef EntityName_t EntityName;
---------------------------------------------------------------------------------------------*/
class DLL_STATE EntityName: public NetworkLib::NetObjectBase
{
public:
	EntityName(const std::string &name)
		: name_(name)
	{}
	EntityName()
		: name_()
	{}
	virtual ~EntityName()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteString(name_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		name_  = reader->ReadString();
	}
public:
	bool operator==(const EntityName &other) const
	{
		return (name_ == other.name_);
	}

	bool operator!=(const EntityName &other) const
	{
		return !(name_ == other.name_);
	}

public:
	void SetEntityName(const std::string &name) 	{ name_ = name; }

	std::string GetEntityName()	const { return name_; }

private:
	std::string name_;
};

} // namespace RTPS

#endif // RTPS_Elements_EntityName_h_IsIncluded
