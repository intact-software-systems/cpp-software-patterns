#pragma once

#include "BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * @brief The Kind enum
 *
 * Inclusive = "[" and "]" or "<=" and ">="
 * Exclusive = "(" and ")" or "<" and ">"
 */
namespace LimitKind {
enum Type
{
    INCLUSIVE,
    EXCLUSIVE
};
}

/**
 * @brief The Kind enum
 *
 * Range connected = "[a,b]"
 * Range disconnected = "[inf, a] and [b, inf]"
 */
enum class DLL_STATE RangeKind
{
    CONNECTED,
    DISCONNECTED
};


enum class DLL_STATE CommDirection : char
{
    Push     = 'P',
    Pull     = 'L',
    PushPull = 'S'
};

}}
