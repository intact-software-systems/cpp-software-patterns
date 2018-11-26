#include "BaseLib/GUUID.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/Debug.h"

#ifdef USE_GCC
#include <uuid/uuid.h>
#endif

#ifdef WIN32
#include <objbase.h>
#endif

namespace BaseLib {

GUUID::GUUID(const std::vector<unsigned char>& bytes)
    : std::vector<unsigned char>(bytes)
{ }

GUUID::GUUID(const std::string& id)
    : std::vector<unsigned char>(id.begin(), id.end())
{ }

GUUID::GUUID()
    : std::vector<unsigned char>()
{ }

std::string GUUID::toString() const
{
    std::stringstream str;
    str << *this;

    return str.str();
}

GUUID GUUID::Create()
{
    GuidGenerator g;
    return g.newGuid();
}

#ifdef USE_GCC

GUUID GuidGenerator::newGuid()
{
    uuid_t id;
    uuid_generate(id);

    std::stringstream os;

    os << std::hex << std::setfill('0');

    for(int i = 0; i < 16; i++)
    {
        os << std::setw(2) << (int) id[i];
    }

    return GUUID(os.str());
}

#endif

#ifdef WIN32
GUUID GuidGenerator::newGuid()
{
    GUID guid;
    ::CoCreateGuid(&guid);

    std::stringstream os;

//    os << std::uppercase;
    std::cout.width(8);
    os << std::hex << guid.Data1;

    std::cout.width(4);
    os << std::hex << guid.Data2;

    std::cout.width(4);
    os << std::hex << guid.Data3;

    std::cout.width(2);
    os << std::hex
        << static_cast<short>(guid.Data4[0])
        << static_cast<short>(guid.Data4[1])
        << static_cast<short>(guid.Data4[2])
        << static_cast<short>(guid.Data4[3])
        << static_cast<short>(guid.Data4[4])
        << static_cast<short>(guid.Data4[5])
        << static_cast<short>(guid.Data4[6])
        << static_cast<short>(guid.Data4[7]);
//    os << std::nouppercase;

    IINFO() << "UUID: " << os.str();
    return GUUID(os.str());
}

#endif

}
