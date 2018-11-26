#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessGroup.h"
#include"RxStreamer/DataAccessGroupIterator.h"
#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DLL_STATE DataAccessGroupChainIterator
        : public Templates::IsCurrentMethod
        , public Templates::LockableType<DataAccessGroupChainIterator>
        , public BaseLib::Templates::Iterator<DataAccessGroup>
{
public:
    DataAccessGroupChainIterator(const DataAccessGroupChain &chain);
    virtual ~DataAccessGroupChainIterator();

    virtual bool IsCurrent() const;

    DataAccessGroup Current();
    DataAccessGroupIterator GroupIterator();

    virtual DataAccessGroup Next();
    virtual bool HasNext() const;

    virtual void Remove();
    virtual void Rewind();

private:
    bool hasNext() const;
    int getAndIncrement();

private:
    DataAccessGroupChain    chain_;
    DataAccessGroup         group_;
    DataAccessGroupIterator groupIterator_;
    int                     index_;
};

}
