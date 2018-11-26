/*
 * VirtualMachineReader.h
 *
 *  Created on: Apr 7, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Machine_VirtualMachineReader_h_IsIncluded
#define RTPS_Machine_VirtualMachineReader_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"
#include"RTPS/Export.h"
namespace NetworkLib
{
    class IOReader;
}

namespace RTPS
{

class StatelessReader;
class StatefulReader;
class RtpsReader;

class DLL_STATE VirtualMachineReader : public BaseLib::Thread
{
public:
    VirtualMachineReader(RtpsReader* rtpsReader);
    virtual ~VirtualMachineReader();

    CLASS_TRAITS(VirtualMachineReader)

    bool ReceiveSubmessage(RtpsMessageReceiver::Ptr &messageReceiver, RtpsSubmessageBase::Ptr &submessage);

public:
    RtpsReader* 	GetReader() const { return rtpsReader_; }

private:
    virtual void run();

private:
    void run(StatelessReader* statelessReader);
    void run(StatefulReader* statefulReader);

private:
    RtpsReader  *rtpsReader_;
};

} // namespace RTPS

#endif // RTPS_Machine_VirtualMachineReader_h_IsIncluded
