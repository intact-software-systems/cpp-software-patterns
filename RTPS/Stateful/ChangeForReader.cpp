/*
 * ChangeForReader.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include "RTPS/Stateful/ChangeForReader.h"

namespace RTPS
{

ChangeForReader::ChangeForReader()
	: isRelevant_(false)
	, status_(ChangeForReaderStatusKind::UNSENT)
{
}

ChangeForReader::ChangeForReader(bool isRelevant, ChangeForReaderStatusKind::Type status)
	: isRelevant_(isRelevant)
	, status_(status)
{
}

ChangeForReader::~ChangeForReader()
{
}

} // namespace RTPS
