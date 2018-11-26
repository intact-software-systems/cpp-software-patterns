#ifndef RxData_Cache_ObjectHome_h_IsIncluded
#define RxData_Cache_ObjectHome_h_IsIncluded

#include"RxData/IncludeExtLibs.h"

#include"RxData/cache/object/ObjectState.h"
#include"RxData/cache/object/ObjectHomeBase.h"
#include"RxData/cache/object/ObjectReference.h"
#include"RxData/cache/object/policy/ObjectHomePolicy.h"

#include"RxData/cache/CacheBase.h"
#include"RxData/cache/AccessObjectCache.h"
#include"RxData/cache/CacheUsage.h"

#include"RxData/cache/observer/DataReaderListener.h"
#include"RxData/cache/observer/ObjectListener.h"
#include"RxData/cache/interfaces/ObjectLoader.h"
#include"RxData/cache/selection/Selection.h"
#include"RxData/cache/selection/SelectionCriterion.h"

#include"RxData/Export.h"
namespace RxData
{

/**
 * For each application-defined class, there is an ObjectHome instance, which exists to globally represent the
 * related set of instances and to perform actions on it. Actually, ObjectHome is the root class for generated
 * classes (each one being dedicated to one application-defined class, so that it embeds the related specificity).
 * The name for such a derived class is FooHome, assuming it corresponds to the application-defined class Foo.
 *
 * A derived ObjectHome (e.g., a FooHome) has no factory. It is created as an object directly by the natural means
 * in each language binding (e.g., using "new" in C++ or Java).
 *
 * @author KVik
 *
 */
template <typename DATA, typename KEY>
class ObjectHome : public ObjectHomeBase
                 , public Observable< ObjectObserver<DATA> >
                 , public DataReaderListener<DATA, KEY>
                 , public ENABLE_SHARED_FROM_THIS_T2(ObjectHome, DATA, KEY)
{
private:
    // ----------------------------------
    // Typedefs
    // ----------------------------------

    /**
     * @brief ListSelection
     */
    typedef IList<SelectionBase::Ptr > ListSelection;

    /**
     * @brief ListObjectLoader
     */
    typedef IList<typename ObjectLoader<DATA, KEY>::Ptr> ListObjectLoader;

    // ----------------------------------
    // Attributes
    // ----------------------------------

    /**
     * The public name of the application-defined class (class_name).
     */
    std::string typeName_;

    /**
     * The object home policy regarding object lifespan, resource-limits, etc.
     */
    ObjectHomePolicy policy_;

    /**
     * A content filter (content_filter) that is used to filter incoming objects. It only concerns
     * subscribing applications; only the incoming objects that pass the content filter will be created
     * in the Cache and by that ObjectHome. This content filter is given by means of a string and is
     * intended to be mapped on the underlying DCPS infrastructure to provide content-based subscription
     * at DLRL level (see Annex B for its syntax). The content_filter attribute is set to NULL by default.
     */
    std::string contentFilter_;

    /**
     * A bool that indicates whether the state of a DLRL Object should always be loaded into that Object
     * (auto_deref = TRUE) or whether this state will only be loaded after it has been accessed explicitly
     * by the application (auto_deref = FALSE). The auto_deref attribute is set to TRUE by default.
     */
    bool autoDeref_;

    /**
     * The list of attached Selection (selections).
     */
    ListSelection selections_;

    /**
     * The signaller to the attached ObjectObserver
     */
    typename Signaller1<bool, DATA>::Ptr objectObserverSignaller_;

    /**
     * The ObjectLoader is the data-source for the ObjectHome
     */
    ListObjectLoader dataSources_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * @param className
     */
    ObjectHome(std::string typeName, ObjectHomePolicy policy)
        : typeName_(typeName)
        , policy_(policy)
        , autoDeref_(true)
        , objectObserverSignaller_(new Signaller1<bool, DATA>())
    { }

    CLASS_TRAITS(ObjectHome)

    /**
     * @brief GetPtr
     * @return
     */
    typename ObjectHome<DATA, KEY>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    // ----------------------------------
    // Operations
    // ----------------------------------

    /**
     * Ask to load the most recent state of a DLRL Object into that Object for all objects
     * managed by that home.
     */
    //public void derefAll()
    //{
        // TODO: Update selections.
    //}

    /**
     * Ask to unload all object states from objects that are attached to this home (underef_all)
     */
    //public void underefAll()
    //{
        // TODO: Clear selections
    //}

    /**
     * Refresh CacheBase with respect to this ObjectHome's dataSources
     */
    virtual void refresh(CacheInterface *source)
    {
        if(dataSources_.empty()) return;

        CacheBase *cacheBase = dynamic_cast<CacheBase*>(source);

        // -- debug --
        ASSERT(cacheBase);
        // -- debug --

        // -------------------------------------------------
        // Iterate DataSources and get new data and update ObjectCache objects
        // -------------------------------------------------
        MutexTypeLocker<ListObjectLoader> lock(&dataSources_);

        for(typename ListObjectLoader::iterator it = dataSources_.begin(), it_end = dataSources_.end(); it != it_end; ++it)
        {
            typename ObjectLoader<DATA, KEY>::Ptr dataSource = *it;

            typename std::map<KEY, DATA> mapOfData = dataSource->loadAll();

            if(!mapOfData.empty())
            {
                ObjectCache::Ptr objectCache = cacheBase->getOrCreateObjectCache(typeName_);
                objectCache->write<DATA, KEY>(mapOfData, this->GetPtr());
            }
        }
    }

    // ----------------------------------
    // Callbacks
    // ----------------------------------

    /**
     * Call-back from data-producer (example: Subscriber). Triggers the CacheListeners
     * attached to the Cache.
     */
    virtual bool OnDataAvailable(const DATA &data, KEY key, CacheBase::Ptr source)
    {
        // -------------------------------------------------
        // Notifies CacheListeners
        // -------------------------------------------------
        source->notifyOnBeginUpdates();

        // ------------------------------------------------------
        // Update the object-cache
        // ------------------------------------------------------
        ObjectCache::Ptr objectCache = source->getOrCreateObjectCache(typeName_);

        // -- debug --
        ASSERT(objectCache);
        // -- debug --

        bool isAdded = onDataAvailablePrivate(data, key, objectCache);

        // -------------------------------------------------
        // Notifies CacheListeners
        // -------------------------------------------------
        source->notifyOnEndUpdates();

        return isAdded;
    }

    /**
     * Call-back from main CacheBase (either Cache or CacheAccess). Does not trigger any CacheListeners
     * attached to the Cache.
     */
    bool onDataAvailable(const DATA &data, KEY key, ObjectCache::Ptr objectCache, bool doNotifyListeners = true)
    {
        return onDataAvailablePrivate(data, key, objectCache, doNotifyListeners);
    }

private:
    /**
     * Private function that inserts new data to cache.
     */
    bool onDataAvailablePrivate(const DATA &data, KEY key, ObjectCache::Ptr objectCache, bool doNotifyListeners = true)
    {
        // ------------------------------------------------------
        // Update the object-cache - thread-safe
        // ------------------------------------------------------
        typename Impl::ObjectReference<DATA>::Ptr objRef = objectCache->write<DATA, KEY>(data, key, this->GetPtr());

        // ------------------------------------------------------
        // Q: How, when and why can this happen?
        // A: data=null, key=not-in-cache, then write returns null.
        // ------------------------------------------------------
        if(!objRef)
        {
            // -- debug --
            //log.debug("Could not write data=" + data + " (null) and key not found! Returning false.");
            // -- debug --

            return false;
        }

        // -------------------------------------------------------
        // Call ObjectListeners - thread-safe
        // -------------------------------------------------------
        if(doNotifyListeners)
        {
            notifyObjectListeners(objRef->get(), objRef->getReadState());

            // ------------------------------------------------------
            // Update selections if auto-refresh is TRUE
            //  - Each selection uses its given selection criterion
            //  - thread-safe
            // ------------------------------------------------------
            updateSelections(objRef);
        }

        return true;
    }

public:
    // ----------------------------------
    // DataSource management for this ObjectHome
    // ----------------------------------

    /**
     * Attach a DataSource for the ObjectHome.
     *
     * @param dataSource
     */
    void attachDataSource(typename ObjectLoader<DATA, KEY>::Ptr dataSource)
    {
        MutexTypeLocker<ListObjectLoader> lock(&dataSources_);
        dataSources_.push_back(dataSource);
    }

    /**
     * Detach the ObjectLoader from the ObjectHome.
     *
     * @param dataSource
     */
    void detachDataSource(typename ObjectLoader<DATA,KEY>::Ptr dataSource)
    {
        MutexTypeLocker<ListObjectLoader> lock(&dataSources_);
        dataSources_.remove(dataSource);
    }

    // ----------------------------------
    // Observer management
    // ----------------------------------

    /**
     * Attach an ObjectListener. When a listener is attached, a bool parameter specifies,
     * when set to TRUE, that the listener should listen also for the modification of the
     * contained objects (concerns_contained_objects).
     *
     * @param listener
     */
    virtual SlotHolder::Ptr Connect(ObjectObserver<DATA> *observer)
    {
        objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectCreated);
        objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectDeleted);
        return objectObserverSignaller_->template Connect<ObjectObserver<DATA> >(observer, &ObjectObserver<DATA>::OnObjectModified);
    }

    /**
     * Detach the ObjectListener from the ObjectHome.
     *
     * @param listener
     */
    virtual bool Disconnect(ObjectObserver<DATA> *observer)
    {
        return objectObserverSignaller_->template Disconnect< ObjectObserver<DATA> >(observer);
    }

    /**
     * @brief DisconnectAll
     */
    virtual void DisconnectAll()
    {
        return objectObserverSignaller_->DisconnectAll();
    }

private:
    /**
     * Call upon on ObjectListeners attached to this object.
     *
     * TODO: A listener can return false, but currently it is ignored.
     */
    void notifyObjectListeners(const DATA &data, ObjectState::Type state)
    {
        switch(state)
        {
            case ObjectState::OBJECT_DELETED:
            {
                // ------------------------------------------
                // TODO: FIXME: Currently this data is null!!
                // ------------------------------------------
                objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectDeleted);
                break;
            }
            case ObjectState::OBJECT_MODIFIED:
            {
                objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectModified);
                break;
            }
            case ObjectState::OBJECT_NEW:
            {
                objectObserverSignaller_->template Signal<ObjectObserver<DATA> >(data, &ObjectObserver<DATA>::OnObjectCreated);
                break;
            }
            case ObjectState::OBJECT_NOT_MODIFIED:
            case ObjectState::OBJECT_VOID:
            {
                break;
            }
            default:
                break;
        }
    }

    /**
     * Updating attached selections.
     */
    void updateSelections(typename Impl::ObjectReference<DATA>::Ptr objRef)
    {
        if(selections_.empty()) return;

        // ------------------------------------------------------
        // Update selections if auto-refresh is TRUE
        //  - Each selection uses its given selection criterion
        // ------------------------------------------------------
        MutexTypeLocker<ListSelection> lock(&selections_);

        for(ListSelection::iterator it = selections_.begin(), it_end = selections_.end(); it != it_end; ++it)
        {
            SelectionBase::Ptr selection = *it;

            selection->filter(objRef);
        }
    }

public:
    // ----------------------------------
    // Selection management
    // ----------------------------------

    /**
     * Create a Selection. The criterion parameter specifies the SelectionCriterion (either a
     * FilterCriterion or a SelectionCriterion) to be attached to the Selection, the auto_refresh
     * parameter specifies if the Selection has to be refreshed automatically or only on demand
     * (see Selection) and a bool parameter specifies, when set to TRUE, that the Selection is
     * concerned not only by its member objects but also by their contained ones (concerns_contained_objects);
     * attached SelectionCriterion belong to the Selection that itself belongs to its creating
     * ObjectHome. When creating a Selection while the DCPS State of the Cache is still set to INITIAL,
     * a PreconditionNotMet is raised.
     */
    typename Selection<KEY, DATA>::Ptr createSelection(typename SelectionCriterion::Ptr criterion, bool autoRefresh, bool concernsContainedObjects)
    {
        typename Selection<KEY, DATA>::Ptr selection(new Selection<KEY, DATA>(criterion, autoRefresh, concernsContainedObjects));

        {
            MutexTypeLocker<ListSelection> lock(&selections_);
            selections_.push_back(selection);
        }

        return selection;
    }

    /**
     * Delete a Selection. This operation deletes the Selection and its attached SelectionCriterion.
     * If the Selection was not created by this ObjectHome, FALSE is returned.
     */
    void deleteSelection(typename Selection<KEY, DATA>::Ptr selection)
    {
        MutexTypeLocker<ListSelection> lock(&selections_);
        selections_.remove(selection);
    }

    /**
     * Create a new cache object (createObject). This operation takes as parameter the CacheAccess
     * concerned by the creation. The following preconditions must be met: the Cache must be set to
     * the DCPS State of ENABLED, and the supplied CacheAccess must be writeable. Not satisfying either
     * precondition will raise a PreconditionNotMet.
     */
    typename ObjectReference<DATA>::Ptr createObject(const DATA &data, KEY key, AccessObjectCache::Ptr access)
    {
        // -- start pre-checks --
        if(access->getCacheUsage() == CacheUsage::READ_ONLY)
        {
            return ObjectReference<DATA>::Ptr();
        }
        // -- end pre-checks --

        ObjectCache::Ptr objectCache = access->getOrCreateObjectCache(typeName_);
        typename ObjectReference<DATA>::Ptr objRef = objectCache->write<DATA, KEY>(data, key, this);

        // -- debug --
        ASSERT(objRef);
        // -- debug --

        return objRef;
    }

    /**
     * Check if object with key is in cache
     */
    bool containsObject(KEY key, CacheBase::Ptr source) const
    {
        ObjectCache::Ptr objectCache = source->getOrCreateObjectCache(typeName_);
        return objectCache->containsKey<KEY>(key);
    }

    /**
     * Retrieve a cache object based on its object-id in the specified CacheBase (find_object).
     */
    DATA findObject(KEY key, CacheBase::Ptr source)
    {
        ObjectCache::Ptr objectCache = source->getOrCreateObjectCache(typeName_);
        return objectCache->read<DATA, KEY>(key);
    }

    /**
     * Obtain from a CacheBase a (typed) list of all objects that match the type of the selected
     * ObjectHome (get_objects). For example the type ObjectRoot[ ] will be substituted by a
     * type Foo[ ] in a FooHome.
     */
    std::map<KEY, DATA> getObjects(CacheBase::Ptr source)
    {
        ObjectCache::Ptr objectCache = source->getOrCreateObjectCache(typeName_);
        return objectCache->readAll<DATA, KEY>();
    }

    /**
     * obtain from a CacheBase a (typed) list of all objects that match the type of the selected
     * ObjectHome and that have been created, modified, or deleted during the last refresh operation
     * (get_created_objects, get_modified_objects, and get_deleted_objects respectively). The type
     * ObjectRoot[ ] will be substituted by a type Foo[ ] in a FooHome.
     */
    std::list<DATA> getCreatedObjects(CacheBase::Ptr source)
    {
        return getObjectsWithReadState(source, ObjectState::OBJECT_NEW);
    }

    /**
     * obtain from a CacheBase a (typed) list of all objects that match the type of the selected
     * ObjectHome and that have been created, modified, or deleted during the last refresh operation
     * (get_created_objects, get_modified_objects, and get_deleted_objects respectively). The type
     * ObjectRoot[ ] will be substituted by a type Foo[ ] in a FooHome.
     */
    std::list<DATA> getModifiedObjects(CacheBase::Ptr source)
    {
        return getObjectsWithReadState(source, ObjectState::OBJECT_MODIFIED);
    }

    /**
     * obtain from a CacheBase a (typed) list of all objects that match the type of the selected
     * ObjectHome and that have been created, modified, or deleted during the last refresh operation
     * (get_created_objects, get_modified_objects, and get_deleted_objects respectively). The type
     * ObjectRoot[ ] will be substituted by a type Foo[ ] in a FooHome.
     */
    std::list<DATA> getDeletedObjects(CacheBase::Ptr source)
    {
        return getObjectsWithReadState(source, ObjectState::OBJECT_DELETED);
    }

private:
    /**
     * Return a list of object references with objectState.
     */
    std::list<DATA> getObjectsWithReadState(CacheBase::Ptr source, ObjectState::Type objectState)
    {
        ObjectCache::Ptr objectCache = source->getOrCreateObjectCache(typeName_);

        return objectCache->readWithReadState<DATA>(objectState);
    }

public:
    // ----------------------------------
    // Getters
    // ----------------------------------

    virtual std::string getTypeName() const
    {
        return typeName_;
    }

    virtual std::string getContentFilter() const
    {
        return contentFilter_;
    }

    virtual bool isAutoDeref() const
    {
        return autoDeref_;
    }

    virtual ObjectHomePolicy getPolicy() const
    {
        return policy_;
    }

    // ----------------------------------
    // Setters
    // ----------------------------------

    void setContentFilter(std::string contentFilter)
    {
        this->contentFilter_ = contentFilter;
    }

    void setAutoDeref(bool autoDeref)
    {
        this->autoDeref_ = autoDeref;
    }
};

}

#endif

