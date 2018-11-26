#pragma once

#include<algorithm>
#include<iostream>
#include<stdlib.h>

#include"NetworkLib/Export.h"

namespace NetworkLib {

/**
* @brief a class containing static methods for endian conversion.
*/
struct DLL_STATE EndianConverter
{
    enum Endian
    {
        ENDIAN_BIG,
        ENDIAN_LITTLE
    };

    static void ByteSwap(unsigned char* b, int n)
    {
        int i = 0;
        int j = n - 1;
        while(i < j)
        {
            std::swap(b[i], b[j]);
            i++, j--;
        }
    }
};
}

