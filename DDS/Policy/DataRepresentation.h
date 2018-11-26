/*
 * DataRepresentation.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_DataRepresentation_h_Included
#define DDS_Policy_DataRepresentation_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

class DLL_STATE DataRepresentation : public QosPolicyBase
{
public:
    DataRepresentation() : QosPolicyBase()
    {}
    virtual ~DataRepresentation()
    {}

    DEFINE_POLICY_TRAITS(DataRepresentation, 23, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *) const
    {

    }

    virtual void Read(NetworkLib::SerializeReader *)
    {

    }

private:
    // TODO
};

}}

#endif
