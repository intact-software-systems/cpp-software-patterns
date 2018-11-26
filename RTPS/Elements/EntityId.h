/*
 * EntityId.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_EntityId_h_IsIncluded
#define RTPS_Elements_EntityId_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/CommonDefines.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
    Mapping of the type GuidPrefix_t
    The PSM maps the EntityId_t to the following structure:

    struct EntityId_t
    {
        octet[3] entityKey;
        octet entityKind;
    };
    typedef EntityId_t EntityId;

The reserved constant ENTITYID_UNKNOWN defined by the PIM is mapped to:

#define ENTITYID_UNKNOWN {0x00, 0x00, 0x00, 0x00}

-----------------------------------------------------
The information on whether the object is a built-in entity,
a vendor-specific entity, or a user-defined entity is
encoded in the two most-significant bits of the entityKind.
These two bits are set to:
-----------------------------------------------------
- '00' for user-defined entities.
- '11' for built-in entities.
- '01' for vendor-specific entities.

-----------------------------------------------------
Table 9.1 - entityKind octet of an EntityId_t
-----------------------------------------------------
Kind of Entity      User-defined Entity     Built-in Entity
unknown             0x00                    0xc0
Participant         N/A                     0xc1
Writer (with Key)   0x02                    0xc2
Writer (no Key)     0x03                    0xc3
Reader (no Key)     0x04                    0xc4
Reader (with Key)   0x07                    0xc7

-----------------------------------------------------
EntityId_t values fully predefined by the RTPS Protocol
-----------------------------------------------------

// participant
#define ENTITYID_PARTICIPANT                                {00,00,01,c1}

// SEDPbuiltinTopicWriter
#define ENTITYID_SEDP_BUILTIN_TOPIC_WRITER                  {00,00,02,c2}

// SEDPbuiltinTopicReader
#define ENTITYID_SEDP_BUILTIN_TOPIC_READER                  {00,00,02,c7}

// SEDPbuiltinPublicationsWriter
#define ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER           {00,00,03,c2}

// SEDPbuiltinPublicationsReader
#define ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER           {00,00,03,c7}

// SEDPbuiltinSubscriptionsWriter
#define ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER          {00,00,04,c2}

// SEDPbuiltinSubscriptionsReader
#define ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER          {00,00,04,c7}

// SPDPbuiltinParticipantWriter
#define ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER            {00,01,00,c2}

// SPDPbuiltinSdpParticipantReader
#define ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER            {00,01,00,c7}

// BuiltinParticipantMessageWriter
#define ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER     {00,02,00,C2}

// BuiltinParticipantMessageReader
#define ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER     {00,02,00,C7}
---------------------------------------------------------------------------------------------*/

/**
 * @brief The EntityId uniquely identifies the Entity within the Participant.
 *
 * The selection of entityId for each RTPS Entity depends on the PSM.
 */
class DLL_STATE EntityId : public NetworkLib::NetObjectBase
{
public:
    EntityId(const std::vector<octet> &key, const octet &kind)
        : entityKey_(key)
        , entityKind_(kind)
    {
        if(entityKey_.size() != EntityId::KEY_SIZE())
            throw CriticalException("Wrong entityId given!");
    }
    EntityId()
        : entityKey_(EntityId::KEY_SIZE(),0x00)
        , entityKind_(0)
    { }
    virtual ~EntityId()
    {}

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        for(size_t i = 0; i < EntityId::KEY_SIZE(); i++)
            writer->WriteInt8(entityKey_[i]);

        writer->WriteInt8(entityKind_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        for(size_t i = 0; i < EntityId::KEY_SIZE(); i++)
            entityKey_[i] = reader->ReadInt8();

        entityKind_ = reader->ReadInt8();
    }

public:
    static EntityId	UNKNOWN()
    {
        static EntityId id;
        return id;
    }

    static unsigned int SIZE()
    {
        static unsigned int sz = 4;
        return sz;
    }

    static unsigned int KEY_SIZE()
    {
        static unsigned int sz = 3;
        return sz;
    }

    static EntityId EntityId_SPDP_BUILTIN_PARTICIPANT_WRITER()
    {
        std::vector<unsigned char> key = ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
        EntityId id;
        id.entityKey_.push_back(key[0]);
        id.entityKey_.push_back(key[1]);
        id.entityKey_.push_back(key[2]);
        id.entityKind_ = key[3];

        return id;
    }


    static EntityId EntityId_ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER()
    {
        std::vector<unsigned char> key = ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
        EntityId id;
        id.entityKey_.push_back(key[0]);
        id.entityKey_.push_back(key[1]);
        id.entityKey_.push_back(key[2]);
        id.entityKind_ = key[3];

        return id;
    }

    static octet EntityKind_USERDEFINED_WRITER_WITHKEY()
    {
        return USERDEFINED_WRITER_WITHKEY;
    }


public:
    bool operator==(const EntityId &other) const
    {
        return (entityKey_ == other.entityKey_) && (entityKind_ == other.entityKind_);
    }

    bool operator!=(const EntityId &other) const
    {
        return !(*this == other);
    }

    bool operator<(const EntityId &other) const
    {
        return this->entityKey_ < other.entityKey_;
    }

public:
    inline void 	SetEntityKey(const std::vector<octet> &key)		{ entityKey_ = key; }
    inline void		SetEntityKind(const octet &kind)				{ entityKind_ = kind; }

    inline std::vector<octet> 	GetEntityKey()	const	{ return entityKey_; }
    inline octet				GetEntityKind()	const 	{ return entityKind_; }

private:
    std::vector<octet>	entityKey_;
    octet 	 			entityKind_;
};

} // namespace RTPS

#endif // RTPS_Elements_EntityId_h_IsIncluded
