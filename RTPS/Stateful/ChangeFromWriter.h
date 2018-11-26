/*
 * ChangeForWriter.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateful_ChangeFromWriter_h_IsIncluded
#define RTPS_Stateful_ChangeFromWriter_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Cache/CacheChange.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE ChangeFromWriter
{
public:
	ChangeFromWriter();
	ChangeFromWriter(bool isRelevant, ChangeFromWriterStatusKind::Type status);
	virtual ~ChangeFromWriter();

public:
	bool 								IsRelevant()		const { return isRelevant_; }
	ChangeFromWriterStatusKind::Type	GetStatus() 		const { return status_; }

	void SetRelevant(bool rel)							{ isRelevant_ = rel; }
	void SetStatus(ChangeFromWriterStatusKind::Type s)	{ status_ = s; }

private:
	bool								isRelevant_;
	ChangeFromWriterStatusKind::Type	status_;
};

}

#endif
