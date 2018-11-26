/***************************************************************************
                          recordtree.cc  -  description
                             -------------------
    begin                : Mon Sep 11 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "recordtree.h"
#include <fstream>
#include <iostream>
#include <string.h>
//#include "../graphviz/graphviz.h"
#include "../boostprop.h"

using namespace std;
using namespace TreeAlgorithms;

clock_t StatsContainer::overhead_time = 0;

namespace Statistics {

void recordTree(ostream& ostr, const StatsContainer &SC)
{
	const TreeStructure &T = SC.Tc;
	
	// -- debug --
	double num_znodes = 0, num_spnodes = 0;
	ASSERTING(T.Ep.size() == num_edges(T.g));
	ASSERTING(T.E.size() == num_edges(T.g));
	// -- debug end --
	
	ostr << "-ST 0"; // << SimTime::Now() ;
	if(strcmp("no_algo", printAlgo(SC.treeAlgo)) != 0) 
		ostr << " -TA " << printAlgo(SC.treeAlgo) ;
	if(strcmp("no_algo", printAlgo(SC.dynamicTreeAlgo)) != 0) 
		ostr << " -DA " << printAlgo(SC.dynamicTreeAlgo) ;
	if(strcmp("no_algo", printAlgo(SC.reconfigAlgo)) != 0) 
		ostr << " -RA " << printAlgo(SC.reconfigAlgo) ;
	if(strcmp("no_algo", printAlgo(SC.meshAlgo)) != 0)
		ostr << " -MA " << printAlgo(SC.meshAlgo) ;
	if(strcmp("no_algo", printAlgo(SC.dynamicMeshAlgo)) != 0)
		ostr << " -DMA " << printAlgo(SC.dynamicMeshAlgo) ;

	ostr << " -ET " << SC.exec_time; 
	ostr << " -OT " << StatsContainer::overhead_time ;
	ostr << " -CC " << SC.cpu_cycles;
	ostr << " -gId " << SC.gId ;
	ostr << " -Echa " << (findDifference(SC.Tp.Ep, T.Ep)).size() ;
	ostr << " -Val " << T.isValid();
	
	ostr << " -Z ";
	VertexSet::const_iterator vit = T.V.begin(), vit_end = T.V.end();
	for( ; vit != vit_end; ++vit)
	{
		if(T.g[*vit].vertexState == GROUP_MEMBER)
		{
			ostr << *vit << " ";
			num_znodes++;
		}
		else
		{
			if(T.g[*vit].vertexState != STEINER_POINT)
			{
				cerr << WRITE_FUNCTION << " error vertex " << *vit << " not group member or steiner point : " << T.g[*vit] << endl;
			}
			ASSERTING(T.g[*vit].vertexState == STEINER_POINT);
		  	num_spnodes++;
		}
	}
	
	ostr << " -S ";
	vit = T.S.begin();
   	vit_end = T.S.end();
	for( ; vit != vit_end; ++vit)
	{
		ASSERTING(T.g[*vit].vertexState == STEINER_POINT);
		ostr << *vit << " ";
	}

	if(!T.C.empty())
	{
		ostr << " -C ";
		vit = T.C.begin();
	   	vit_end = T.C.end();
		for( ; vit != vit_end; ++vit)
			ostr << *vit << " ";
	}
	
	ostr << "-E ";
	EdgePair::const_iterator pit = T.Ep.begin(), pit_end = T.Ep.end();
	for( ; pit != pit_end; ++pit)
	{
		ostr << *pit << " ";
	}
	ostr << endl;

	// -- debug --
	ASSERTING(num_znodes == T.V.size() - T.S.size());
	ASSERTING(num_spnodes == T.S.size());
	// -- debug end --
	
	//cerr << WRITE_PRETTY_FUNCTION << " -ET " << SC.exec_time << " -OT " << StatsContainer::overhead_time << endl;
	StatsContainer::overhead_time = 0;
}

bool doFlush()
{
	// do we need to flush to file -> only when 90 % or more filled up
	//if(statBuffer.size() > (0.9 * statBuffer.capacity())) return true;

	return true;
}

EdgePair findDifference(const EdgePair &a, const EdgePair &b)
{
	EdgePair unionEp(a), interEp;
	EdgePair::const_iterator bit = b.begin(), bit_end  = b.end();
	for( ; bit != bit_end; ++bit)
	{
		bool intersection = false;
		EdgePair::iterator unit = unionEp.begin(), unit_end = unionEp.end();
		for( ; unit != unit_end; ++unit)
		{
			if(*unit == *bit)
			{
				intersection = true;
				break;
			}
		}
	
		if(intersection) interEp.push_back(*bit);
		else unionEp.push_back(*bit);
	}

	// -- debug --
	//cerr << "Union: " << endl;
	//EdgePair::iterator uni_it = unionEp.begin(), uni_it_end = unionEp.end();
	//for( ; uni_it != uni_it_end; ++uni_it)
	//		cerr << *uni_it << endl;
	
	//cerr << "Insersection: " << endl;
	//uni_it = interEp.begin();
   	//uni_it_end = interEp.end();
	//for( ; uni_it != uni_it_end; ++uni_it)
	//		cerr << *uni_it << endl;
	// -- debug end --
	 
	EdgePair::iterator unit = interEp.begin(), unit_end = interEp.end();
	for( ; unit != unit_end; ++unit)
	{
		unionEp.removeEdge(*unit);
	}
	
	//cerr << "Diff: " << endl;
	//EdgePair::iterator uni_it = unionEp.begin(), uni_it_end = unionEp.end();
	//for( ; uni_it != uni_it_end; ++uni_it)
	//		cerr << *uni_it << " ";
	//cerr << endl;
	
	return unionEp;
}

double findMedian(const std::multiset<double> &dset)
{
	double final_median = 0;
	int i; 
	std::multiset<double>::const_iterator sit, sit_end;
	for(sit = dset.begin(), sit_end = dset.end(), i = 0; sit != sit_end; ++sit, i++)
	{
		//cerr << i << " " << *sit << ",";
		if((int)(dset.size()/2) <= i)
			break;
	}
	final_median = *sit;
	
	if(dset.size() % 2 != 0 && dset.size() > 3) // odd number
		final_median = (final_median + (*(++sit)))/2;

	//cerr << WRITE_FUNCTION << " final_median: " << final_median << endl;
	return final_median;
}

double findMedian(const std::vector<double> &arr)
{
    unsigned int size = arr.size();

    //cerr << WRITE_FUNCTION << " size " << size << endl;
    int middle = size/2;
    double final_median = arr[middle];
    //cerr << " arr[" << middle << "] = " << final_median << endl;

    if(size % 2 != 0 && size > 3)
    {
        // odd number
        final_median = (final_median + (arr[middle+1]))/2;
        //cerr << " odd number : final_median[middle+1] " << arr[middle+1] << endl;
    }

    //cerr << WRITE_FUNCTION << " final_median: " << final_median << endl;
    return final_median;
}

double findMedian(const std::vector<double> &arr, const int &size)
{
    //cerr << WRITE_FUNCTION << " size " << size << endl;
    int middle = size/2;
    double final_median = arr[middle];
    //cerr << " arr[" << middle << "] = " << final_median << endl;

    if(size % 2 != 0 && size > 3)
    {
        // odd number
        final_median = (final_median + (arr[middle+1]))/2;
        //cerr << " odd number : final_median[middle+1] " << arr[middle+1] << endl;
    }

    //cerr << WRITE_FUNCTION << " final_median: " << final_median << endl;
    return final_median;
}

double findMedian(double arr[], const int &size)
{
	//cerr << WRITE_FUNCTION << " size " << size << endl;
	int middle = size/2;
	double final_median = arr[middle];
	//cerr << " arr[" << middle << "] = " << final_median << endl;

	if(size % 2 != 0 && size > 3)
	{	
		// odd number
		final_median = (final_median + (arr[middle+1]))/2;
		//cerr << " odd number : final_median[middle+1] " << arr[middle+1] << endl;
	}

	//cerr << WRITE_FUNCTION << " final_median: " << final_median << endl;
	return final_median;
}

}; // namespace Statistics


