/***************************************************************************
                          VivaldiInformation.cpp  -  description
                             -------------------
    begin                : Thu May 10 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "NetworkInformationLib/VivaldiInformation.h"

#include <iostream>
#ifdef USE_GCC
#include <sys/utsname.h>
#endif

#include <vector>

//using namespace std;
//using namespace boost;
//using namespace GraphAlgorithms;

namespace NetworkInformationLib
{

/*-----------------------------------------------------------------------
						vivaldi operators
------------------------------------------------------------------------- */
//std::ostream& operator<<(std::ostream& ostr, const VivaldiInfo &vv)
//{
	//ostr << "eid " << vv.eid << " msg: " << vv.getMsg() <<  " type: " << vv.getType() << " coords: " << vv.getCoords() << " err: " << vv.getErr() << " rtt: " << vv.getRtt() ;
//	return ostr;
//}

/*std::ostream& operator<<(std::ostream& ostr, const std::vector<double> &coords)
{
	ostr << "(" ;
	for(std::vector<double>::const_iterator vit = coords.begin(), vit_end = coords.end(); vit != vit_end; ++vit)
	{
		ostr << *vit << "," ;
	}
	ostr << ")"; 
	
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const double *coords)
{
	ostr << "(" ;
	for(int i = 0; i < getDim(); i++)
	{
		ostr << coords[i] << "," ;
	}
	ostr << ")"; 
	
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const VivaldiSpace::VivaldiType& type)
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
}
*/

} // namespace NetworkInformationLib

