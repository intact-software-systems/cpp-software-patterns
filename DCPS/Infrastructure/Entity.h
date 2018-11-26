/*
 * Entity.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_Infrastructure_Entity_h_IsIncluded
#define DCPS_Infrastructure_Entity_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/Status/IncludeLibs.h"
#include"DCPS/Infrastructure/Listener.h"
#include"DCPS/Domain/DomainFwd.h"

#include"DCPS/Export.h"

namespace DCPS { namespace Status {
    class StatusCondition;
}}

namespace DCPS { namespace Infrastructure
{

/**
 * @brief The EntityState class is a part of the context of an entity
 */
class DLL_STATE EntityState
{
public:

private:
    static volatile unsigned int entityID_;

private:
    bool                            enabled_;
    DCPS::Status::StatusCondition	*statusCondition_;

    DCPS::Status::StatusMask 	statusMask_;
    DCPS::Status::StatusMask	statusChanges_;
};

/**
 * @brief The EntityConfig class
 */
class DLL_STATE EntityConfig
{
public:

};

/**
 * @brief
 *
 * This class is the abstract base class for all the DCPS objects that support QoS policies, a listener and
 * a status condition.
 */
class DLL_STATE Entity : public Lockable<Mutex>
{
public:
    Entity(bool enabled = true);
    Entity(const Status::StatusMask &mask, bool enabled = true);
    virtual ~Entity();

    CLASS_TRAITS(Entity)

    /**
     * @brief
     *
     * This operation is used to set the QoS policies of the Entity. This operation must be provided by each of the derived Entity
     * classes (DomainParticipant, Topic, Publisher, DataWriter, Subscriber, DataReader) so that the policies that are
     * meaningful to each Entity can be set.
     *
     * The set of policies specified as the qos_list parameter are applied on top of the existing QoS, replacing the values of any
     * policies previously set.
     *
     * As described in Section 7.1.3, Supported QoS, on page 96, certain policies are immutable; they can only be set at
     * Entity creation time, or before the entity is made enabled. If set_qos is invoked after the Entity is enabled and it attempts
     * to change the value of an immutable policy, the operation will fail and it returns IMMUTABLE_POLICY.
     *
     * Section 7.1.3, Supported QoS, on page 96 also describes that certain values of QoS policies can be incompatible with
     * the settings of the other policies. The set_qos operation will also fail if it specifies a set of values that once combined with
     * the existing values would result in an inconsistent set of policies. In this case, the return value is
     * INCONSISTENT_POLICY.
     *
     * If the application supplies a non-default value for a QoS policy that is not supported by the implementation of the service,
     * the set_qos operation will fail and return UNSUPPORTED.
     *
     * The existing set of policies are only changed if the set_qos operation succeeds. This is indicated by the OK return value.
     * In all other cases, none of the policies is modified.
     *
     * Each derived Entity class (DomainParticipant, Topic, Publisher, DataWriter, Subscriber, DataReader) has a
     * corresponding special value of the QoS (PARTICIPANT_QOS_DEFAULT, PUBLISHER_QOS_DEFAULT, SUBSCRIBER_QOS_DEFAULT, TOPIC_QOS_DEFAULT,
     * DATAWRITER_QOS_DEFAULT, DATAREADER_QOS_DEFAULT). This special value may be used as a parameter to the set_qos operation to indicate
     * that the QoS of the Entity should be changed to match the current default QoS set in the Entitys factory. The operation
     * set_qos cannot modify the immutable QoS so a successful return of the operation indicates that the mutable QoS for the
     * Entity has been modified to match the current default for the Entitys factory.
     *
     * Possible error codes returned in addition to the standard ones: INCONSISTENT_POLICY, IMMUTABLE_POLICY.
     */
    //virtual ReturnCode::Type SetQoS(QosPolicyList) = 0;

    /**
     * @brief
     *
     * This operation allows access to the existing set of QoS policies for the Entity. This operation must be provided by each
     * of the derived Entity classes (DomainParticipant, Topic, Publisher, DataWriter, Subscriber, DataReader) so that the
     * policies meaningful to the particular Entity are retrieved.
     */
    //virtual ReturnCode::Type GetQoS(QosPolicyList) = 0;

    /**
     * @brief
     *
     * This operation installs a Listener on the Entity. The listener will only be invoked on the changes of communication status
     * indicated by the specified mask.
     *
     * It is permitted to use nil as the value of the listener. The nil listener behaves as a Listener whose operations perform
     * no action.
     *
     * Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will replace it
     * with the new one. Consequently if the value nil is passed for the listener parameter to the set_listener operation, any
     * existing listener will be removed.
     *
     * This operation must be provided by each of the derived Entity classes (DomainParticipant, Topic, Publisher, DataWriter,
     * Subscriber, DataReader) so that the listener is of the concrete type suitable to the particular Entity.
     */
    //virtual ReturnCode::Type SetListener(Listener::Ptr &, Status::StatusMask &) = 0;

    /**
     * @brief
     *
     * This operation allows access to the existing Listener attached to the Entity.
     * This operation must be provided by each of the derived Entity classes (DomainParticipant, Topic, Publisher, DataWriter,
     * Subscriber, DataReader) so that the listener is of the concrete type suitable to the particular Entity.
     */
    //virtual Listener::Ptr GetListener() = 0;

public:
    /**
     * @brief
     *
     * This operation allows access to the StatusCondition (Section 7.1.2.1.9, StatusCondition Class) associated with the
     * Entity. The returned condition can then be added to a WaitSet (Section 7.1.2.1.6, WaitSet Class) so that the application
     * can wait for specific status changes that affect the Entity.
     */
    DCPS::Status::StatusCondition* GetStatusCondition();

    /**
     * TODO: Can I use this?
     */
    void NotifyStatusCondition();

    /**
     * @briefs
     *
     * This operation retrieves the list of communication statuses in the Entity that are triggered. That is, the list of statuses
     * whose value has changed since the last time the application read the status. The precise definition of the triggered state
     * of communication statuses is given in Section 7.1.4.2, Changes in Status, on page 126.
     *
     * When the entity is first created or if the entity is not enabled, all communication statuses are in the untriggered state so
     * the list returned by the get_status_changes operation will be empty.
     *
     * The list of statuses returned by the get_status_changes operation refers to the statuses that are triggered on the Entity
     * itself and does not include statuses that apply to contained entities.
     */
    virtual Status::StatusMask GetStatusChanges() const;

    /**
     * @brief
     *
     * This operation enables the Entity. Entity objects can be created either enabled or disabled. This is controlled by the value
     * of the ENTITY_FACTORY Qos policy (Section 7.1.3.20, ENTITY_FACTORY) on the corresponding factory for the
     * Entity.
     *
     * The default setting of ENTITY_FACTORY is such that, by default, it is not necessary to explicitly call enable on newly
     * created entities (see Section 7.1.3.20, ENTITY_FACTORY).
     *
     * The enable operation is idempotent. Calling enable on an already enabled Entity returns OK and has no effect.
     *
     * If an Entity has not yet been enabled, the following kinds of operations may be invoked on it:
     * - Operations to set or get an Entity's QoS policies (including default QoS policies) and listener
     * - get_statuscondition
     * - factory operations
     * - get_status_changes and other get status operations (although the status of a disabled entity never changes)
     * - lookup operations
     *
     * Other operations may explicitly state that they may be called on disabled entities; those that do not will return the error
     * NOT_ENABLED.
     *
     * It is legal to delete an Entity that has not been enabled by calling the proper operation on its factory.
     * Entities created from a factory that is disabled, are created disabled regardless of the setting of the ENTITY_FACTORY
     * Qos policy.
     * Calling enable on an Entity whose factory is not enabled will fail and return PRECONDITION_NOT_MET.
     * If the ENTITY_FACTORY Qos policy has autoenable_created_entities set to TRUE, the enable operation on the factory
     * will automatically enable all entities created from the factory.
     * The Listeners associated with an entity are not called until the entity is enabled. Conditions associated with an entity that
     * is not enabled are inactive, that is, have a trigger_value==FALSE (see Section 7.1.4.4, Conditions and Wait-sets, on
     * page 131).
     */
    virtual ReturnCode::Type Enable();

    /**
     * @brief IsEnabled
     * @return
     */
    bool IsEnabled() const;

    /**
     * @brief GetInstanceHandle returns the InstanceHandle_t that represents the Entity.
     * @return
     */
    virtual InstanceHandle GetInstanceHandle() const;

    /**
     * TODO: Implement logic
     */
    virtual void Close();

    /**
     * @brief
     *
     * Indicates that references to this object may go out of scope but that
     * the application expects to look it up again later. Therefore, the
     * Service must consider this object to be still in use and may not
     * close it automatically.
     */
    virtual void Retain();

    /**
     * @brief
     *
     * Associated with each one of an Entity’s communication status is a logical StatusChangedFlag.
     * This flag indicates whether that particular communication status has changed since the last
     * time the status was ‘read’ by the application. The way the status changes is slightly different
     * for the Plain Communication Status and the Read Communication status.
     *
     * Note that Figure 7.14 is only conceptual; it simply represents that the Entity knows which
     * specific statuses have changed. It does not imply any particular implementation of the
     * StatusChangedFlag in terms of boolean values.
     *
     * For the plain communication status, the StatusChangedFlag flag is initially set to FALSE. It
     * becomes TRUE whenever the plain communication status changes and it is reset to FALSE each
     * time the application accesses the plain communication status via the proper
     * get_<plain communication status> operation on the Entity.
     *
     * The communication status is also reset to FALSE whenever the associated listener operation is
     * called as the listener implicitly accesses the status which is passed as a parameter to the
     * operation. The fact that the status is reset prior to calling the listener means that if the
     * application calls the get_<plain communication status> from inside the listener it will see
     * the status already reset.
     *
     * An exception to this rule is when the associated listener is the 'nil' listener. As described
     * in Section 7.1.4.3.1, "Listener Access to Plain Communication Status," on page 129 the 'nil'
     * listener is treated as a NO-OP and the act of calling the 'nil' listener does not reset the
     * communication status.
     *
     * For example, the value of the StatusChangedFlag associated with the REQUESTED_DEADLINE_MISSED
     * status will become TRUE each time new deadline occurs (which increases the total_count field
     * within RequestedDeadlineMissedStatus). The value changes to FALSE when the application accesses
     * the status via the corresponding get_requested_deadline_missed_status method on the proper Entity.
     *
     * TODO: void StatusWasRead(Status);
     */
    void SetStatusChangedFlag(DCPS::Status::StatusMask &mask, bool isChanged);

    /**
     * @brief Enable
     * @param participant
     * @return
     */
    ReturnCode::Type EnableParticipant(Domain::DomainParticipantPtr participant);

protected:
    static volatile unsigned int entityID_;

protected:
    // TODO: Refactor into EntityState
    bool                            enabled_;
    DCPS::Status::StatusCondition	*statusCondition_;

    DCPS::Status::StatusMask 	statusMask_;
    DCPS::Status::StatusMask	statusChanges_;
};

}}

#endif

