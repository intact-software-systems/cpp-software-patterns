/*
 * Timestamp.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_Timestamp_h_IsIncluded
#define RTPS_Elements_Timestamp_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
 	Time_t
The representation of the time is the one defined by the IETF Network Time Protocol (NTP)
Standard (IETF RFC 1305). In this representation, time is expressed in seconds and fraction
of seconds using the formula:

time = seconds + (fraction / 2^(32))

page: 153

struct Time_t
{
	long seconds; // time in seconds
	int64 fraction; // time in sec/2^32
};

typedef Time_t Timestamp;

//Mapping of the reserved values:
#define TIME_ZERO {0, 0}
#define TIME_INVALID {-1, 0xffffffff}
#define TIME_INFINITE {0x7fffffff, 0xffffffff}
---------------------------------------------------------------------------------------------*/
class DLL_STATE Timestamp : public NetworkLib::NetObjectBase
{
public:
	Timestamp(const long &sec, const int64 &frac)
		: seconds_(sec)
		, fraction_(frac)
	{}
	Timestamp()
		: seconds_(-1)
		, fraction_(0xffffffff)
	{}
	virtual ~Timestamp()
	{}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		writer->WriteInt64(seconds_);
		writer->WriteInt64(fraction_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		seconds_ = reader->ReadInt64();
		fraction_ = reader->ReadInt64();
	}

public:
    static Timestamp ZERO()
	{
		static Timestamp ts(0,0);
		return ts;
	}

    //static Timestamp NOW()
	//{
	//	static Timestamp ts(0,0);
	//	return ts;
	//}

    static Timestamp INVALID()
	{
		static Timestamp ts(-1,0xffffffff);
		return ts;
	}

    static Timestamp Infinite()
	{
		static Timestamp ts(0x7fffffff,0xffffffff);
		return ts;
	}

	static unsigned int SIZE()
	{
		static unsigned int sz = 8;
		return sz;
	}

public:
	bool operator==(const Timestamp &other) const
	{
		return (seconds_ == other.seconds_) && (fraction_ == other.fraction_);
	}

	bool operator!=(const Timestamp &other) const
	{
		return !(*this == other);
	}

public:
	inline void SetTime(long seconds, int64 frac) 	{ seconds_ = seconds; fraction_ = frac; }
	inline void SetSeconds(long seconds)				  	{ seconds_ = seconds; }
	inline void SetFraction(int64 frac)				{ fraction_ = frac; }

	inline int64 GetTime()		const { return seconds_ + (fraction_ / 2^32); }
	inline long	GetSeconds()			const { return seconds_; }
	inline int64 GetFraction() 	const { return fraction_; }

private:
	long 			seconds_; 	// time in seconds
	int64 	fraction_; 	// time in sec/2^32
};

} // namespace RTPS

#endif // RTPS_Elements_Timestamp_h_IsIncluded
