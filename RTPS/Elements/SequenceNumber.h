/*
 * SequenceNumber.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_SequenceNumber_h_IsIncluded
#define RTPS_Elements_SequenceNumber_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
    Mapping of the type SequenceNumber_t
    Using this structure, the 64-bit sequence number is:
    seq_num = high * 2^32 + low

struct SequenceNumber_t
{
    long high;
    int64 low;
};

//typedef SequenceNumber_t SequenceNumber;

//Mapping of the reserved values:
#define SEQUENCENUMBER_UNKNOWN {-1,0}
---------------------------------------------------------------------------------------------*/
class DLL_STATE SequenceNumber : public NetworkLib::NetObjectBase
{
public:
    typedef int64_t		Value;

    SequenceNumber(Value value = MIN_VALUE)
        : high_(-1)
        , low_(0)
    {
        setValue(value);
    }
    virtual ~SequenceNumber()
    {}

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt64(high_);
        writer->WriteInt64(low_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        high_ = reader->ReadInt64();
        low_ = reader->ReadInt64();
    }

public:
    inline int64_t GetSequenceNumber() const { return LOW_BASE * this->high_ + this->low_; } //return (high_ * 2^32 + low_); }

    // SEQUENCENUMBER_UNKOWN is defined by the RTPS spec.
    static SequenceNumber UNKNOWN()
    {
        static SequenceNumber seqNum(-1,0);
        return seqNum;
    }

    static unsigned int SIZE()
    {
        static unsigned int sz = 8;
        return sz;
    }

public:

    /// Pre-increment.
    SequenceNumber& operator++()
    {
        if(this->low_ == INTACT_UINT32_MAX)
        {
            if(this->high_ == INTACT_INT32_MAX)
            {
                // this code is here, despite the RTPS spec statement:
                // "sequence numbers never wrap"
                this->high_ = 0;
                this->low_ = 1;
            }
            else
            {
                ++this->high_;
                this->low_ = 0;
            }
        }
        else
        {
            ++this->low_;
        }
        return *this ;
    }

    /// Post-increment.
    SequenceNumber operator++(int)
    {
        SequenceNumber value(*this);
        ++*this;
        return value ;
    }

    SequenceNumber previous() const
    {
        SequenceNumber retVal(*this);
        if ((this->low_ == 1) && (this->high_ == 0))
        {
            retVal.high_ = INTACT_INT32_MAX;
            retVal.low_  = INTACT_UINT32_MAX;
            return retVal;
        }
        if (this->low_ == 0)
        {
            --retVal.high_;
            retVal.low_ = INTACT_UINT32_MAX;
        }
        else
        {
            --retVal.low_;
        }
        return retVal ;
    }


public:
    /// N.B. This comparison assumes that the shortest distance between
    ///      the values being compared is the positive counting
    ///      sequence between them.  This means that MAX-2 is less
    ///      than 2 since they are separated by only four counts from
    ///      MAX-2 to 2.  But that 2 is less than MAX/2 since the
    ///      shortest distance is from 2 to MAX/2.
    bool operator<(const SequenceNumber& rvalue) const
    {
        const int64_t distance = int64_t(rvalue.high_ - high_)*2;
        return (distance == 0) ?
                (this->low_ < rvalue.low_) :   // High values equal, compare low
                (distance < 0) ?               // Otherwise just use high
                        (INTACT_INT32_MAX < -distance) :
                        (distance < INTACT_INT32_MAX);
    }

    /// Derive a full suite of logical operations.
    bool operator==(const SequenceNumber& rvalue) const
    {
        return (this->high_ == rvalue.high_) &&
                (this->low_ == rvalue.low_) ;
    }

    bool operator!=(const SequenceNumber& rvalue) const
    {
        return (this->high_ != rvalue.high_) ||
                (this->low_ != rvalue.low_) ;
    }

    bool operator>=(const SequenceNumber& rvalue) const
    {
        return !(*this  < rvalue);
    }

    bool operator<=(const SequenceNumber& rvalue) const
    {
        return !(rvalue < *this);
    }

    bool operator>(const SequenceNumber& rvalue) const
    {
        return (rvalue < *this) && (*this != rvalue);
    }

public:

    friend std::ostream& operator<<(std::ostream &ostr, const SequenceNumber &seq)
    {
        ostr << seq.GetSequenceNumber();
        return ostr;
    }

    friend std::ostream& operator<<(std::ostream &ostr, const std::set<SequenceNumber> &handleSeq)
    {
        ostr << "(";
        for(std::set<SequenceNumber>::const_iterator it = handleSeq.begin(), it_end = handleSeq.end(); it != it_end; ++it)
            ostr << it->GetSequenceNumber() << ", ";
        ostr << ")";

        return ostr;
    }

    friend std::ostream& operator<<(std::ostream &ostr, const std::pair<SequenceNumber, SequenceNumber> &handleRange)
    {
        ostr << "(" << handleRange.first << "-" << handleRange.second << ")";
        return ostr;
    }

private:
    void setValue(Value value)
    {
        if (value < MIN_VALUE)
        {
            value = MIN_VALUE;
        }
        this->high_ = (long) (value / LOW_BASE);
        this->low_  = (int64) (value % LOW_BASE);
    }

    static const Value MAX_VALUE = 0x7fffffffffffffff; //INT_FAST64_MAX; //(std::numeric_limits<int64_t>::max)();
    static const Value MIN_VALUE = 1;
    static const Value LOW_BASE = 0x0000000100000000LL;

private:
    // Private constructor used to force construction of SEQUENCENUMBER_UNKNOWN.
    // Also used by operator>> to allow deserialization of the same value.
    SequenceNumber(long high, int64 low)
    : high_(high)
    , low_(low)
    {}

private:
    long 	high_;
    int64 	low_;
};

typedef std::pair<SequenceNumber, SequenceNumber> 	SequenceRange;
typedef std::set<SequenceNumber>					SequenceSet;

} // namespace RTPS

DLL_STATE inline RTPS::SequenceNumber operator+(const RTPS::SequenceNumber& lhs, int rhs)
{
    return RTPS::SequenceNumber(lhs.GetSequenceNumber() + rhs);
}

DLL_STATE inline RTPS::SequenceNumber operator+(int lhs, const RTPS::SequenceNumber& rhs)
{
    return rhs + lhs;
}

#endif // RTPS_Elements_SequenceNumber_h_IsIncluded
