#ifndef INTEGRATIONTYPE_H
#define INTEGRATIONTYPE_H

#include "BaseLib/CommonDefines.h"

namespace BaseLib { namespace Templates
{

/**
 * Add template types to support intergration between layers, for example, DCPS to RTPS to SocketReactor.
 *
 * Integration is typically data flowing up or down. Consider using observer pattern.
 */
template <typename T>
class DataReader
{
public:
    virtual std::vector<T> Read() = 0;
    virtual std::vector<T> Take() = 0;
};


template <typename K, typename V>
class DataWriter
{
public:
    virtual bool Write(const V &t) = 0;
    virtual bool Write(const V &t, const K &key) = 0;

    virtual bool Dispose(const K &key) = 0;
    virtual bool Unregister(const K &key) = 0;
};

}}

#endif
