#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * @brief
 *
 * This policy controls the behavior of the Entity as a factory for other
 * entities. This policy concerns only DomainParticipant (as factory for
 * Publisher, Subscriber, and Topic), Publisher (as factory for DataWriter),
 * and Subscriber (as factory for DataReader). This policy is mutable.
 * A change in the policy affects only the entities created after the change;
 * not the previously created entities.
 * The setting of autoenable_created_entities to TRUE indicates that the
 * newly created object will be enabled by default.
 * A setting of FALSE indicates that the Entity will not be automatically
 * enabled. The application will need to enable it explicitly by means of the
 * enable operation (see Section 7.1.2.1.1.7, enable). The default setting
 * of autoenable_created_entities = TRUE means that, by default, it is not
 * necessary to explicitly call enable on newly created entities.
 */
class DLL_STATE Activation
{
public:
    Activation() : autoActivate_(true)
    { }

    Activation(bool autoActivate)
        : autoActivate_(autoActivate)
    { }

    bool IsAutoActivate() const
    {
        return autoActivate_;
    }

    static Activation AutoActivate()
    {
        static Activation entityFactory(true);

        return entityFactory;
    }

    static Activation ManuallyActivate()
    {
        static Activation entityFactory(false);

        return entityFactory;
    }

private:
    bool autoActivate_;
};

}}
