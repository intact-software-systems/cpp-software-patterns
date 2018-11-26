#pragma once

#include"NetworkLib/IncludeExtLibs.h"

namespace NetworkLib {

/**
- TODO:
    If the receiver component is expected to respond to a message it receives, the message header should contain a RETURN ADDRESS.
    The specific response is also encapsulated in an appro- priate message, which leads to a REQUEST-REPLY scenario.
    A CORRELATION IDENTIFIER in the message header—which is basically an ASYNCHRONOUS COMPLETION TOKEN — indicates to
    the receiver of the reply message to which concrete request message the reply corresponds.

    If the size of the information to transmit exceeds the maximum message length, break it into multiple smaller messages that
    are transmitted to the receiver as a MESSAGE SEQUENCE. If the data encapsulated in a message is only useful or valid for a
    limited time, specify a corresponding MESSAGE EXPIRATION in the message header.
*/

/**
 * Templated implementation of the "Message" software design pattern. This pattern
 * states that a message shall be divided into a header and a payload part.
 *
 * The header contains meta-information about source, destination, timestamps, etc.
 * The payload contains the domain data to be received and executed upon by the receiving party.
 */
template <typename SERIALIZER, typename HEADER, typename PAYLOAD>
class MessageBase : public Templates::Lockable<Mutex>
                    , public Templates::SplitSerializer<SERIALIZER>
{
public:
    MessageBase(const HEADER& header, const PAYLOAD& payload)
        : header_(header)
        , payload_(payload)
    {}

    virtual ~MessageBase()
    {}

    CLASS_TRAITS(MessageBase)

    /**
     * @brief Read serializes data from header and payload
     * @param archive the deserializer
     */
    virtual void Save(SERIALIZER& archive) const
    {
        archive(header_, payload_);
    }

    /**
     * @brief Write deserializes data from header and payload
     * @param archive is the serializer
     */
    virtual void Load(SERIALIZER& archive)
    {
        archive(header_, payload_);
    }

private:
    Templates::ComparableImmutableType<HEADER>  header_;
    Templates::ComparableImmutableType<PAYLOAD> payload_;
};

}
