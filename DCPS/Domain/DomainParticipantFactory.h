/*
 * DomainParticipantFactory.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Domain_DomainParticipantFactory_h_IsIncluded
#define DCPS_Domain_DomainParticipantFactory_h_IsIncluded

#include"DCPS/CommonDefines.h"
#include"DCPS/Domain/DomainParticipantListener.h"
#include"DCPS/Domain/DomainParticipant.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Domain
{

/**
 * @brief The DomainParticipantFactoryConfig class
 */
class DLL_STATE DomainParticipantFactoryConfig
{
public:
    DomainParticipantFactoryConfig();
    DomainParticipantFactoryConfig(DDS::DomainParticipantFactoryQoS factoryQos,
                                   DDS::DomainParticipantQoS defaultDomainParticipantQos);
    virtual ~DomainParticipantFactoryConfig();

    /**
     * This operation retrieves the default value of the DomainParticipant
     * QoS, that is, the QoS policies which will be used for newly created
     * {@link DomainParticipant} entities in the case where the QoS policies
     * are defaulted in the {@link #createParticipant()} operation.
     *
     * The values retrieved will match the set of values specified on the
     * last successful call to
     * {@link #setDefaultParticipantQos(DomainParticipantQos)}, or else, if
     * the call was never made, the default values identified by the DDS
     * specification.
     */
    DDS::DomainParticipantQoS DefaultDomainParticipantQos() const;

    /**
     * This operation sets a default value of the DomainParticipant QoS
     * policies which will be used for newly created
     * {@link DomainParticipant} entities in the case where the QoS policies
     * are defaulted in the {@link #createParticipant()} operation.
     *
     * @throws InconsistentPolicyException if the resulting policies are
     * not self consistent; in that case, the operation will have no
     * effect.
     */
    void SetDefaultDomainParticipantQos(const DDS::DomainParticipantQoS &defaultDomainParticipantQos);

    /**
     * This operation returns the value of the OSPLDomainParticipantFactory QoS
     * policies.
     */
    DDS::DomainParticipantFactoryQoS DomainParticipantFactoryQos() const;

    /**
     * This operation sets the value of the OSPLDomainParticipantFactory QoS
     * policies. These policies control the behavior of the object, a factory
     * for entities.
     *
     * Note that despite having QoS, the OSPLDomainParticipantFactory is not an
     * {@link Entity}.
     *
     * @throws InconsistentPolicyException if the resulting policies are
     * not self consistent; in that case, the operation will have no
     * effect.
     */
    void SetDomainParticipantFactoryQos(const DDS::DomainParticipantFactoryQoS &domainParticipantFactoryQos);

private:
    /**
     * @brief domainParticipantFactoryQos_ is the current QoS for this factory.
     */
    DDS::DomainParticipantFactoryQoS domainParticipantFactoryQos_;

    /**
     * @brief defaultDomainParticipantQos_ is a holder for the default DomapinParticipant QoS.
     */
    DDS::DomainParticipantQoS defaultDomainParticipantQos_;
};

/**
 * @brief The DomainParticipantFactoryState class
 */
class DLL_STATE DomainParticipantFactoryState
{
public:
    DomainParticipantFactoryState();
    DomainParticipantFactoryState(RxData::CacheDescription description);
    virtual ~DomainParticipantFactoryState();

    DCPSConfig::DomainParticipantAccess participants();

private:
    /**
     * @brief cache_
     */
    RxData::Cache::Ptr cache_;
};

/**
 * @brief The DomainParticipantFactory class is a singleton to create and keep track of DomainParticipant objects.
 */
class DLL_STATE DomainParticipantFactory
        : public Templates::ContextObject<DomainParticipantFactoryConfig, DomainParticipantFactoryState, BaseLib::Templates::NullCriticalState>
{
private:
    /**
     * @brief Empty constructor for the DomainParticipantFactory gives default QoS profiles.
     */
    DomainParticipantFactory();

    /**
     * @brief Constructor for DomainParticipantFactory that enables user defined QoS for the factory and also changing
     * the default QoS DomainParticipant objects.
     */
    DomainParticipantFactory(DDS::DomainParticipantQoS defaultParticipantQos, DDS::DomainParticipantFactoryQoS defaultFactoryQos);
    virtual ~DomainParticipantFactory();

    friend class Singleton<DomainParticipantFactory>;

public:
    // -----------------------------------------------------------------------
    // Singleton Access
    // -----------------------------------------------------------------------

    /**
     * This operation returns the OSPLDomainParticipantFactory
     * singleton. The operation is idempotent, that is, it can be called
     * multiple times without side effects, and each time it will return a
     * OSPLDomainParticipantFactory instance that is equal to the previous
     * results.
     */
    static DomainParticipantFactory& Instance();

public:
    // -----------------------------------------------------------------------
    // Instance Methods
    // -----------------------------------------------------------------------

    /**
     * Create a new participant in the domain with ID 0 having default QoS
     * and no listener.
     */
    DomainParticipant::Ptr CreateParticipant();

    /**
     * This operation creates a new DomainParticipant object. The
     * DomainParticipant signifies that the calling application intends to
     * join the domain identified by the domainId argument.
     */
    DomainParticipant::Ptr CreateParticipant(const DDS::Elements::DomainId &domainId);

    /**
     * This operation creates a new DomainParticipant object having default
     * QoS and no listener. The DomainParticipant signifies that the calling
     * application intends to join the domain identified by the domainId
     * argument.
     *
     * @param statuses Of which status changes the listener should be notified.
     */
    DomainParticipant::Ptr CreateParticipant(
            const DDS::Elements::DomainId &domainId,
            const DDS::DomainParticipantQoS &qos,
            const DomainParticipantListener::Ptr &listener,
            const Status::StatusMask &statuses);

    /**
     * Create a new domain participant.
     *
     * @param statuses Of which status changes the listener should be
     * notified. A null collection signifies all status
     * changes.
     *
     * @see #createParticipant(int, DomainParticipantQos, DomainParticipantListener, Collection)
     */
    //DomainParticipant::Ptr CreateParticipant(
    //									const DomainId &domainId,
    //									const std::string &qosLibraryName,
    //									const std::string qosProfileName,
    //									const DomainParticipantListener::Ptr &listener,
    //									const Status::StatusMask &statuses);

    /**
     * This operation retrieves a previously created DomainParticipant
     * belonging to specified domain ID. If no such DomainParticipant exists,
     * the operation will return null.
     *
     * If multiple DomainParticipant entities belonging to that domain ID
     * exist, then the operation will return one of them. It is not specified
     * which one.
     */
    DomainParticipant::Ptr LookupParticipant(const DDS::Elements::DomainId &domainId);

private:

    /**
     * @brief createParticipant is a private function to create DomainParticipant objects.
     */
    DomainParticipant::Ptr createParticipant(
            const DDS::Elements::DomainId &domainId,
            const DDS::DomainParticipantQoS &qos,
            const DomainParticipantListener::Ptr &listener,
            const Status::StatusMask &statuses);

private:
    /**
     * @brief factory_ is the singleton object of this factory
     */
    static BaseLib::Singleton<DomainParticipantFactory> factory_;
};

}}

#endif
