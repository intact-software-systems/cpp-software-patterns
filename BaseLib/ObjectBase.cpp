#include "BaseLib/ObjectBase.h"
#include "BaseLib/ObjectManager.h"
#include "BaseLib/MutexLocker.h"
#include <sstream>

using namespace std;

namespace BaseLib
{

ObjectBase::ObjectBase(std::string objectName) : name_(objectName)
{
    if(name_.empty() == true)
    {
        static int count = 0;
        stringstream ostr;
        ostr << "BaseLib.ObjectBase." << count++;
        name_ = ostr.str();
    }

    ObjectManager::getObjectManager()->AddObject(this);
}

ObjectBase::~ObjectBase()
{
    ObjectManager::getObjectManager()->RemoveObject(this);
}

std::string ObjectBase::Name() const throw()
{
    MutexLocker lock(&mutex_);
    return name_;
}

void ObjectBase::SetName(std::string name) throw()
{
    MutexLocker lock(&mutex_);
    name_ = name;
}

//shared_ptr<ObjectBase> ObjectBase::create()
//{
//	shared_ptr<ObjectBase> px(new ObjectBase());
    // create weak pointers from px here
//	return px;
//}

} // namespace BaseLib

