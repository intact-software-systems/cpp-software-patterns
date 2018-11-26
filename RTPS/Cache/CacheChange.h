/*
 * CacheChange.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#ifndef RTPS_Cache_CacheChange_h_IsIncluded
#define RTPS_Cache_CacheChange_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Cache/CacheChangeBase.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Submessages/RtpsData.h"

#include"RTPS/Export.h"
namespace RTPS
{

// TODO: CacheChange may have a list of DATA pointers in case of data fragments
class DLL_STATE CacheChange : public CacheChangeBase
{
public:
	CacheChange();
	CacheChange(ChangeKindId::Type type, const GUID &guid, const SequenceNumber &seq, const RtpsData::Ptr &data, InstanceHandle handle = InstanceHandle::NIL());
	virtual ~CacheChange();

    CLASS_TRAITS(CacheChange)

public:
	virtual bool IsValid() const;

public:
	virtual const ChangeKindId::Type& 	GetChangeKind() const;
	virtual const GUID& 				GetGUID() const;
	virtual const SequenceNumber& 		GetSequenceNumber() const;
	virtual const InstanceHandle&       GetInstanceHandle() const;
	virtual RtpsData::Ptr				GetData() const;

	virtual void 	SetChangeKind(const ChangeKindId::Type&);
	virtual void 	SetGUID(const GUID&);
	virtual void 	SetSequenceNumber(const SequenceNumber&);
	virtual void    SetInstanceHandle(const InstanceHandle &handle);
	virtual void 	SetData(const RtpsData::Ptr &data);

private:
	ChangeKindId::Type 	    changeKind_;
	GUID				    guid_;
	SequenceNumber		    sequenceNumber_;
	RtpsData::Ptr		    data_;
	InstanceHandle          instanceHandle_;
};

} // namespace RTPS

#endif // RTPS_Cache_CacheChange_h_IsIncluded
