#ifndef PARTICIPANTPROXY_H
#define PARTICIPANTPROXY_H

#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*
+@protocolVersion : ProtocolVersion_t
+@guidPrefix : GuidPrefix_t
+@vendorId : VendorId_t
+@expectsInlineQos : bool
+availableBuiltinEndpoints : BuiltinEndpointSet_t[]
+metatrafficUnicastLocatorList : Locator_t[]
+metatrafficMulticastLocatorList : Locator_t[]
+defaultMulticastLocatorList : Locator_t[]
+defaultUnicastLocatorList : Locator_t[]
+manualLivelinessCount : Count_t
*/

/**
 * @brief The ParticipantProxy class
 */
class DLL_STATE ParticipantProxy
{
public:
    ParticipantProxy();
    ParticipantProxy(const ProtocolVersion &protocolVersion,
                     const GuidPrefix &guidPrefix,
                     const VendorId &vendorId,
                     bool expectsInlineQos,
                     const LocatorVector &metatrafficUnicastLocatorList,
                     const LocatorVector &metatrafficMulticastLocatorList,
                     const LocatorVector &defaultMulticastLocatorList,
                     const LocatorVector &defaultUnicastLocatorList,
                     const Count &manualLivelinessCount);
    virtual ~ParticipantProxy();

public:
    ProtocolVersion protocolVersion() const;
    void setProtocolVersion(const ProtocolVersion &protocolVersion);

    GuidPrefix guidPrefix() const;
    void setGuidPrefix(const GuidPrefix &guidPrefix);

    VendorId vendorId() const;
    void setVendorId(const VendorId &vendorId);

    bool expectsInlineQos() const;
    void setExpectsInlineQos(bool expectsInlineQos);

    LocatorVector metatrafficUnicastLocatorList() const;
    void setMetatrafficUnicastLocatorList(const LocatorVector &metatrafficUnicastLocatorList);

    LocatorVector metatrafficMulticastLocatorList() const;
    void setMetatrafficMulticastLocatorList(const LocatorVector &metatrafficMulticastLocatorList);

    LocatorVector defaultMulticastLocatorList() const;
    void setDefaultMulticastLocatorList(const LocatorVector &defaultMulticastLocatorList);

    LocatorVector defaultUnicastLocatorList() const;
    void setDefaultUnicastLocatorList(const LocatorVector &defaultUnicastLocatorList);

    Count manualLivelinessCount() const;
    void setManualLivelinessCount(const Count &manualLivelinessCount);

private:
    ProtocolVersion protocolVersion_;
    GuidPrefix      guidPrefix_;
    VendorId        vendorId_;
    bool            expectsInlineQos_;

    // TODO: BuiltinEndpointSet     availableBuiltinEndpoints_;
    LocatorVector   metatrafficUnicastLocatorList_;
    LocatorVector   metatrafficMulticastLocatorList_;
    LocatorVector   defaultMulticastLocatorList_;
    LocatorVector   defaultUnicastLocatorList_;
    Count           manualLivelinessCount_;
};

}

#endif // PARTICIPANTPROXY_H
