#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Serialize/NetObjectBase.h"

#include"NetworkLib/Export.h"

namespace NetworkLib {
class SerializerControllerConfig
{
public:
    SerializerControllerConfig()
    {}

    ~SerializerControllerConfig()
    {}
};

class SerializerControllerState
{
//private:
//    typedef RxData::ObjectCacheProxy<InstanceHandle, Publication::PublisherPtr>      PublisherAccess;
public:
    SerializerControllerState()
    {}

    ~SerializerControllerState()
    {}
};

/**
 * @brief The SerializerController class
 *
 * For the parameterized serializers, perhaps use a factory pattern, where a serializer is created
 * on the fly, but from where?
 *
 * SerializerCache -> (key, value) -> (serializerId, implementation)
 */
class DLL_STATE SerializerController : public BaseLib::Templates::NoCopy<SerializerController>
{
private:
    friend class Singleton<SerializerController>;

    SerializerController();
    virtual ~SerializerController();

public:
    static SerializerController& Instance();

//public:
//    template <typename T, typename Archive>
//    SerializeWriter::Ptr Save()
//    {
//        //ExternalSplitSerializer<T, Archive> serializer;
//        return SerializeWriter::Ptr();
//    }

//    template <typename T, typename Archive>
//    SerializeReader::Ptr Read()
//    {
//        return SerializeReader::Ptr();
//    }

    SerializeWriter* Save(const NetObjectBase& object) const;

    bool Load(SerializeReader* buffer, NetObjectBase* object);

private:
    static BaseLib::Singleton<SerializerController> instance_;

    BaseLib::Templates::ContextObject<SerializerControllerConfig, SerializerControllerState, BaseLib::Templates::NullCriticalState> context_;
};

}
