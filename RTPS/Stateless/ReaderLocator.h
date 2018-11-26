/*
 * ReaderLocator.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateless_ReaderLocator_h_IsIncluded
#define RTPS_Stateless_ReaderLocator_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Cache/IncludeLibs.h"
#include"RTPS/Stateful/ChangeForReader.h"
#include"RTPS/Structure/RtpsReaderProxyBase.h"
#include"RTPS/Export.h"
namespace RTPS
{

/* --------------------------------------------------
    ReaderLocator is created from StatelessWriter
    - GuardCondition should be initialized from StatelessWriter
      with Functor=StatelessWriter, ARG=DataCache
    - StatelessWriter attaches GuardCondition to its WaitSet
    - The operator function call is dispatched from StatelessWriter
----------------------------------------------------*/
class DLL_STATE ReaderLocator : public RtpsReaderProxyBase
{
public:
    typedef std::map<SequenceNumber, CacheChange::Ptr>	MapCacheChange;

public:
    ReaderLocator(const Locator &locator, bool expectsInlineQos = false);
    ReaderLocator(bool expectsInlineQos = false);
    virtual ~ReaderLocator();

    CLASS_TRAITS(ReaderLocator)

public:
    // ----------------------------------------------------
    //  From RTPS implementation specs.
    // ----------------------------------------------------
    /*ChangeForReader NextRequestedChange();
    ChangeForReader NextUnsentChange();
    MapCacheChange 	RequestedChanges();
    MapCacheChange 	UnsentChanges();
*/
public:
    // ----------------------------------------------------
    //    Getter and Setter
    // ----------------------------------------------------
    bool	ExpectsInlineQos()			    const 	{ return expectsInlineQos_; }
    void 	SetExpectsInlineQos(bool e)			    { expectsInlineQos_ = e; }

    const LocatorList&	GetLocatorList()	const   { return locatorList_; }
    const Locator&		GetLocator()		const   { return locator_; }

private:
    MapCacheChange	requestedChanges_;
    MapCacheChange	unsentChanges_;
    LocatorList		locatorList_;
    Locator			locator_;
    bool 			expectsInlineQos_;
};

} // namespace RTPS

#endif // RTPS_Stateless_ReaderLocator_h_IsIncluded
