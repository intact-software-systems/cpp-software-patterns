#ifndef BaseLib_Multiton_h_IsIncluded
#define BaseLib_Multiton_h_IsIncluded

#include <map>

namespace BaseLib
{

/**
 @description Implementation of Multiton software pattern.

    class Foo : public Multiton<std::string, Foo> {};
    Foo& foo1 = Foo::getRef("foobar");
    Foo* foo2 = Foo::getPtr("foobar");
    Foo::destroy();

    class Foo : public Multiton<Foo> {};
    class Bar : public Multiton<Bar,int> {};

 @url http://stackoverflow.com/questions/2346091/c-templated-class-implementation-of-the-multiton-pattern
*/
template <typename Key, typename T>
class Multiton
{
public:
    static void Destroy()
    {
        for (typename std::map<Key, T*>::iterator it = instances.begin(); it != instances.end(); ++it) {
            delete (*it).second;
        }
    }

    static T& GetRef(const Key& key)
    {
        typename std::map<Key, T*>::iterator it = instances.find(key);

        if(it != instances.end())
        {
            return *(T*)(it->second);
        }

        T* instance = new T;
        instances[key] = instance;

        return *instance;
    }

    static T* GetPtr(const Key& key)
    {
        typename std::map<Key, T*>::iterator it = instances.find(key);

        if(it != instances.end())
        {
            return (T*)(it->second);
        }

        T* instance = new T;
        instances[key] = instance;

        return instance;
    }

protected:
    Multiton() {}
    virtual ~Multiton() {}

private:
    Multiton(const Multiton&) {}
    Multiton& operator = (const Multiton&) { return *this; }

    static std::map<Key, T*> instances;
};

template <typename Key, typename T> std::map<Key, T*> Multiton<Key, T>::instances;

} // namespace BaseLib

#endif // BaseLib_Multiton_h_IsIncluded
