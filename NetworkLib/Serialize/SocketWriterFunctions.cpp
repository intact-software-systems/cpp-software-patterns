#include"NetworkLib/Serialize/SocketWriterFunctions.h"

namespace NetworkLib
{

void SocketWriterFunctions::WriteMapStringInt16(SerializeWriter *writer, const MapStringInt16 &mapStringInt)
{
    writer->WriteInt32(mapStringInt.size());
    for(MapStringInt16::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
    {
        writer->WriteString(it->first);
        writer->WriteInt16(it->second);
    }
}

void SocketWriterFunctions::WriteMapStringInt32(SerializeWriter *writer, const MapStringInt32 &mapStringInt)
{
    writer->WriteInt32(mapStringInt.size());
    for(MapStringInt32::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
    {
        writer->WriteString(it->first);
        writer->WriteInt32(it->second);
    }
}

void SocketWriterFunctions::WriteMapStringInt64(SerializeWriter *writer, const MapStringInt64 &mapStringInt)
{
    writer->WriteInt32(mapStringInt.size());
    for(MapStringInt64::const_iterator it = mapStringInt.begin(), it_end = mapStringInt.end(); it != it_end; ++it)
    {
        writer->WriteString(it->first);
        writer->WriteInt64(it->second);
    }
}

void SocketWriterFunctions::WriteMapStringDouble(SerializeWriter *writer, const MapStringDouble &mapStringDouble)
{
    writer->WriteInt32(mapStringDouble.size());
    for(MapStringDouble::const_iterator it = mapStringDouble.begin(), it_end = mapStringDouble.end(); it != it_end; ++it)
    {
        writer->WriteString(it->first);
        writer->WriteDouble(it->second);
    }
}

void SocketWriterFunctions::WriteMapStringString(SerializeWriter *writer, const MapStringString &mapStringString)
{
    writer->WriteInt32(mapStringString.size());
    for(MapStringString::const_iterator it = mapStringString.begin(), it_end = mapStringString.end(); it != it_end; ++it)
    {
        writer->WriteString(it->first);
        writer->WriteString(it->second);
    }
}


void SocketWriterFunctions::WriteSetInt8(SerializeWriter *writer, const SetInt8 &setVariables)
{
    writer->WriteInt32(setVariables.size());
    for(SetInt8::const_iterator it = setVariables.begin(), it_end = setVariables.end(); it != it_end; ++it)
        writer->WriteInt8(*it);
}

void SocketWriterFunctions::WriteSetInt16(SerializeWriter *writer, const SetInt16 &setVariables)
{
    writer->WriteInt32(setVariables.size());
    for(SetInt16::const_iterator it = setVariables.begin(), it_end = setVariables.end(); it != it_end; ++it)
        writer->WriteInt16(*it);
}

void SocketWriterFunctions::WriteSetInt32(SerializeWriter *writer, const SetInt32 &setVariables)
{
    writer->WriteInt32(setVariables.size());
    for(SetInt32::const_iterator it = setVariables.begin(), it_end = setVariables.end(); it != it_end; ++it)
        writer->WriteInt32(*it);
}

void SocketWriterFunctions::WriteSetInt64(SerializeWriter *writer, const SetInt64 &setVariables)
{
    writer->WriteInt32(setVariables.size());
    for(SetInt64::const_iterator it = setVariables.begin(), it_end = setVariables.end(); it != it_end; ++it)
        writer->WriteInt64(*it);
}

void SocketWriterFunctions::WriteSetString(SerializeWriter *writer, const SetString &setString)
{
    writer->WriteInt32(setString.size());
    for(SetString::const_iterator it = setString.begin(), it_end = setString.end(); it != it_end; ++it)
        writer->WriteString(*it);
}



void SocketWriterFunctions::WriteVectorUInt8(SerializeWriter *writer, const VectorUInt8 &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorUInt8::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteInt8(*it);
}

void SocketWriterFunctions::WriteVectorInt8(SerializeWriter *writer, const VectorInt8 &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorInt8::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteInt8(*it);
}

void SocketWriterFunctions::WriteVectorInt16(SerializeWriter *writer, const VectorInt16 &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorInt16::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteInt16(*it);
}

void SocketWriterFunctions::WriteVectorInt8(SerializeWriter *writer, const VectorInt32 &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorInt32::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteInt32(*it);
}

void SocketWriterFunctions::WriteVectorInt8(SerializeWriter *writer, const VectorInt64 &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorInt64::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteInt64(*it);
}

void SocketWriterFunctions::WriteVectorString(SerializeWriter *writer, const VectorString &vectorVariables)
{
    writer->WriteInt32(vectorVariables.size());
    for(VectorString::const_iterator it = vectorVariables.begin(), it_end = vectorVariables.end(); it != it_end; ++it)
        writer->WriteString(*it);
}

}

