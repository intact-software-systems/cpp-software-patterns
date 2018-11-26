#ifndef RxData_Cache_SelectionSubject_h_IsIncluded
#define RxData_Cache_SelectionSubject_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/observer/SelectionListener.h"

namespace RxData
{

template <typename DATA>
class SelectionSubject
        : public SelectionObserver<DATA>
        , public BaseLib::Concurrent::Observable<SelectionObserver<DATA> >
{
public:
    SelectionSubject()
        : selectionObserverSignaller_(new Signaller1<void, DATA>())
    {}
    virtual ~SelectionSubject()
    {}

    /**
     * Called when an object enters the Selection.
     *
     * @param data
     */
    virtual void OnObjectIn(DATA data)
    {
        selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectIn);
    }

    /**
     * Called when an object exits the Selection.
     *
     * @param data
     */
    virtual void OnObjectOut(DATA data)
    {
        selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectOut);
    }

    /**
     * Called when the contents of an object belonging to the Selection changes
     *
     * @param data
     */
    virtual void OnObjectModified(DATA data)
    {
        selectionObserverSignaller_->template Signal<SelectionObserver<DATA> >(data, &SelectionObserver<DATA>::OnObjectModified);
    }

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

    virtual void DisconnectAll()
    {
        selectionObserverSignaller_->DisconnectAll();
    }

private:
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
};

}

#endif
