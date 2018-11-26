#ifndef BaseLib_Policy_Communication_h_IsIncluded
#define BaseLib_Policy_Communication_h_IsIncluded

#include "BaseLib/Export.h"

namespace BaseLib {
namespace Policy {

enum class DLL_STATE CommunicationKind
{
    Unicast,
    Multicast,
    Broadcast
};

class DLL_STATE Communication
{
public:
    Communication(CommunicationKind kind, int numToSend = 1)
        : kind_(kind)
        , numToSend_(numToSend)
    { }

    virtual ~Communication()
    { }

    const CommunicationKind& Kind() const
    {
        return kind_;
    }

    int NumToSend() const
    {
        return numToSend_;
    }

    static Communication Unicast(int numSend = 1)
    {
        return Communication(CommunicationKind::Unicast, numSend);
    }

    static Communication Multicast(int numSend = 1)
    {
        return Communication(CommunicationKind::Multicast, numSend);
    }

    static Communication Broadcast(int numSend = 1)
    {
        return Communication(CommunicationKind::Broadcast, numSend);
    }

private:
    CommunicationKind kind_;
    int               numToSend_;
};

}
}

#endif
