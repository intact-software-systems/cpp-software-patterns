/*
 * TypeName.h
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */
#ifndef DDS_Elements_TypeName_h_IsIncluded
#define DDS_Elements_TypeName_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/Export.h"

namespace DDS { namespace Elements
{

/**
 * @brief The TypeName class is an identifier of the data associated with a Topic.
 */
class DLL_STATE TypeName
        : public BaseLib::Templates::ComparableImmutableType<std::string>
//                         , public NetworkLib::NetObjectBase
{
public:
    /**
     * @brief TypeName
     * @param name
     */
    TypeName(std::string name = std::string(""));

    /**
     * @brief TypeName generates typename using Utility::GetTypeName<T>()
     */
//    template <typename T>
//    TypeName();

    /**
     * @brief ~TypeName
     */
    virtual ~TypeName();

//public:
//    virtual void Write(SerializeWriter *writer) const
//    {
//        writer->WriteString(this->delegate());
//    }

//    virtual void Read(SerializeReader *reader)
//    {
//        this->Set(reader->ReadString());
//    }
};

/**
 * @brief TypeName generates typename using Utility::GetTypeName<T>()
 */
//template <typename T>
//TypeName::TypeName()
//    : BaseLib::Templates::ComparableMutableType<std::string>(Utility::GetTypeName<T>())
//{ }

}}

#endif
