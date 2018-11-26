#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * @brief
 *
 * The purpose of this QoS is to allow the application to attach additional
 * information to the created Entity objects such that when a remote application
 * discovers their existence it can access that information and use it for its
 * own purposes. One possible use of this QoS is to attach security credentials
 * or some other information that can be used by the remote application to
 * authenticate the source. In combination with operations such as
 * ignore_participant, ignore_publication, ignore_subscription,
 * and ignore_topic these QoS can assist an application to define and enforce
 * its own security policies. The use of this QoS is not limited to security,
 * rather it offers a simple, yet flexible extensibility mechanism.
 */
class DLL_STATE UserData
{
    typedef std::vector<uint8_t> ByteSeq;

public:
    UserData()
        : value_()
    { }

    UserData(const ByteSeq& seq)
        : value_(seq)
    { }

    virtual ~UserData()
    { }

    static UserData Empty()
    {
        return UserData();
    }

    const ByteSeq& GetValue() const
    {
        return value_;
    }

private:
    ByteSeq value_;
};

}}
