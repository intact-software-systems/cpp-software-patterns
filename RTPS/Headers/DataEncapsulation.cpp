#include"RTPS/Headers/DataEncapsulation.h"

namespace RTPS
{
DataEncapsulation::DataEncapsulation(char endian)
	: endian_(endian)
{

}
DataEncapsulation::~DataEncapsulation()
{

}

void DataEncapsulation::Write(NetworkLib::SerializeWriter *writer) const
{
	writer->WriteInt8(endian_);
}

void DataEncapsulation::Read(NetworkLib::SerializeReader *reader)
{
	endian_ = reader->ReadInt8();
}

} // namespace RTPS

