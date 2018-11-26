#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/InstanceHandle.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The SerializerBase class
 *
 * TODO: Use this as base for all serializers?
 */
class SerializerBase
{
public:
    virtual InstanceHandle Handle() const = 0;
};

/**
 * Interface for serializing objects.
 */
template <typename Archive>
class Serializer
{
public:
    CLASS_TRAITS(Serializer)

    virtual void Serialize(Archive & archive) = 0;
};

/**
 * External interface for serializing objects.
 */
template <typename T, typename Archive>
class ExternalSerializer
{
public:
    CLASS_TRAITS(ExternalSerializer)

    virtual void Serialize(const T &t, Archive & archive) = 0;
};

/**
 * Interface for serializing and initializing (deserializing), i.e., read and write.
 */
template <typename Archive>
class SplitSerializer
{
public:
    CLASS_TRAITS(SplitSerializer)

    virtual void Save(Archive & archive) const = 0;

    virtual void Load(Archive & archive) = 0;
};

/**
 * External interface for serializing and initializing (deserializing), i.e., read and write.
 */
template <typename T, typename Archive>
class ExternalSplitSerializer
{
public:
    CLASS_TRAITS(ExternalSplitSerializer)

    virtual void Save(const T &t, Archive & archive) const = 0;

    virtual void Load(T &t, Archive & archive) = 0;
};

/**
 * TODO: Only an example, remove.
 */
namespace example
{
/**
 * @brief The AbstractArchiver class
 */
class AbstractArchiver
{
public:
    template <typename ARG1, typename ARG2, typename ARG3>
    void operator()(ARG1 , ARG2 , ARG3 )
    {
        // serialize objects
    }
};

/**
 * @brief The MyClass class
 */
class MyClass : public Serializer<AbstractArchiver>
{
public:

    virtual void Serialize(AbstractArchiver & archive)
    {
        archive(a_, b_, c_);
    }

//    virtual void Save(AbstractArchiver & archive) const
//    {
//        archive(a_, b_, c_);
//    }

//    virtual void Load(AbstractArchiver & archive)
//    {
//        archive(a_, b_, c_);
//    }

private:
    int a_;
    int b_;
    int c_;
};

//int main()
//{
//    MyClass d;
//    BinaryOutputArchive ar( std::cout );
//    ar( d ); // static assertion failure: cereal detected both a serialize and save/load pair for MyDerived
//}
}

}}
