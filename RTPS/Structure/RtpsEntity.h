/*
 * RtpsEntity.h
 *
 *  Created on: Nov 14, 2011
 *      Author: knuthelv
 */

#ifndef RTPS_Structure_RtpsEntity_h_IsIncluded
#define RTPS_Structure_RtpsEntity_h_IsIncluded

#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE RtpsEntity
{
public:
	RtpsEntity();
	RtpsEntity(const GUID &guid);

	virtual ~RtpsEntity();

public:
	const GUID& GetGUID() 					const 	{ return guid_;}

	void 		SetGUID(const GUID &guid) 			{ guid_ = guid; }

private:
	GUID	guid_;
};

}

#endif
