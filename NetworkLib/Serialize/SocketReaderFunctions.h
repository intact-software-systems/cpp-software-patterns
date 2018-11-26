#pragma once

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

struct DLL_STATE SocketReaderFunctions
{
    /* -------------------------------------------------------------------------
            Map functions
    ---------------------------------------------------------------------------*/
    typedef std::map<std::string, short>       MapStringInt16;
    typedef std::map<std::string, int>         MapStringInt32;
    typedef std::map<std::string, long long>   MapStringInt64;
    typedef std::map<std::string, double>      MapStringDouble;
    typedef std::map<std::string, std::string> MapStringString;

    static void ReadMapStringInt16(SerializeReader* reader, MapStringInt16& mapStringInt);
    static void ReadMapStringInt32(SerializeReader* reader, MapStringInt32& mapStringInt);
    static void ReadMapStringInt64(SerializeReader* reader, MapStringInt64& mapStringInt);
    static void ReadMapStringDouble(SerializeReader* reader, MapStringDouble& mapStringDouble);
    static void ReadMapStringString(SerializeReader* reader, MapStringString& mapStringString);

    /* -------------------------------------------------------------------------
            Set functions
    ---------------------------------------------------------------------------*/

    typedef std::set<char>        SetInt8;
    typedef std::set<short>       SetInt16;
    typedef std::set<int>         SetInt32;
    typedef std::set<long long>   SetInt64;
    typedef std::set<std::string> SetString;

    static void ReadSetInt8(SerializeReader* reader, SetInt8& setVars);
    static void ReadSetInt16(SerializeReader* reader, SetInt16& setVars);
    static void ReadSetInt32(SerializeReader* reader, SetInt32& setVars);
    static void ReadSetInt64(SerializeReader* reader, SetInt64& setVars);
    static void ReadSetString(SerializeReader* reader, SetString& setString);

    /* -------------------------------------------------------------------------
            Vector functions
    ---------------------------------------------------------------------------*/

    typedef std::vector<uint8_t>     VectorUInt8;
    typedef std::vector<char>        VectorInt8;
    typedef std::vector<short>       VectorInt16;
    typedef std::vector<int>         VectorInt32;
    typedef std::vector<long long>   VectorInt64;
    typedef std::vector<std::string> VectorString;

    static void ReadVectorUInt8(SerializeReader* reader, VectorUInt8& vectorArray);
    static void ReadVectorInt8(SerializeReader* reader, VectorInt8& vectorArray);
    static void ReadVectorInt16(SerializeReader* reader, VectorInt16& vectorArray);
    static void ReadVectorInt32(SerializeReader* reader, VectorInt32& vectorArray);
    static void ReadVectorInt64(SerializeReader* reader, VectorInt64& vectorArray);
    static void ReadVectorString(SerializeReader* reader, VectorString& vectorArray);
};

}

