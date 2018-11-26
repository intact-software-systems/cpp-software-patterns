/*
 * Property.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_Property_h_IsIncluded
#define RTPS_Elements_Property_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
	Mapping of the type Property_t

struct Property_t
{
	std::string name;
	std::string value;
};

typedef Property_t Property;
---------------------------------------------------------------------------------------------*/
class DLL_STATE Property: public NetworkLib::NetObjectBase
{
public:
	Property(const std::string &name, const std::string &value)
		: name_(name)
		, value_(value)
	{}
	Property()
		: name_()
		, value_()
	{}
	virtual ~Property()
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteString(name_);
		writer->WriteString(value_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		name_  = reader->ReadString();
		value_ = reader->ReadString();
	}

public:
	bool operator==(const Property &other) const
	{
		return (name_ == other.name_) && (value_ == other.value_);
	}

	bool operator!=(const Property &other) const
	{
		return !((name_ == other.name_) && (value_ == other.value_));
	}

public:
	void SetPropertyName(const std::string &name)  	{ name_ = name; }
	void SetPropertyValue(const std::string &value)	{ value_ = value; }

	std::string GetPropertyName()	const { return name_; }
	std::string GetPropertyValue()	const { return value_; }

private:
	std::string name_;
	std::string value_;
};

} // namespace RTPS

#endif // RTPS_Elements_Property_h_IsIncluded
