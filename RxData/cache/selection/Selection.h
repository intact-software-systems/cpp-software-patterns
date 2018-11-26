#ifndef RxData_Cache_Selection_h_IsIncluded
#define RxData_Cache_Selection_h_IsIncluded

#include"RxData/CommonDefines.h"

#include"RxData/cache/selection/SelectionBase.h"
#include"RxData/cache/selection/FilterCriterion.h"
#include"RxData/cache/selection/MembershipState.h"
#include"RxData/cache/selection/QueryCriterion.h"

#include"RxData/cache/observer/SelectionListener.h"

#include"RxData/cache/object/ObjectReference.h"
#include"RxData/cache/object/ObjectState.h"

#include"RxData/cache/object/interfaces/ObjectCacheReader.h"
#include"RxData/Export.h"
namespace RxData
{

/**
 * Class whose instances act as representatives of a given subset of objects. The subset is defined
 * by an expression attached to the selection.
 *
 * A Selection is a means to designate a subset of the instances of a given ObjectHome, fulfilling
 * a given criterion. This criterion is given by means of the attached SelectionCriterion.
 *
 * TODO: To restrict access to this.filter(objRef), create an interface Selection <DATA> without that function.
 *
 * @author KVik
 *
 * @param <DATA>
 */
template <typename KEY, typename DATA>
class Selection : public SelectionBase
                , public Observable< SelectionObserver<DATA> >
                , public ENABLE_SHARED_FROM_THIS_T2(Selection, KEY, DATA)
{
private:
    // ----------------------------------
    // Typedefs
    // ----------------------------------

    typedef IMap<KEY, DATA> MapData;

    // ----------------------------------
    // Attributes
    // ----------------------------------

    /**
     * The SelectionListener is activated when the composition of the Selection is modified as well
     * as when one of its members is modified. A member can be considered as modified, either
     * when the member is modified or when that member or one of its contained objects is modified
     * (depending on the value of concerns_contained). Modifications in the Selection are considered
     * with respect to the state of the Selection last time is was examined, for instance:
     *
     * - at each incoming updates processing, if auto_refresh is TRUE.
     * - at each explicit call to refresh, if auto_refresh is FALSE.
     */
    typename Signaller1<void, DATA>::Ptr selectionObserverSignaller_;

    /**
     * The corresponding SelectionCriterion (criterion). It is given at Selection creation time
     * (see ObjectHome::create_selection).
     */
    SelectionCriterion::Ptr selectionCriterion_;

    /**
     * A bool auto_refresh that indicates if the Selection has to be refreshed at each incoming
     * modification (TRUE) or only on demand (FALSE). It is given at Selection creation time
     * (see ObjectHome::create_selection).
     */
    bool autoRefresh_;

    /**
     * A bool concerns_contained that indicates whether the Selection considers the modification
     * of one of its members based on its content only (FALSE) or based on its content or the content
     * of its contained objects (TRUE). It is given at Selection creation time
     * (see ObjectHome::create_selection).
     */
    bool concernsContained_;

    /**
     * The list of the objects that are part of the selection (members).
     *
     * Proposal: Use ObjectCache<DATA> as input to DataList to provide thread-safe access to the Cache,
     * this way we can get addedElements, removedElements, modifiedElements very easy!
     *
     * Comment: Actually, this does not work so well with a selection. It is better for the
     * Selection to search through the ObjectCache<DATA> and add the necessary references to its
     * own DataList.
     */
    MapData members_;

public:
    // ----------------------------------
    // Constructor
    // ----------------------------------

    /**
     * @param selectionCriterion
     * @param autoRefresh
     * @param concernsContained
     * @param members
     */
    Selection(SelectionCriterion::Ptr selectionCriterion, bool autoRefresh, bool concernsContained)
        : selectionObserverSignaller_(new Signaller1<void, DATA>())
        , selectionCriterion_(selectionCriterion)
        , autoRefresh_(autoRefresh)
        , concernsContained_(concernsContained)
        , members_()
    { }

    virtual ~Selection()
    { }

    CLASS_TRAITS(Selection)

    /**
     * @brief GetPtr
     * @return
     */
    typename Selection<KEY, DATA>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    // ----------------------------------
    // Operations
    // ----------------------------------

    /**
     * Set the SelectionListener (set_listener), that will be triggered when the composition of the
     * selection changes, as well as if the members are modified. set_listener returns the
     * previously set listener if any; set_listener called with a NULL parameter discards the current
     * listener.
     */
    virtual SlotHolder::Ptr Connect(SelectionObserver<DATA> *observer)
    {
        selectionObserverSignaller_->template Connect<SelectionObserver<DATA> >(observer, &SelectionObserver<DATA>::OnObjectIn);
        selectionObserverSignaller_->template Connect<SelectionObserver<DATA> >(observer, &SelectionObserver<DATA>::OnObjectOut);
        return selectionObserverSignaller_->template Connect<SelectionObserver<DATA> >(observer, &SelectionObserver<DATA>::OnObjectModified);
    }

    virtual bool Disconnect(SelectionObserver<DATA> *observer)
    {
        return selectionObserverSignaller_->template Disconnect<SelectionObserver<DATA> >(observer);
    }

    /**
     * Request that the Selection updates its members (refresh).
     *
     * If auto_refresh is TRUE then the function does nothing because it is automatically updated
     * by the Cache it is attached to.
     */
    virtual void refresh(typename ObjectCacheReader::Ptr objectCacheReader)
    {
        if(isAutoRefresh() == true) return;

        // -----------------------------------------------
        // Search and apply filter to all objects in cache
        // -----------------------------------------------
        std::map<KEY, DATA> dataMap = objectCacheReader->readAll<DATA, KEY>();

        for(typename std::map<KEY, DATA>::iterator it = dataMap.begin(), it_end = dataMap.end(); it != it_end; ++it)
        {
            // -- debug --
            //ASSERT(it->second);
            // -- debug --

            applyFilterToData(it->second, it->first);
        }
    }

    /**
     * Called from ObjectHome and applies the attached filter to the object.
     *
     * If auto_refresh is FALSE then the function does nothing because it should not be updated
     * by the Cache it is attached to.
     *
     * @param objRef
     * @return
     */
    virtual bool filter(typename ObjectRoot::Ptr obj)
    {
        if(isAutoRefresh() == false) return false;

        typename Impl::ObjectReference<DATA>::Ptr objRef = std::dynamic_pointer_cast<Impl::ObjectReference<DATA>>(obj);

        if(!objRef)
        {
            IWARNING() << "Failed to cast object root to data of type " << TYPE_NAME(DATA);
            ASSERT(objRef);
            return false;
        }

        return applyFilterToData(objRef->get(), objRef-> template getObjectKey<KEY>());
    }

private:
    /**
     * Applies filter and notifies selection listeners regarding membership changes.
     *
     * @param newData
     * @param key
     * @return
     */
    bool applyFilterToData(DATA newData, KEY key)
    {
        // -- debug --
        //ASSERT(newData);
        //ASSERT(key);
        // -- debug --

        // -----------------------------------------------
        // Apply one of two possible filter types
        // -----------------------------------------------
        if(selectionCriterion_->getKind() == SelectionCriteria::FILTER_CRITERION)
        {
            return applyFilterCriterion(newData, key);
        }
        else if(selectionCriterion_->getKind() == SelectionCriteria::FILTER_CRITERION)
        {
            return applyQueryCriterion(newData, key);
        }

        // -----------------------------------------------
        // This shouldn't happen!
        // -----------------------------------------------

        return false;
    }


    /**
     * Use filter criterion to check if the data should be in the selection or not.
     *
     * @param newData
     * @param key
     */
    bool applyFilterCriterion(DATA newData, KEY key)
    {
        typename FilterCriterion<DATA>::Ptr filterCriterion = std::dynamic_pointer_cast<FilterCriterion<DATA>>(selectionCriterion_);

        // -- debug --
        ASSERT(filterCriterion);
        // -- debug --

        // -----------------------------------------------
        // Get current membership state
        // -----------------------------------------------
        MembershipState::Type currentMembershipState = getMembershipState(key);

        // -----------------------------------------------
        // Run filter criterion - then add or remove from selection
        // -----------------------------------------------
        bool addToSelection = filterCriterion->CheckObject(newData, currentMembershipState);

        if(addToSelection)
        {
            bool wasInSelection = members_.containsKey(key);
            DATA prevData = addObjectToSelection(key, newData);

            // -----------------------------------------------
            // Notify listeners accordingly
            // -----------------------------------------------
            if(!wasInSelection) // incoming object is new to this selection
            {
                notifyListeners(ObjectState::OBJECT_NEW, newData);
            }
            else if(!(newData == prevData)) // object has changed
            {
                notifyListeners(ObjectState::OBJECT_MODIFIED, newData);
            }
            else // object has not changed
            {
                notifyListeners(ObjectState::OBJECT_NOT_MODIFIED, newData);
            }

            return true;
        }
        else // remove from selection
        {
            bool prevData = removeObjectFromSelection(key);

            // -----------------------------------------------
            // If object reference is removed then notify listeners
            // -----------------------------------------------
            if(prevData)
            {
                notifyListeners(ObjectState::OBJECT_DELETED, newData);
            }

            return false;
        }
    }

    /**
     * Use query criterion to check if the data should be in the selection or not.
     *
     * @param <KEY>
     * @param newData
     * @param key
     * @return
     */
    bool applyQueryCriterion(DATA , KEY )
    {
        typename QueryCriterion<DATA>::Ptr queryCriterion = std::dynamic_pointer_cast<QueryCriterion<DATA>>(selectionCriterion_);

        // -- debug --
        ASSERT(queryCriterion);
        // -- debug --

        // -----------------------------------------------
        // TODO: Apply filter and check data
        // -----------------------------------------------


        return false;
    }

    /**
     * Add object to this selection.
     *
     * @param key
     * @param data
     * @return
     */
    DATA addObjectToSelection(KEY key, DATA data)
    {
        DATA prevData = members_.get(key);
        members_.put(key, data);
        return prevData;
    }

    /**
     * Remove object with key from selection.
     *
     * @param objectId
     * @return removed object
     */
    bool removeObjectFromSelection(KEY key)
    {
        return members_.erase(key) == 1;
    }

    /**
     * Get the selection's membership state of object with id = objectId.
     *
     * @param objectId
     * @return
     */
    MembershipState::Type getMembershipState(KEY key) const
    {
        MembershipState::Type currentMembershipState = MembershipState::UNDEFINED_MEMBERSHIP;

        if(members_.containsKey(key))
            currentMembershipState = MembershipState::ALREADY_MEMBER;
        else
            currentMembershipState = MembershipState::NOT_MEMBER;

        return currentMembershipState;
    }

    /**
     * Notify the attached listener about the object-state change.
     *
     * @param state
     * @param data
     */
    void notifyListeners(ObjectState::Type state, DATA data)
    {
        switch(state)
        {
            case ObjectState::OBJECT_DELETED:
            {
                selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectOut);
                break;
            }
            case ObjectState::OBJECT_MODIFIED:
            {
                selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectModified);
                break;
            }
            case ObjectState::OBJECT_NEW:
            {
                selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectIn);
                break;
            }
            // TODO: Not modified should not call on-object modified?
            case ObjectState::OBJECT_NOT_MODIFIED:
            case ObjectState::OBJECT_VOID:
            {
                break;
            }
            default:
                break;
        }
    }

public:

    // ----------------------------------
    // Getters
    // ----------------------------------

    virtual SelectionCriterion::Ptr getSelectionCriterion() const
    {
        return selectionCriterion_;
    }

    virtual bool isAutoRefresh() const
    {
        return autoRefresh_;
    }

    virtual bool isConcernsContained() const
    {
        return concernsContained_;
    }

    std::map<KEY, DATA> getMembers() const
    {
        return members_;
    }
};

}

#endif

