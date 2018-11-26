#pragma once

#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/Export.h"

namespace NetworkLib
{

/**
 * - Policy on max number of socket connections on server.
 * - Policy on what to do when resource limits met, ex spawn another server or refuse.
 * - Load balance between existing servers, round robin or fill up first.
 */

class DLL_STATE QosConnectionPolicy
{
public:
    QosConnectionPolicy(int maxNumConnections)
        : maxNumConnections_(maxNumConnections)
    {}
    ~QosConnectionPolicy() {}

    int GetMaxNumConnections() const { return maxNumConnections_; }

private:
    int maxNumConnections_;
};

namespace LoadPolicyKind
{
    enum Type
    {
        ROUND_ROBIN,
        FILL_UP_FIRST
    };
}

class DLL_STATE QosLoadPolicy
{
public:
    QosLoadPolicy(LoadPolicyKind::Type kind)
        : kind_(kind)
    {}
    ~QosLoadPolicy()
    {}

    LoadPolicyKind::Type GetKind() const { return kind_; }

    bool operator==(const QosLoadPolicy &policy) const
    {
        return kind_ == policy.kind_;
    }

    bool operator!=(const QosLoadPolicy &policy) const
    {
        return kind_ != policy.kind_;
    }

private:
    LoadPolicyKind::Type kind_;
};

class DLL_STATE QosSocketAcceptorStatus
{
public:
    QosSocketAcceptorStatus(int numAcceptedSockets)
        : numAcceptedSockets_(numAcceptedSockets)
    {}
    ~QosSocketAcceptorStatus()
    {}

    int GetNumAcceptedSocket() const { return numAcceptedSockets_; }

private:
    int numAcceptedSockets_;
};

}
