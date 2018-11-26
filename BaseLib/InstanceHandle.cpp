#include "BaseLib/InstanceHandle.h"

namespace BaseLib
{

InstanceHandle::InstanceHandle(InstanceHandleType handle)
    : handle_(handle)
{

}

InstanceHandle::~InstanceHandle()
{

}


std::ostream &operator<<(std::ostream &ostr, const BaseLib::InstanceHandleSeq &handleSeq)
{
    ostr << "(";
    for(BaseLib::InstanceHandleSeq::const_iterator it = handleSeq.begin(), it_end = handleSeq.end(); it != it_end; ++it)
        ostr << it->GetHandle() << ", ";
    ostr << ")";

    return ostr;
}


std::ostream &operator<<(std::ostream &ostr, const BaseLib::InstanceHandleSet &handleSeq)
{
    ostr << "(";
    for(BaseLib::InstanceHandleSet::const_iterator it = handleSeq.begin(), it_end = handleSeq.end(); it != it_end; ++it)
        ostr << it->GetHandle() << ", ";
    ostr << ")";

    return ostr;
}

} // namespace BaseLib


