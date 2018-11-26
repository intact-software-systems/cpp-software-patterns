cmake_minimum_required(VERSION 3.2)

SET(LIB_NAME RxTransport)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

project(${LIB_NAME})
add_definitions(-DMAKE_DLL_${LIB_NAME})

set(SOURCE_FILES
        IncludeLibs.h
        Export.h
        IncludeExtLibs.h
        CommonDefines.h
        MessagePolicy.h
        MessagePolicy.cpp
        Message.h
        MessageFuture.h
        MessageChannel.h
        MessageController.h
        MessageControllerData.h
        MessageControllerPolicy.cpp
        MessageControllerPolicy.h
        MessageBuilder.cpp
        MessageBuilder.h
        MessageCache.h
        ChannelFactory.cpp
        ChannelFactory.h
        ChannelFeedbackEvent.h
        PublisherChannel.h
        SubscriberChannel.h
        RWChannel.h
        MessageControllerAlgorithms.h
        Reader/MessageReaderPolicy.h
        Reader/MessageReaderPolicy.cpp
        Reader/MessageReader.h
        Reader/MessageReaderFunctions.h
        Reader/MessageReaderFunctions.cpp
        Reader/MessageReaderData.h
        Reader/MessageReaderAlgorithms.h
        Reader/MessageReaders.h
        Reader/MessageWriterProxy.h
        Reader/MessageWriterProxy.cpp
        Reader/MessageWriterProxies.cpp
        Reader/MessageWriterProxies.h
        Reader/MessageWriterProxyAlgorithms.cpp
        Reader/MessageWriterProxyAlgorithms.h
        Reader/MessageReaderPolicyAlgorithms.h
        Writer/MessageWriter.h
        Writer/MessageWriters.h
        Writer/MessageWriterPolicy.h
        Writer/MessageWriterPolicy.cpp
        Writer/MessageWriterFunctions.h
        Writer/MessageWriterFunctions.cpp
        Writer/MessageWriterAlgorithms.h
        Writer/MessageWriterPolicyAlgorithms.h
        Writer/MessageWriterData.h
        Writer/MessageReaderProxy.h
        Writer/MessageReaderProxy.cpp
        Writer/MessageReaderProxies.h
        Writer/MessageReaderProxies.cpp
        Writer/MessageReaderProxyAlgorithms.cpp
        Writer/MessageReaderProxyAlgorithms.h
        )

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
