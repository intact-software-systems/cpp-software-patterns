#pragma once

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Serialize/SerializeBase.h"

#include"NetworkLib/Export.h"

namespace NetworkLib {

struct DLL_STATE SocketWriterFunctions
{
    /* -------------------------------------------------------------------------
            Map functions
    ---------------------------------------------------------------------------*/
    typedef std::map<std::string, short>       MapStringInt16;
    typedef std::map<std::string, int>         MapStringInt32;
    typedef std::map<std::string, long long>   MapStringInt64;
    typedef std::map<std::string, double>      MapStringDouble;
    typedef std::map<std::string, std::string> MapStringString;

    static void WriteMapStringInt16(SerializeWriter* writer, const MapStringInt16& mapStringInt);

    static void WriteMapStringInt32(SerializeWriter* writer, const MapStringInt32& mapStringInt);

    static void WriteMapStringInt64(SerializeWriter* writer, const MapStringInt64& mapStringInt);

    static void WriteMapStringDouble(SerializeWriter* writer, const MapStringDouble& mapStringDouble);

    static void WriteMapStringString(SerializeWriter* writer, const MapStringString& mapStringString);

    /* -------------------------------------------------------------------------
            Set functions
    ---------------------------------------------------------------------------*/
    typedef std::set<char>        SetInt8;
    typedef std::set<short>       SetInt16;
    typedef std::set<int>         SetInt32;
    typedef std::set<long long>   SetInt64;
    typedef std::set<std::string> SetString;

    static void WriteSetInt8(SerializeWriter* writer, const SetInt8& setVariables);

    static void WriteSetInt16(SerializeWriter* writer, const SetInt16& setVariables);

    static void WriteSetInt32(SerializeWriter* writer, const SetInt32& setVariables);

    static void WriteSetInt64(SerializeWriter* writer, const SetInt64& setVariables);

    static void WriteSetString(SerializeWriter* writer, const SetString& setString);

    /* -------------------------------------------------------------------------
            Vector functions
    ---------------------------------------------------------------------------*/
    typedef std::vector<uint8_t>     VectorUInt8;
    typedef std::vector<char>        VectorInt8;
    typedef std::vector<short>       VectorInt16;
    typedef std::vector<int>         VectorInt32;
    typedef std::vector<long long>   VectorInt64;
    typedef std::vector<std::string> VectorString;

    static void WriteVectorUInt8(SerializeWriter* writer, const VectorUInt8& vectorVariables);

    static void WriteVectorInt8(SerializeWriter* writer, const VectorInt8& vectorVariables);

    static void WriteVectorInt16(SerializeWriter* writer, const VectorInt16& vectorVariables);

    static void WriteVectorInt8(SerializeWriter* writer, const VectorInt32& vectorVariables);

    static void WriteVectorInt8(SerializeWriter* writer, const VectorInt64& vectorVariables);

    static void WriteVectorString(SerializeWriter* writer, const VectorString& vectorVariables);
};

}

