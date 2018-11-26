/*
 * TopicName.h
 *
 *  Created on: Jul 1, 2012
 *      Author: knuthelv
 */
#ifndef DDS_Elements_TopicName_h_IsIncluded
#define DDS_Elements_TopicName_h_IsIncluded

#include"DDS/IncludeExtLibs.h"
#include"DDS/Export.h"

namespace DDS { namespace Elements
{
/**
 * @brief The TopicName class is an identifier of the Topic.
 */
class DLL_STATE TopicName
        : public BaseLib::Templates::ComparableImmutableType<std::string>
//                          , public NetworkLib::NetObjectBase
{
public:
    /**
     * @brief TopicName
     * @param name
     */
    TopicName(std::string name = std::string(""));

    /**
     * @brief TopicName generates topicname using Utility::GetTypeName<T>()
     */
//    template <typename T>
//    TopicName();

    /**
     * @brief ~TopicName
     */
    virtual ~TopicName();

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
 * @brief TopicName generates topicname using Utility::GetTypeName<T>()
 */
//template <typename T>
//TopicName::TopicName()
//    : BaseLib::Templates::ComparableMutableType<std::string>(Utility::GetTypeName<T>())
//{ }

}}

#endif

