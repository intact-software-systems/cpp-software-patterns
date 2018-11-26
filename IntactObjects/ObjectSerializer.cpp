#include "IntactObjects/ObjectSerializer.h"

namespace IntactObjects
{

// TODO: Return bytesWritten for each function!

void ObjectSerializer::WriteMapStringGroupInformation(NetworkLib::SerializeWriter *writer, MapStringGroupInformation &mapStringInformation)
{
	writer->WriteInt32((int)mapStringInformation.size());
	for(MapStringGroupInformation::iterator it = mapStringInformation.begin(), it_end = mapStringInformation.end(); it != it_end; ++it)
	{
		writer->WriteString(it->first);
		GroupInformation &groupInfo = it->second;
		groupInfo.Write(writer);
	}
}

void ObjectSerializer::WriteMapStringHostInformation(NetworkLib::SerializeWriter *writer, MapStringHostInformation &mapStringInformation)
{
	writer->WriteInt32((int)mapStringInformation.size());
	for(MapStringHostInformation::iterator it = mapStringInformation.begin(), it_end = mapStringInformation.end(); it != it_end; ++it)
	{
		writer->WriteString(it->first);
		HostInformation &hostInfo = it->second;
		hostInfo.Write(writer);
	}
}

void ObjectSerializer::WriteSetGroupHandle(NetworkLib::SerializeWriter *writer, SetGroupHandle &setGroupHandle)
{
	writer->WriteInt32((int)setGroupHandle.size());
	for(SetGroupHandle::iterator it = setGroupHandle.begin(), it_end = setGroupHandle.end(); it != it_end; ++it)
	{
		GroupHandle handle = *it;
		handle.Write(writer);
	}
}

void ObjectSerializer::ReadMapStringGroupInformation(NetworkLib::SerializeReader *reader, MapStringGroupInformation &mapStringInformation)
{
	int size = reader->ReadInt32();
	for(int i = 0; i < size; i++)
	{
		string str = reader->ReadString();
		GroupInformation info;
		info.Read(reader);
		
		mapStringInformation.insert(pair<string, GroupInformation>(str, info));
	}
}

void ObjectSerializer::ReadMapStringHostInformation(NetworkLib::SerializeReader *reader, MapStringHostInformation &mapStringInformation)
{
	int size = reader->ReadInt32();
	for(int i = 0; i < size; i++)
	{
		string str = reader->ReadString();
		HostInformation info;
		info.Read(reader);
		
		mapStringInformation.insert(pair<string, HostInformation>(str, info));
	}
}

void ObjectSerializer::ReadSetGroupHandle(NetworkLib::SerializeReader *reader, SetGroupHandle &setGroupHandle)
{
	int size = reader->ReadInt32();
	for(int i = 0; i < size; i++)
	{
		GroupHandle handle;
		handle.Read(reader);
		setGroupHandle.insert(handle);
	}
}

}

