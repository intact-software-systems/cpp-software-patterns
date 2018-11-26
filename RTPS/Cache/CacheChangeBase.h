/*
 * CacheChangeBase.h
 *
 *  Created on: Feb 28, 2012
 *      Author: knuthelv
 */

#ifndef Cache_CacheChangeBase_h_IsIncluded
#define Cache_CacheChangeBase_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Submessages/RtpsData.h"

#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE CacheChangeBase
{
public:
	CacheChangeBase();
	virtual ~CacheChangeBase();

    CLASS_TRAITS(CacheChangeBase)

	virtual bool IsValid() const = 0;

	virtual const ChangeKindId::Type& 	GetChangeKind() const = 0;
	virtual const GUID& 				GetGUID() const = 0;
	virtual const SequenceNumber& 		GetSequenceNumber() const = 0;
	virtual const InstanceHandle&       GetInstanceHandle() const = 0;
	virtual RtpsData::Ptr				GetData() const = 0;

	virtual void 	SetChangeKind(const ChangeKindId::Type&) = 0;
	virtual void 	SetGUID(const GUID&) = 0;
	virtual void 	SetSequenceNumber(const SequenceNumber&) = 0;
	virtual void    SetInstanceHandle(const InstanceHandle &) = 0;
	virtual void	SetData(const RtpsData::Ptr&) = 0;
};

} // namespace RTPS

#endif //Cache_CacheChangeBase_IsIncluded
