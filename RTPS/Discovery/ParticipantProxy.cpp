#include "RTPS/Discovery/ParticipantProxy.h"

namespace RTPS
{

ParticipantProxy::ParticipantProxy()
{
}

ParticipantProxy::ParticipantProxy(
        const ProtocolVersion &protocolVersion,
        const GuidPrefix &guidPrefix,
        const VendorId &vendorId,
        bool expectsInlineQos,
        const LocatorVector &metatrafficUnicastLocatorList,
        const LocatorVector &metatrafficMulticastLocatorList,
        const LocatorVector &defaultMulticastLocatorList,
        const LocatorVector &defaultUnicastLocatorList,
        const Count &manualLivelinessCount)
    : protocolVersion_(protocolVersion)
    , guidPrefix_(guidPrefix)
    , vendorId_(vendorId)
    , expectsInlineQos_(expectsInlineQos)
    , metatrafficUnicastLocatorList_(metatrafficUnicastLocatorList)
    , metatrafficMulticastLocatorList_(metatrafficMulticastLocatorList)
    , defaultMulticastLocatorList_(defaultMulticastLocatorList)
    , defaultUnicastLocatorList_(defaultUnicastLocatorList)
    , manualLivelinessCount_(manualLivelinessCount)
{

}

ParticipantProxy::~ParticipantProxy()
{

}
ProtocolVersion ParticipantProxy::protocolVersion() const
{
    return protocolVersion_;
}

void ParticipantProxy::setProtocolVersion(const ProtocolVersion &protocolVersion)
{
    protocolVersion_ = protocolVersion;
}
GuidPrefix ParticipantProxy::guidPrefix() const
{
    return guidPrefix_;
}

void ParticipantProxy::setGuidPrefix(const GuidPrefix &guidPrefix)
{
    guidPrefix_ = guidPrefix;
}
VendorId ParticipantProxy::vendorId() const
{
    return vendorId_;
}

void ParticipantProxy::setVendorId(const VendorId &vendorId)
{
    vendorId_ = vendorId;
}
bool ParticipantProxy::expectsInlineQos() const
{
    return expectsInlineQos_;
}

void ParticipantProxy::setExpectsInlineQos(bool expectsInlineQos)
{
    expectsInlineQos_ = expectsInlineQos;
}
LocatorVector ParticipantProxy::metatrafficUnicastLocatorList() const
{
    return metatrafficUnicastLocatorList_;
}

void ParticipantProxy::setMetatrafficUnicastLocatorList(const LocatorVector &metatrafficUnicastLocatorList)
{
    metatrafficUnicastLocatorList_ = metatrafficUnicastLocatorList;
}
LocatorVector ParticipantProxy::metatrafficMulticastLocatorList() const
{
    return metatrafficMulticastLocatorList_;
}

void ParticipantProxy::setMetatrafficMulticastLocatorList(const LocatorVector &metatrafficMulticastLocatorList)
{
    metatrafficMulticastLocatorList_ = metatrafficMulticastLocatorList;
}
LocatorVector ParticipantProxy::defaultMulticastLocatorList() const
{
    return defaultMulticastLocatorList_;
}

void ParticipantProxy::setDefaultMulticastLocatorList(const LocatorVector &defaultMulticastLocatorList)
{
    defaultMulticastLocatorList_ = defaultMulticastLocatorList;
}
LocatorVector ParticipantProxy::defaultUnicastLocatorList() const
{
    return defaultUnicastLocatorList_;
}

void ParticipantProxy::setDefaultUnicastLocatorList(const LocatorVector &defaultUnicastLocatorList)
{
    defaultUnicastLocatorList_ = defaultUnicastLocatorList;
}
Count ParticipantProxy::manualLivelinessCount() const
{
    return manualLivelinessCount_;
}

void ParticipantProxy::setManualLivelinessCount(const Count &manualLivelinessCount)
{
    manualLivelinessCount_ = manualLivelinessCount;
}

}
