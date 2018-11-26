/*
 * ChangeForWriter.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include "RTPS/Stateful/ChangeFromWriter.h"

namespace RTPS
{

ChangeFromWriter::ChangeFromWriter()
	: isRelevant_(true)
	, status_(ChangeFromWriterStatusKind::MISSING)
{
}

ChangeFromWriter::ChangeFromWriter(bool isRelevant, ChangeFromWriterStatusKind::Type status)
	: isRelevant_(isRelevant)
	, status_(status)
{
}

ChangeFromWriter::~ChangeFromWriter()
{
}

} // namespace RTPS
