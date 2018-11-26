/*
 * SerializedPayload.h
 *
 *  Created on: Mar 8, 2012
 *      Author: knuthelv
 */
#ifndef RTPS_Elements_SerializedPayload_h_IsIncluded
#define RTPS_Elements_SerializedPayload_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/CommonDefines.h"
#include"RTPS/CommonDefines.h"
#include"RTPS/Export.h"
namespace RTPS
{
/*--------------------------------------------------------------------------------------------
    class SerializedPayload
---------------------------------------------------------------------------------------------*/
class DLL_STATE SerializedPayload : public NetworkLib::NetObjectBase
{
public:
    SerializedPayload(const NetworkLib::SerializeWriter::Ptr &payload, unsigned short endian = OMG_CDR_BE)
        : endian_(endian)
        , length_(0)
        , serializedOutPayload_(payload)
    {
        if(!serializedOutPayload_)
            throw CriticalException("Initialized with null-pointer payload!");

        length_ = serializedOutPayload_->GetLength();
    }
    SerializedPayload(unsigned short endian = OMG_CDR_BE)
        : endian_(endian)
        , length_(0)
    {

    }
    virtual ~SerializedPayload()
    { }


    CLASS_TRAITS(SerializedPayload)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        if(!serializedOutPayload_)
        {
            IWARNING() << "No data!";
            return;
        }

        writer->WriteInt16(endian_);
        writer->WriteInt16(length_);
        writer->Write(serializedOutPayload_->GetData(), serializedOutPayload_->GetLength());
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        endian_ = reader->ReadInt16();

        switch(endian_)
        {
            case OMG_CDR_BE:
            {
                break;
            }
            case OMG_CDR_LE:
            {
                break;
            }
            case OMG_PL_CDR_BE:
            {
                break;
            }
            case OMG_PL_CDR_LE:
            {
                break;
            }
            default:
                throw CriticalException("Did not recognize endian type!");
                break;
        }

        length_ = reader->ReadInt16();

        std::vector<char> payload(length_, 0);
        reader->Read(&payload[0], length_);
        serializedInPayload_ = InByteArray::Ptr(new InByteArray(length_));
        serializedInPayload_->CopyData(&payload[0], length_);
    }

public:
    const InByteArray::Ptr& 	GetInPayload() 			const { return serializedInPayload_; }
    unsigned short				GetEndian() 			const { return endian_; }
    unsigned short              GetSize()               const { return length_; }

public:
    void SetOutPayload(const NetworkLib::SerializeWriter::Ptr &ptr)
    {
        ASSERT(ptr);
        serializedOutPayload_ = ptr;
        length_ = serializedOutPayload_->GetLength();
    }

    void SetEndian(unsigned short endian)				{ endian_ = endian; }

private:
    unsigned short 			endian_;
    unsigned short			length_;
    NetworkLib::SerializeWriter::Ptr 	serializedOutPayload_;
    InByteArray::Ptr		serializedInPayload_;
};

} // namespace RTPS

#endif // RTPS_Elements_SerializedPayload_h_IsIncluded
