/*
 * ChangeForReader.h
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Stateful_ChangeForReader_h_IsIncluded
#define RTPS_Stateful_ChangeForReader_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/Cache/CacheChange.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE ChangeForReader
{
public:
    ChangeForReader();
    ChangeForReader(bool isRelevant, ChangeForReaderStatusKind::Type status);
    ~ChangeForReader();

    CLASS_TRAITS(ChangeForReader)

public:
    bool 							IsRelevant()		const { return isRelevant_; }
    ChangeForReaderStatusKind::Type	GetStatus() 		const { return status_; }

    void SetRelevant(bool rel)							{ isRelevant_ = rel; }
    void SetStatus(ChangeForReaderStatusKind::Type s)	{ status_ = s; }

private:
    bool							isRelevant_;
    ChangeForReaderStatusKind::Type	status_;
};

} // namespace RTPS

#endif // RTPS_Stateful_ChangeForReader_h_IsIncluded
