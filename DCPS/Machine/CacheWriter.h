#ifndef DCPS_Machine_CacheWriter_h_IsIncluded
#define DCPS_Machine_CacheWriter_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Export.h"

namespace DCPS
{

/**
 * TODO: Move to RxData or NetworkLib and implement for each serializing strategy: RTPS, JSON, XML, etc.
 */
template <typename KEY, typename DATA>
class CacheWriter
{
public:
    CLASS_TRAITS(CacheWriter)

    virtual bool Write(KEY key, const DATA &data, const InstanceHandle &handle) = 0;

    virtual bool Dispose(const InstanceHandle &handle) = 0;
    virtual bool Unregister(const InstanceHandle &handle) = 0;

    virtual bool Dispose(KEY key) = 0;
    virtual bool Unregister(KEY key) = 0;
};

/**
 * @brief The CacheWriter class
 *
 * TODO: can be a wait function with a maximum timout
 *
 * - Write to the RTPS cache_
 * - this cache_ writes to to the "RTPS" cache_->Write(byteBuffer) using the RxData,
 * - then the VirtualMachine is listening for changes to any caches and writing to
 *
 * - TODO: Instead of writing to DCPS::Publication::CacheWriter, write to RxData::Cache::ObjectWriter and let
 *   either RTPS or SocketReactor be listeners to the cache
 *
 *  - ObjectWriter<SerializeWriter::Ptr, handle>
 *
 *  - A DCPS configurator constructs this writer using the appropriate cache writer
 */
class DCPSCacheWriter
{
public:
    DCPSCacheWriter()
        : cache_()
    {}

    DCPSCacheWriter(const DCPS::Publication::CacheWriter &cache)
        : cache_(cache)
    {}

    virtual ~DCPSCacheWriter()
    {}

    template <typename DATA> //, typename ARCHIVE>
    bool Write(const DATA &data, const InstanceHandle &handle)
    {
        // ------------------------------------------------------------------------------------
        // Find serializer/archiver - configured locally and controlled centrally through a singleton
        // ------------------------------------------------------------------------------------
        //ARCHIVE archiver = SerializerController::GetInstance().GetSerializer<ARCHIVE>();

        // ------------------------------------------------------------------------------------
        // By using an external serializer I can support many different kinds of serializations
        // without tightly coupling them to the specific type.
        // ------------------------------------------------------------------------------------
        //ExternalSplitSerializer<DATA, ARCHIVE> serializer = SerializerController::GetInstance().GetSerializer<DATA, ARCHIVE>();
        //serializer.Save(data, archiver);

        SerializeWriter::Ptr buffer(SerializerController::Instance().Save(data));

        // ------------------------------------------------------------------------------------
        // Write to cache, for example, RTPS or directly to SocketReactor
        // ------------------------------------------------------------------------------------
        bool isWritten = cache_.Write(buffer, handle, BaseLib::Timestamp());

        return isWritten;
    }

//    template <typename DATA>
//    bool Write(const DATA &data, const InstanceHandle &handle)
//    {
//        // Use default serializer

//        return false;
//    }

    bool Dispose(const InstanceHandle &handle)
    {
        return cache_.Dispose(handle);
    }

    bool Unregister(const InstanceHandle &handle)
    {
        return cache_.Unregister(handle);
    }

    template <typename KEY>
    bool Dispose(KEY key)
    {
        return false;
    }

    template <typename KEY>
    bool Unregister(KEY key)
    {
        return false;
    }

private:
    /**
     * @brief cache_ is the access to the serializing implementation, for example, RTPS
     */
    DCPS::Publication::CacheWriter cache_;
};

}

#endif
