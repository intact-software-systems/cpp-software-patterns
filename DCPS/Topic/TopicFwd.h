#ifndef DCPS_Topic_TopicFwd_h_IsIncluded
#define DCPS_Topic_TopicFwd_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"

namespace DCPS { namespace Topic
{

template <typename DATA>
class Topic;

template <typename DATA>
class ContentFilteredTopic;

template <typename DATA>
class TopicInstance;

template <typename DATA>
class TopicDescription;

class TopicHolder;

FORWARD_CLASS_TRAITS(TopicHolder)

}}

#endif // DCPS_Topic_TopicFwd_h_IsIncluded
