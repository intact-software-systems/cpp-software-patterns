/*
 * ParameterList.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_ParameterList_h_IsIncluded
#define RTPS_Elements_ParameterList_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/Parameter.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
    Mapping of the type ParameterList

The ParameterList may contain multiple Parameters with the same value for the parameterId.
This is used to provide a collection of values for that kind of Parameter.
The use of ParameterList encapsulation makes it possible to extend the protocol and introduce
new parameters and still be able to preserve interoperability with earlier versions of
the protocol.

#define PID_SENTINEL (1)

The PID_SENTINEL is used to terminate the parameter list and its length is ignored. The PID_PAD is used to enforce
alignment of the parameter that follows and its length can be anything (as long as it is a multiple of 4).
The complete set of possible values for the parameterId in version 2.1 of the protocol appears in Section 9.6.3.

ParameterList is used as part of several messages to encapsulate QoS parameters that may affect the interpretation of
the message. The encapsulation of the parameters follows a mechanism that allows extensions to the QoS without
breaking backwards compatibility.
---------------------------------------------------------------------------------------------*/
typedef std::vector<Parameter>	ParameterVector;

class DLL_STATE ParameterList : public NetworkLib::NetObjectBase
{
public:
    ParameterList(const ParameterVector	&parameterVector)
        : parameterVector_(parameterVector)
        {}
    ParameterList()
        : parameterVector_()
        {}
    virtual ~ParameterList()
    {}

    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        for(ParameterVector::const_iterator it = parameterVector_.begin(), it_end = parameterVector_.end(); it != it_end; ++it)
        {
            const Parameter &parameter = *it;
            parameter.Write(writer);
        }

        Parameter padding = Parameter::SENTINEL();
        padding.Write(writer);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        int i = 0;
        for(i = 0; i <= 1000; i++)
        {
            Parameter parameter;
            parameter.Read(reader);

            if(parameter.GetParameterId() != ParameterId::SENTINEL())
            {
                parameterVector_.push_back(parameter);
            }
            else
            {
                break;
            }
        }

        // -- fail safe --
        if(i >= 1000)
            IFATAL() << "ParamterList reading failed! PID_SENTINEL not identified!";
        // -- end fail safe --
    }

public:
    bool IsEmpty()	const { return parameterVector_.empty(); }

public:
    const ParameterVector&	GetParameterList()	const { return parameterVector_; }

    void SetParameterList(const ParameterVector &vec)	{ parameterVector_ = vec; }

    unsigned short GetSize() const
    {
        unsigned short size = 0;
        for(ParameterVector::const_iterator it = parameterVector_.begin(), it_end = parameterVector_.end(); it != it_end; ++it)
        {
            const Parameter &parameter = *it;
            size = size + parameter.GetSize();
        }

        size = size + Parameter::SENTINEL().GetSize();

        return size;
    }

private:
    ParameterVector	parameterVector_;
};

} // namespace RTPS

#endif // RTPS_Elements_ParameterList_h_IsIncluded
