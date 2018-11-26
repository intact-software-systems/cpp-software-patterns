/***************************************************************************
                          VivaldiInformation.h  -  description
                             -------------------
    begin                : Thu May 10 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef NetworkInformationLib_VivaldiInformation_h_IsIncluded
#define NetworkInformationLib_VivaldiInformation_h_IsIncluded

#include <iostream>

#ifdef USE_GCC
#include <sys/utsname.h>
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/shared_array.hpp>

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/vivaldi.h"
#include "NetworkInformationLib/VivaldiDefines.h"
#include "NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{

typedef std::pair<int, int>		EdgeId;

namespace VivaldiSpace
{
	static int getDim()
	{
		return VIVALDI_DIMENSION;
	}

	enum VivaldiType
	{
		VIVALDI_TYPE_ERROR = 0,

		NO_TYPE,
		NEIGHBOR_UNREACHABLE,
		NEIGHBOR,
		FROM_SERVER,
		TO_SERVER
	};

	enum MessageType
	{
		MESSAGE_TYPE_ERROR = 0,
		NO_MESSAGE_TYPE
	};

} // end namespace VivaldiSpace

DLL_STATE std::ostream& operator<<(std::ostream& ostr, const VivaldiSpace::VivaldiType& type);

/*-----------------------------------------------------------------------
					vivaldi info passed to threads
------------------------------------------------------------------------- */
struct DLL_STATE VivaldiInfo : public MicroMiddleware::RmiObjectBase
{
public:
	VivaldiInfo()
        : RmiObjectBase()
        , eid(-1,-1)
	{ }		
	VivaldiInfo(const VivaldiInfo &vv)
        : RmiObjectBase()
		, eid(vv.getEid())
		, nodeData(vv.getNodeData())
		, hostInformation(vv.getHostInformation())
	{ }
	VivaldiInfo(const VVNodeData &vn, const HostInformation &hostInfo = HostInformation()) 
        : RmiObjectBase()
		, eid(-1,-1)
		, nodeData(vn)
		, hostInformation(hostInfo)
	{ }
	~VivaldiInfo() {}

	inline HostInformation 	getHostInformation()						const { return hostInformation; }
	inline VVNodeData 		getNodeData()								const { return nodeData; }
	inline EdgeId 			getEid() 									const { return eid; }
	inline int				getId()										const { return nodeData.GetNodeId(); }

	
	inline void setHostInformation(const HostInformation &hostInfo)		{ hostInformation = hostInfo; }
	inline void setCoords(const vector<double> c) 						{ nodeData.SetCoords(c); }
	inline void setErr(const double &err) 								{ nodeData.SetErr(err); }
	inline void setRtt(const double &rtt) 								{ nodeData.SetRtt(rtt); }
	inline void setId(const int &i)										{ nodeData.SetNodeId(i); }
	inline void setEid(const int &u, const int &v)						{ eid = EdgeId(u,v); }
	inline void setEid(const EdgeId &id)								{ eid = id; }
	inline void setNodeData(const VVNodeData &n)						{ nodeData= n; }
	
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		nodeData.Write(writer);
		hostInformation.Write(writer);
		writer->WriteInt32(eid.first);
		writer->WriteInt32(eid.second);
	}
	
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		nodeData.Read(reader);
		hostInformation.Read(reader);
		eid.first = reader->ReadInt32();
		eid.second = reader->ReadInt32();
	}

	friend std::ostream& operator<<(std::ostream& ostr, const VivaldiSpace::VivaldiType& type)
	{
		switch(type)
		{
#define CASE(t) case t: ostr << #t; break;
			CASE(VivaldiSpace::VIVALDI_TYPE_ERROR)
			CASE(VivaldiSpace::NO_TYPE)
			CASE(VivaldiSpace::NEIGHBOR_UNREACHABLE)
			CASE(VivaldiSpace::NEIGHBOR)
			CASE(VivaldiSpace::FROM_SERVER)
			CASE(VivaldiSpace::TO_SERVER)
#undef CASE
			default:
				cerr << "Missing operator<< case for VivaldiType " << endl;
				ASSERTING(0);
				break;
		}
		return ostr;
	}

	friend std::ostream& operator<<(std::ostream& ostr, const double *coords)
	{
		ostr << "(" ;
		for(int i = 0; i < VivaldiSpace::getDim(); i++)
		{
			ostr << coords[i] << "," ;
		}
		ostr << ")"; 

		return ostr;
	}

	/*friend std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords)
	{
		ostr << "(" ;
		for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
		{
			ostr << *vit << "," ;
		}
		ostr << ")"; 

		return ostr;
	}*/
	
	friend std::ostream& operator<<(std::ostream& ostr, const VivaldiInfo &vv)
	{
		ostr << "eid (" << vv.eid.first << "," << vv.eid.second << ") " << vv.getNodeData();
		return ostr;
	}

public:
	EdgeId 			eid;
	VVNodeData		nodeData;
	HostInformation hostInformation;
};

//typedef boost::shared_ptr<VivaldiInfo> 	VivaldiInfoPtr;
//std::ostream& operator<<(std::ostream& ostr, const VivaldiInfo &vv);
//std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords);
//std::ostream& operator<<(std::ostream& ostr, const double *coords);

} // end namespace NetworkInformationLib

#endif // VivaldiInformation_h_IsIncluded

