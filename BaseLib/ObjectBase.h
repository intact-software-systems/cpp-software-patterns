#ifndef BaseLib_ObjectBase_h_Included
#define BaseLib_ObjectBase_h_Included

#include<exception>
#include<iostream>
#include<string>

#include"BaseLib/Mutex.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

// TODO: Add function that uniquely id the object
class DLL_STATE ObjectBase
{
public:
    ObjectBase(std::string objectName = std::string(""));
    virtual ~ObjectBase();

    std::string Name() const throw();
    void SetName(std::string name) throw();

    //shared_ptr<X> create();
    //static virtual shared_ptr<ObjectBase> create();

private:
    std::string 	name_;
    mutable Mutex	mutex_;
};


/*template <class T>
class IObject {
public:
    IObject(T obj) : obj_(obj)
    {}
private:
    T obj_;
}
*/

} // namespace ObjectBase

#endif // BaseLib_ObjectBase_h_Included


