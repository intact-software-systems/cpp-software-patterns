#include"NetworkLib/Serialize/SocketReaderFunctions.h"

namespace NetworkLib
{

void SocketReaderFunctions::ReadMapStringInt16(SerializeReader *reader, MapStringInt16 &mapStringInt)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
    {
        std::string str = reader->ReadString();
        short val = reader->ReadInt16();

        mapStringInt.insert(std::pair<std::string, int>(str, val));
    }
}

void SocketReaderFunctions::ReadMapStringInt32(SerializeReader *reader, MapStringInt32 &mapStringInt)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
    {
        std::string str = reader->ReadString();
        int val = reader->ReadInt32();

        mapStringInt.insert(std::pair<std::string, int>(str, val));
    }
}

void SocketReaderFunctions::ReadMapStringInt64(SerializeReader *reader, MapStringInt64 &mapStringInt)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
    {
        std::string str = reader->ReadString();
        long long val = reader->ReadInt64();
        mapStringInt.insert(std::pair<std::string, long long>(str, val));
    }
}

void SocketReaderFunctions::ReadMapStringDouble(SerializeReader *reader, MapStringDouble &mapStringDouble)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
    {
        std::string str = reader->ReadString();
        double val = reader->ReadDouble();
        mapStringDouble.insert(std::pair<std::string, double>(str, val));
    }
}

void SocketReaderFunctions::ReadMapStringString(SerializeReader *reader, MapStringString &mapStringString)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
    {
        std::string str = reader->ReadString();
        std::string val = reader->ReadString();
        mapStringString.insert(std::pair<std::string, std::string>(str, val));
    }
}

void SocketReaderFunctions::ReadSetInt8(SerializeReader *reader, SetInt8 &setVars)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        setVars.insert(reader->ReadInt8());
}

void SocketReaderFunctions::ReadSetInt16(SerializeReader *reader, SetInt16 &setVars)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        setVars.insert(reader->ReadInt16());
}

void SocketReaderFunctions::ReadSetInt32(SerializeReader *reader, SetInt32 &setVars)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        setVars.insert(reader->ReadInt32());
}

void SocketReaderFunctions::ReadSetInt64(SerializeReader *reader, SetInt64 &setVars)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        setVars.insert(reader->ReadInt64());
}

void SocketReaderFunctions::ReadSetString(SerializeReader *reader, SetString &setString)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        setString.insert(reader->ReadString());
}

void SocketReaderFunctions::ReadVectorUInt8(SerializeReader *reader, VectorUInt8 &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadInt8());
}

void SocketReaderFunctions::ReadVectorInt8(SerializeReader *reader, VectorInt8 &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadInt8());
}

void SocketReaderFunctions::ReadVectorInt16(SerializeReader *reader, VectorInt16 &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadInt16());
}

void SocketReaderFunctions::ReadVectorInt32(SerializeReader *reader, VectorInt32 &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadInt32());
}

void SocketReaderFunctions::ReadVectorInt64(SerializeReader *reader, VectorInt64 &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadInt64());
}

void SocketReaderFunctions::ReadVectorString(SerializeReader *reader, VectorString &vectorArray)
{
    int size = reader->ReadInt32();
    for(int i = 0; i < size; i++)
        vectorArray.push_back(reader->ReadString());
}

}
