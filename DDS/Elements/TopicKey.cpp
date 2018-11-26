#include "DDS/Elements/TopicKey.h"

namespace DDS { namespace Elements
{

TopicKey::TopicKey()
{ }

TopicKey::TopicKey(TopicKeyType key)
    : BaseLib::Templates::ComparableImmutableType< DDS::Elements::TopicKeyType >(key)
{ }

TopicKey::TopicKey(const BaseLib::InstanceHandle &handle)
    : BaseLib::Templates::ComparableImmutableType< DDS::Elements::TopicKeyType >( DDS::Elements::TopicKeyType(handle.GetHandle()) )
{ }

TopicKey::~TopicKey()
{ }

}}
