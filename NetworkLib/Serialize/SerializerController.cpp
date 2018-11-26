#include"NetworkLib/Serialize/SerializerController.h"
#include"NetworkLib/Serialize/ByteArray.h"
//#include"NetworkLib/Serialize/InByteArray.h"

namespace NetworkLib
{

Singleton<SerializerController> SerializerController::instance_;

SerializerController::SerializerController()
    : context_()
{ }

SerializerController::~SerializerController()
{ }

SerializerController& SerializerController::Instance()
{
    return instance_.GetRef();
}

SerializeWriter* SerializerController::Save(const NetObjectBase &object) const
{
    ByteArray *array = new ByteArray();
    object.Write(array);

    return array;
}

bool SerializerController::Load(SerializeReader *buffer, NetObjectBase *object)
{
    ASSERT(object);
    ASSERT(buffer);

    object->Read(buffer);

    return true;
}

}
