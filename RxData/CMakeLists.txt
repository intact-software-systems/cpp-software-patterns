cmake_minimum_required(VERSION 3.2)
project(RxData)

add_definitions(-DMAKE_DLL_RxData)

SET(LIB_NAME RxData)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
    cache/collection/Collection.h
    cache/collection/DataFlyweightWeakReference.h
    cache/collection/DataList.h
    cache/collection/DataMap.h
    cache/collection/DataSet.h
    cache/decorator/CacheDecorator.h
    cache/interfaces/ObjectAccess.h
    cache/interfaces/ObjectLoader.h
    cache/interfaces/ObjectReader.h
    cache/interfaces/ObjectReference.h
    cache/interfaces/ObjectWriter.h
    cache/object/access/ObjectAccessProxy.h
    cache/object/interfaces/ObjectCacheProxy.h
    cache/object/interfaces/ObjectCacheReader.h
    cache/object/interfaces/ObjectCacheWriter.h
    cache/object/interfaces/ObjectHomeBase.h
    cache/object/policy/ObjectHomePolicy.h
    cache/object/reader/ObjectReaderProxy.h
    cache/object/reader/ObjectReaderStub.h
    cache/object/writer/ObjectWriterProxy.h
    cache/object/ObjectCache.cpp
    cache/object/ObjectCache.h
    cache/object/ObjectHome.h
    cache/object/ObjectHomeBase.h
    cache/object/ObjectId.h
    cache/object/ObjectReference.h
    cache/object/ObjectRoot.cpp
    cache/object/ObjectRoot.h
    cache/object/ObjectState.h
    cache/observer/CacheListener.h
    cache/observer/CacheReaderListener.h
    cache/observer/DataReaderListener.h
    cache/observer/ObjectListener.h
    cache/observer/SelectionListener.h
    cache/policy/Ejection.h
    cache/policy/Lifespan.h
    cache/policy/ResourceLimits.h
    cache/relation/RelationDescription.h
    cache/relation/RelationKind.h
    cache/relation/RelationName.h
    cache/scope/ObjectScope.h
    cache/scope/ReferenceScope.h
    cache/selection/FilterCriterion.h
    cache/selection/MembershipState.h
    cache/selection/QueryCriterion.h
    cache/selection/Selection.h
    cache/selection/SelectionBase.h
    cache/selection/SelectionCriteria.h
    cache/selection/SelectionCriterion.h
    cache/strategy/EvictionStrategy.h
    cache/strategy/LeastFrequentlyUsed.h
    cache/strategy/LeastRecentlyUsed.h
    cache/subject/CacheSubject.h
    cache/subject/ObjectSubject.h
    cache/subject/SelectionSubject.h
    cache/AccessObjectCache.h
    cache/Cache.cpp
    cache/Cache.h
    cache/CacheAccess.cpp
    cache/CacheAccess.h
    cache/CacheAccessInterface.h
    cache/CacheBase.cpp
    cache/CacheBase.h
    cache/CacheDescription.cpp
    cache/CacheDescription.h
    cache/CacheFactory.cpp
    cache/CacheFactory.h
    cache/CacheInterface.h
    cache/CacheKind.h
    cache/CacheUsage.h
    cache/Contract.h
    cache/IncludeLibs.h
    machine/IncludeLibs.h
    machine/VirtualMachineCache.cpp
    machine/VirtualMachineCache.h
    CommonDefines.h
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
