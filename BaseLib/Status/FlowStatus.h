#ifndef BaseLib_Status_FlowStatus_h_Included
#define BaseLib_Status_FlowStatus_h_Included

#include"BaseLib/Templates/MethodInterfaces.h"
#include"BaseLib/Status/Fraction.h"
#include"BaseLib/Status/EventStatusMethods.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Status
{

/**
 * TODO: Use this in collections
 *
 * - Throughput: packets/second
 * - Packet Delivery Ratio: (packets received)/(packets sent)
 * - Latency: (mean latency)/packet
 * - Protocol Overhead: (protocol packets)/(packets received) and (protocol packets sent)/(packets received)
 * - Route: (Average number of packets)/route
 *
 *  Check QosParse.py from master thesis.
 *
 * TODO: Organize in a map<Kind,Templates::StateStatus<Kind>> stateStatus_;
 */
class DLL_STATE FlowStatus
        : public Templates::LockableType<FlowStatus>
{
public:
    FlowStatus(Duration windowDuration = Duration::FromMinutes(1));
    virtual ~FlowStatus();

    enum class DLL_STATE Kind
    {
        NO,
        ENQUEUED,
        DEQUEUED,
        REJECTED,
        FILTERED
    };

    void Enqueue(int num = 1);
    void Dequeue(int num = 1);
    void Reject(int num = 1);
    void Filter(int num = 1);

    Count QueueSize() const;

    StateStatusTimestamped<Kind> Enqueued() const;
    StateStatusTimestamped<Kind> Dequeued() const;
    StateStatusTimestamped<Kind> Rejected() const;
    StateStatusTimestamped<Kind> Filtered() const;

    Fraction DeliveryRatio() const;

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream &operator<<(std::ostream &ostr, const FlowStatus &status)
    {
        ostr << "["
                << status.enqueued_
                << ", "
                << status.dequeued_
                << ", "
                << status.rejected_
                << ","
                << status.filtered_
                << "]";

        return ostr;
    }


private:
    Count queueSize_;

    // TODO: Organize in a map<Kind,Templates::StateStatus<Kind>> stateStatus_;
    StateStatusTimestamped<Kind> enqueued_;
    StateStatusTimestamped<Kind> dequeued_;
    StateStatusTimestamped<Kind> rejected_;
    StateStatusTimestamped<Kind> filtered_;
};

}}

#endif
