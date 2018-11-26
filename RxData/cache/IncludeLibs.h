#pragma once

#include"RxData/cache/Cache.h"
#include"RxData/cache/CacheAccess.h"
#include"RxData/cache/CacheBase.h"
#include"RxData/cache/CacheDescription.h"
#include"RxData/cache/CacheFactory.h"
#include"RxData/cache/CacheKind.h"
#include"RxData/cache/CacheUsage.h"
#include"RxData/cache/Contract.h"

#include"RxData/cache/collection/Collection.h"
#include"RxData/cache/collection/DataFlyweightWeakReference.h"
#include"RxData/cache/collection/DataList.h"
#include"RxData/cache/collection/DataMap.h"
#include"RxData/cache/collection/DataSet.h"

#include"RxData/cache/interfaces/ObjectLoader.h"
#include"RxData/cache/interfaces/ObjectReader.h"
#include"RxData/cache/interfaces/ObjectReference.h"
#include"RxData/cache/interfaces/ObjectWriter.h"
#include"RxData/cache/interfaces/ObjectAccess.h"

#include"RxData/cache/observer/CacheListener.h"
#include"RxData/cache/observer/CacheReaderListener.h"
#include"RxData/cache/observer/DataReaderListener.h"
#include"RxData/cache/observer/ObjectListener.h"
#include"RxData/cache/observer/SelectionListener.h"

#include"RxData/cache/object/ObjectCache.h"
#include"RxData/cache/object/ObjectHome.h"
#include"RxData/cache/object/ObjectHomeBase.h"
#include"RxData/cache/object/ObjectId.h"
#include"RxData/cache/object/ObjectReference.h"
#include"RxData/cache/object/ObjectRoot.h"
#include"RxData/cache/object/ObjectState.h"

#include"RxData/cache/policy/Ejection.h"
#include"RxData/cache/policy/Lifespan.h"
#include"RxData/cache/policy/ResourceLimits.h"

#include"RxData/cache/relation/RelationDescription.h"
#include"RxData/cache/relation/RelationKind.h"
#include"RxData/cache/relation/RelationName.h"

#include"RxData/cache/scope/ObjectScope.h"
#include"RxData/cache/scope/ReferenceScope.h"

#include"RxData/cache/selection/FilterCriterion.h"
#include"RxData/cache/selection/MembershipState.h"
#include"RxData/cache/selection/QueryCriterion.h"
#include"RxData/cache/selection/Selection.h"
#include"RxData/cache/selection/SelectionCriteria.h"
#include"RxData/cache/selection/SelectionCriterion.h"

#include"RxData/cache/strategy/LeastRecentlyUsed.h"

#include"RxData/cache/object/interfaces/ObjectCacheReader.h"
#include"RxData/cache/object/interfaces/ObjectCacheWriter.h"
#include"RxData/cache/object/interfaces/ObjectCacheProxy.h"

#include"RxData/cache/object/policy/ObjectHomePolicy.h"
#include"RxData/cache/object/reader/ObjectReaderProxy.h"
#include"RxData/cache/object/reader/ObjectReaderStub.h"
#include"RxData/cache/object/writer/ObjectWriterProxy.h"
#include"RxData/cache/object/access/ObjectAccessProxy.h"
