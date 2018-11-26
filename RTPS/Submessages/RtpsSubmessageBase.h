/*
 * RtpsSubmessageBase.h
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#ifndef RTPS_Submessages_RtpsSubmessageBase_h_IsIncluded
#define RTPS_Submessages_RtpsSubmessageBase_h_IsIncluded

#include"RTPS/Elements/IncludeLibs.h"
#include"RTPS/Headers/IncludeLibs.h"
#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"
namespace RTPS
{

class DLL_STATE RtpsSubmessageBase : public NetworkLib::NetObjectBase
{
public:
    RtpsSubmessageBase(const SubmessageHeader &header);
    RtpsSubmessageBase();
    virtual ~RtpsSubmessageBase();

    CLASS_TRAITS(RtpsSubmessageBase)

public:
    // ----------------------------------------------------------
    // Return submessage kind
    // ----------------------------------------------------------
    virtual SubmessageKind::Type GetSubmessageKind() const = 0;

    // ----------------------------------------------------------
    // Retrieves the byte size of each SubmessageKind
    // ----------------------------------------------------------
    virtual int64 GetSubmessageSize() const = 0;

    // ----------------------------------------------------------
    // Implements a validity check for each SubmessageKind
    // Refer to RTPS manual chapter 8.3.7 for each SubmessageKind
    // ----------------------------------------------------------
    virtual bool IsValid() const = 0;

public:
    // ----------------------------------------------------------
    // Convenience function to write SubmessageHeader and
    // Submessage in one call
    // ----------------------------------------------------------
    virtual bool WriteWithHeader(NetworkLib::SerializeWriter *);

public:
    bool IsBigEndian() const;
    bool IsLittleEndian() const;

public:
    void SetTimestamp(const Timestamp &time)			    { timeStamp_ = time;}
    void SetSrcGuidPrefix(const GuidPrefix &prefix)		{ sourceGuidPrefix_ = prefix; }
    void SetSubmessageHeader(const SubmessageHeader& h) 	{ header_ = h; }

    Timestamp 	        GetTimestamp()			const { return timeStamp_; }
    GuidPrefix	        GetSourceGuidPrefix() 	const { return sourceGuidPrefix_; }
    SubmessageHeader&	GetSubmessageHeader()	      { return header_; }

protected:
    SubmessageHeader	header_;

private:
    Timestamp	        timeStamp_;
    GuidPrefix 	        sourceGuidPrefix_;
};

} // namespace RTPS

#endif // RTPS_Submessages_RtpsSubmessageBase_h_IsIncluded
