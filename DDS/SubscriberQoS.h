#ifndef DDS_Subscription_SubscriberQoS_h_IsIncluded
#define DDS_Subscription_SubscriberQoS_h_IsIncluded

#include"DDS/Export.h"

namespace DDS
{

class DLL_STATE SubscriberQoS
{
public:
    SubscriberQoS();

/*public:
   void policy(const Policy::Partition& partition) {
      partition_ = partition;
   }
private:
   Policy::Partition             partition_;
*/
};

}

#endif
