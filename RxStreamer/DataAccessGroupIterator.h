#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessGroup.h"
#include"RxStreamer/DataAccessGroupIterator.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE DataAccessGroupIterator
        : public Templates::IsCurrentMethod
        , public Templates::IsStartedMethod
        , public Templates::LockableType<DataAccessGroupIterator>
        , public BaseLib::Templates::Iterator<DataAccessBase::Ptr>
{
public:
    DataAccessGroupIterator(const DataAccessGroup &group);
    DataAccessGroupIterator();
    virtual ~DataAccessGroupIterator();

    virtual bool IsCurrent() const;

    DataAccessBase::Ptr Current();

    virtual DataAccessBase::Ptr Next();
    virtual bool HasNext() const;

    virtual void Remove();
    virtual void Rewind();

    virtual bool IsStarted() const;

private:
    bool hasNext() const;
    int getAndIncrement();

private:
    DataAccessGroup         group_;
    DataAccessBase::Ptr     currentAccess_;
    int                     index_;
};

}
