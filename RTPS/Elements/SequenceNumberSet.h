/*
 * SequenceNumberSet.h
 *
 *  Created on: Mar 10, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Elements_SequenceNumberSet_h_IsIncluded
#define RTPS_Elements_SequenceNumberSet_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/SequenceNumber.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
struct SequenceNumberSet
{
	SequenceNumber_t bitmapBase;
	std::vector<long, 8> bitmap;
};

This document uses the following notation for a specific bitmap:

bitmapBase/numBits:bitmap

For example, in bitmap “1234/12:00110”, bitmapBase=1234 and numBits=12. The bits apply as follows to the sequence
numbers:
---------------------------------------------------------------------------------------------*/

class DLL_STATE SequenceNumberSet : public NetworkLib::NetObjectBase
{
public:
	typedef std::bitset<32>         BitSet32;
	//typedef std::bitset<256>        BitSet256;
	typedef std::vector<BitSet32>   VectorBitSet32;

public:
	SequenceNumberSet()
		: numBits_(0)
		, vectorBitMap32_(8)
	{}
	virtual ~SequenceNumberSet()
	{}

	const SequenceNumber&	GetBitmapBase() 	const { return bitmapBase_; }
	const int64& 	GetNumBits()		const { return numBits_; }

	void SetBitmapBase(const SequenceNumber &seq)	{ bitmapBase_ = seq; }

	bool IsEmpty() const
	{
		return bitmapBase_ == SequenceNumber::UNKNOWN();
	}

public:
	virtual void Write(NetworkLib::SerializeWriter *writer) const
	{
		bitmapBase_.Write(writer);
		writer->WriteInt64(numBits_);

		int vectorSize = (int)::ceil(float(numBits_)/32.0);

		// -- debug --
		ASSERT(numBits_ < 256);
		ASSERT(vectorSize >= 0);
		ASSERT(vectorSize <= 8);
		// -- debug --

		for(int i = 0; i < vectorSize; i++)
		{
			const BitSet32 &bitSet = vectorBitMap32_[i];
			int64 num = bitSet.to_ulong();
			writer->WriteInt64(num);
		}
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		bitmapBase_.Read(reader);
		numBits_ = reader->ReadInt64();

		int vectorSize = (int)::ceil(float(numBits_)/32.0);

		// -- debug --
		ASSERT(numBits_ < 256);
		ASSERT(vectorSize >= 0);
		ASSERT(vectorSize <= 8);
		// -- debug --

		for(int i = 0; i < vectorSize; i++)
		{
            int64 num = reader->ReadInt64();

			BitSet32 bitSet(num);
			vectorBitMap32_[i] = bitSet;
		}
	}

	bool AddSeqNumber(const SequenceNumber &seqNum, bool value)
	{
        int nextGapPos = int (seqNum.GetSequenceNumber() - bitmapBase_.GetSequenceNumber());
		if(nextGapPos > 255) return false;

		// -- debug --
		ASSERT(nextGapPos >= 0);
		// -- debug --

		return SetBitAtPosition(nextGapPos, value);
	}

    bool SetBitAtPosition(unsigned int pos, bool value)
	{
		// -- debug --
        //ASSERT(pos >= 0);
		//ASSERT(pos < 256);
		// -- debug --

        //if(pos < 0)     return false;
		if(pos > 255)   return false;

		int localPos = pos % 32;
		int vectorPos = (int)::floor(float(pos)/32.0);

		// -- debug --
		ASSERT(vectorPos >= 0);
		ASSERT(vectorPos < 8);
		ASSERT(localPos < 32);
		// -- debug --

		BitSet32 &bitSet = vectorBitMap32_[vectorPos];

		// TODO: check if we unset the last bit, and then reduce the size of numBits_
		/*if(value == false && numBits_ == pos)
		{
			if(bitSet.test(localPos))
			{
				// TODO: how to find last bit set efficiently?
			}
		} else */
		if(numBits_ < pos && value == true)
		{
			numBits_ = pos;
		}

		bitSet.set(localPos, value);

		return true;
	}

    bool TestBitAtPosition(unsigned int pos) const
	{
		if(pos > numBits_) return false;

		int localPos = pos % 32;
		int vectorPos = (int)::floor(float(pos)/32.0);

		// -- debug --
		ASSERT(vectorPos >= 0);
		ASSERT(vectorPos < 8);
		ASSERT(localPos < 32);
		// -- debug --

		const BitSet32 &bitSet = vectorBitMap32_.at(vectorPos);

		return bitSet.test(localPos);
	}

public:
	static unsigned int SIZE()
	{
		static unsigned int headerSize = 4 + SequenceNumber::SIZE();
		return headerSize;
	}

	static SequenceNumberSet UNKNOWN()
	{
		static SequenceNumberSet seq;
		return seq;
	}

public:
	bool operator==(const SequenceNumberSet& rvalue) const
	{
		return (this->bitmapBase_ == rvalue.bitmapBase_) &&	(this->numBits_ == rvalue.numBits_);
	}

	bool operator!=(const SequenceNumberSet& rvalue) const
	{
		return (this->bitmapBase_ != rvalue.bitmapBase_) || (this->numBits_ != rvalue.numBits_);
	}

private:
	SequenceNumber		bitmapBase_;
	int64       numBits_;
	//BitSet256         bitMap_;        //Specification: <long, 8> = 32 bits * 8 bits = 256 bits
	VectorBitSet32      vectorBitMap32_;
	//boost::dynamic_bitset<> bitMap_;
};

} // namespace RTPS

#endif // RTPS_Elements_SequenceNumberSet_h_IsIncluded
