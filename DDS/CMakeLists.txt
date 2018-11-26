cmake_minimum_required(VERSION 3.2)
project(DDS)

add_definitions(-DMAKE_DLL_DDS)

SET(LIB_NAME DDS)
SET(EXTRA_LIBS BaseLib RxData NetworkLib)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
    Discovery/IncludeLibs.h
    Discovery/ParticipantBuiltinTopicData.cpp
    Discovery/ParticipantBuiltinTopicData.h
    Discovery/PublicationBuiltinTopicData.cpp
    Discovery/PublicationBuiltinTopicData.h
    Discovery/SubscriptionBuiltinTopicData.cpp
    Discovery/SubscriptionBuiltinTopicData.h
    Discovery/TopicBuiltinTopicData.cpp
    Discovery/TopicBuiltinTopicData.h
    Elements/DomainId.cpp
    Elements/DomainId.h
    Elements/FilterExpression.cpp
    Elements/FilterExpression.h
    Elements/FilterExpressionParameters.cpp
    Elements/FilterExpressionParameters.h
    Elements/IncludeLibs.h
    Elements/TopicKey.cpp
    Elements/TopicKey.h
    Elements/TopicName.cpp
    Elements/TopicName.h
    Elements/TypeName.cpp
    Elements/TypeName.h
    Factory/DDSConfig.cpp
    Factory/DDSConfig.h
    Policy/CorePolicyFwd.h
    Policy/DataRepresentation.h
    Policy/Deadline.h
    Policy/DestinationOrder.h
    Policy/Durability.h
    Policy/DurabilityService.h
    Policy/EntityFactory.h
    Policy/GroupData.h
    Policy/History.h
    Policy/IncludeLibs.h
    Policy/LatencyBudget.h
    Policy/Lifespan.h
    Policy/Liveliness.h
    Policy/Ownership.h
    Policy/OwnershipStrength.h
    Policy/Partition.h
    Policy/PolicyKind.h
    Policy/Presentation.h
    Policy/QosChecker.cpp
    Policy/QosChecker.h
    Policy/QosPolicyBase.h
    Policy/QosPolicyCount.h
    Policy/ReaderDataLifecycle.h
    Policy/Reliability.h
    Policy/ResourceLimits.h
    Policy/TimeBasedFilter.h
    Policy/TopicData.h
    Policy/TransportPriority.h
    Policy/TypeConsistencyEnforcement.h
    Policy/UserData.h
    Policy/WriterDataLifecycle.h
    CommonDefines.h
    DataReaderQoS.cpp
    DataReaderQoS.h
    DataWriterQoS.cpp
    DataWriterQoS.h
    DomainParticipantFactoryQoS.cpp
    DomainParticipantFactoryQoS.h
    DomainParticipantQoS.cpp
    DomainParticipantQoS.h
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h
    PublisherQoS.cpp
    PublisherQoS.h
    SubscriberQoS.cpp
    SubscriberQoS.h
    TopicQoS.cpp
    TopicQoS.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
