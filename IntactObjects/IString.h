#ifndef IntactObjects_ObjectSerializer_h_IsIncluded
#define IntactObjects_ObjectSerializer_h_IsIncluded

#include <string>

#include"IntactObjects/IncludeExtLibs.h"
#include"IntactObjects/Export.h"
namespace IntactObjects
{

class DLL_STATE IString : public MicroMiddleware::RmiObjectBase
{
public:
	IString(std::string str) : istring_(str)
	{}
	IString(const std::string &str) : istring_(str)
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteString(istring_);
	}
	
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		istring_ = reader->ReadString();
	}
	
	std::string GetString() 				const 	{ return istring_; }
	void SetString(const std::string &str) 			{ istring_ = str; }

private:
	std::string istring_;
};

} // namespace IntactObjects

#endif //

