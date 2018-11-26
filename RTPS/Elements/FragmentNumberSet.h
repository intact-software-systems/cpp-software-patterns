/*
 * FragmentNumberSet.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_FragmentNumberSet_h_IsIncluded
#define RTPS_Elements_FragmentNumberSet_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/FragmentNumber.h"

#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
struct FragmentNumberSet
{
	FragmentNumber_t bitmapBase;
	std::vector<long, 8> bitmap;
};

This document uses the following notation for a specific bitmap:

bitmapBase/numBits:bitmap

For example, in bitmap “1234/12:00110”, bitmapBase=1234 and numBits=12. The bits apply as follows to the sequence
numbers:
---------------------------------------------------------------------------------------------*/

// TODO: Finish implementation!
class DLL_STATE FragmentNumberSet : public NetworkLib::NetObjectBase
{
public:
	FragmentNumberSet()
		: numBits_(-1)
	{}
	virtual ~FragmentNumberSet()
	{}

	const FragmentNumber&	GetBitmapBase() 	const { return bitmapBase_; }
	const int64& 	GetNumBits()		const { return numBits_; }

	void SetBitmapBase(const FragmentNumber &seq)	{ bitmapBase_ = seq; }
	//void SetBitmap(

	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		bitmapBase_.Write(writer);
		writer->WriteInt64(numBits_);

		//for(std::bitset<long>::iterator it = bitMap_.begin(), it_end = bitMap_.end(); it != it_end; ++it)
		//{
		//	writer->WriteInt32(*it);
		//}
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		bitmapBase_.Read(reader);
		numBits_ = reader->ReadInt64();
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int headerSize = FragmentNumber::SIZE() + 4;
		return headerSize;
	}

private:
	FragmentNumber				bitmapBase_;
	int64				numBits_;
	//std::bitset<unsigned int> 	bitMap_;
};

} // namespace RTPS

#endif // RTPS_Elements_FragmentNumberSet_h_IsIncluded
