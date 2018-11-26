#include "BaseLib/Status/AccessCount.h"

namespace BaseLib { namespace Status
{

AccessCount::AccessCount()
    : readCount_(0)
    , modifiedCount_(0)
{ }

AccessCount::~AccessCount()
{ }

void AccessCount::Read()
{
    ++readCount_;
}

void AccessCount::NotModify()
{
    // TODO: Currently not counting not modified
}

void AccessCount::Modify()
{
    ++modifiedCount_;
}

const Count& AccessCount::NumRead() const
{
    return readCount_;
}

const Count& AccessCount::NumModified() const
{
    return modifiedCount_;
}

Count AccessCount::NumAccessed() const
{
    return readCount_.Number() + modifiedCount_.Number();
}

}}
