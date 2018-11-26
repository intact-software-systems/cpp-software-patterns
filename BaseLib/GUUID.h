#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>

#include "BaseLib/Export.h"

namespace BaseLib {

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
class DLL_STATE GUUID : public std::vector<unsigned char>
{
public:
    GUUID(const std::vector<unsigned char>& bytes);
    GUUID(const std::string& fromString);

    GUUID();

    std::string toString() const;

    static GUUID Create();

    friend std::ostream& operator<<(std::ostream& s, const GUUID& guid)
    {
        if(!guid.empty())
        {
            for(const unsigned char& entry : guid)
            {
                s << entry;
            }
        }
        return s;
    }
};

// Class that can create new guids. The only reason this exists instead of
// just a global "newGuid" function is because some platforms will require
// that there is some attached context. In the case of android, we need to
// know what JNIEnv is being used to call back to Java, but the newGuid()
// function would no longer be cross-platform if we parameterized the android
// version. Instead, construction of the GuidGenerator may be different on
// each platform, but the use of newGuid is uniform.
struct DLL_STATE GuidGenerator
{
    GUUID newGuid();
};

}

