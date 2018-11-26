/***************************************************************************
                          floyd_warshall_apspt.cc  -  description
                             -------------------
    begin                : Thu Jul 26 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "floyd_warshall_apsp.h"
#include <algorithm> // for std::min and std::max

using namespace std;
using namespace boost;

namespace TreeAlgorithms 
{
		  
void floyd_warshall_apsp(const GraphN &g, DistanceMatrix& d, ParentMatrix& p, const VertexSet &V)
{
	dEdgeMapConst w = get(&EdgeProp::weight, g);

	VertexSet::const_iterator firstv, lastv, firstv2, lastv2;
	edge_iteratorN 	 first, last;
	double inf = (std::numeric_limits<double>::max)();
	
	for(firstv = V.begin(), lastv = V.end(); firstv != lastv; firstv++)    // initialize matrix: distance infinity
	{
		for(firstv2 = V.begin(), lastv2 = V.end(); firstv2 != lastv2; firstv2++)
		{ 
			if(*firstv == *firstv2)	d[*firstv][*firstv2] = 0;
			else  d[*firstv][*firstv2] = inf; 

			p[*firstv][*firstv2] = -1; //*firstv;
			//cerr << "[FW::Initialization]  p: " << *firstv << " = " << p[*firstv][*firstv2] << endl;
		}
	}
	
    for(boost::tuples::tie(first, last) = edges(g); first != last; first++)
	{
		vertex_descriptorN src = source(*first, g), targ = target(*first, g);
		
		d[src][targ] = w[*first]; 	
		p[src][targ] = src;  
		
		d[targ][src] = w[*first];
		p[targ][src] = targ; 	
		
		//cerr << "[FW::Initialization] (" << source(*first, g) << "," << target(*first, g) << ") Parent map: " << p[source(*first, g)][target(*first, g)];
		//cerr << " Weight Map: " << d[source(*first, g)][target(*first, g)] << endl;
	}
    
	VertexSet::const_iterator i, lasti, j, lastj, k, lastk;
	for(k = V.begin(), lastk = V.end(); k != lastk; k++)
	{
		for(i = V.begin(), lasti = V.end(); i != lasti; i++)
		{
			for(j = V.begin(), lastj = V.end(); j != lastj; j++)
			{
				if(d[*i][*j] > d[*i][*k] + d[*k][*j])
					p[*i][*j] = p[*k][*j];

				d[*i][*j] = std::min(d[*i][*j], d[*i][*k] + d[*k][*j]);
			}
      	}
	}
}

void floyd_warshall_apsp(const GraphN &g, DistanceMatrix& d, ParentMatrix& p)
{
	dEdgeMapConst w = get(&EdgeProp::weight, g);

	vertex_iteratorN firstv, lastv, firstv2, lastv2;
	edge_iteratorN 	 first, last;
	double inf = (std::numeric_limits<double>::max)();
	
    // initialize matrix: distance infinity
    for(boost::tuples::tie(firstv, lastv) = vertices(g); firstv != lastv; firstv++)
	{
        for(boost::tuples::tie(firstv2, lastv2) = vertices(g); firstv2 != lastv2; firstv2++)
		{ 
			if(*firstv == *firstv2)	d[*firstv][*firstv2] = 0;
			else  d[*firstv][*firstv2] = inf; 

			p[*firstv][*firstv2] = -1; //*firstv;
			//cerr << "[FW::Initialization]  p: " << *firstv << " = " << p[*firstv][*firstv2] << endl;
		}
	}
	
    for(boost::tuples::tie(first, last) = edges(g); first != last; first++)
	{
		vertex_descriptorN src = source(*first, g), targ = target(*first, g);
		
		d[src][targ] = w[*first]; 	
		p[src][targ] = src;  
		
		d[targ][src] = w[*first];
		p[targ][src] = targ; 	

		//cerr << "[FW::Initialization] (" << source(*first, g) << "," << target(*first, g) << ") Parent map: " << p[source(*first, g)][target(*first, g)];
		//cerr << " Weight Map: " << d[source(*first, g)][target(*first, g)] << endl;
	}
    
	vertex_iteratorN i, lasti, j, lastj, k, lastk;
    for(boost::tuples::tie(k, lastk) = vertices(g); k != lastk; k++)  // main Floyd Warshall algorithm
	{
        for(boost::tuples::tie(i, lasti) = vertices(g); i != lasti; i++)
		{
            for(boost::tuples::tie(j, lastj) = vertices(g); j != lastj; j++)
			{
				if(d[*i][*j] > d[*i][*k] + d[*k][*j])
					p[*i][*j] = p[*k][*j];

				d[*i][*j] = std::min(d[*i][*j], d[*i][*k] + d[*k][*j]);
			}
      	}
	}
}

void floyd_warshall_apsp(const GraphN &g, DistanceVectorMatrix& d, ParentVectorMatrix& p, const VertexSet &V)
{
	dEdgeMapConst w = get(&EdgeProp::weight, g);

	VertexSet::const_iterator firstv, lastv, firstv2, lastv2;
	edge_iteratorN 	 first, last;
	double inf = (std::numeric_limits<double>::max)();
	
	for(firstv = V.begin(), lastv = V.end(); firstv != lastv; firstv++)    // initialize matrix: distance infinity
	{
		for(firstv2 = V.begin(), lastv2 = V.end(); firstv2 != lastv2; firstv2++)
		{ 
			if(*firstv == *firstv2)	d[*firstv][*firstv2] = 0;
			else  d[*firstv][*firstv2] = inf; 

			p[*firstv][*firstv2] = -1; //*firstv;
			//cerr << "[FW::Initialization]  p: " << *firstv << " = " << p[*firstv][*firstv2] << endl;
		}
	}
	
    for(boost::tuples::tie(first, last) = edges(g); first != last; first++)
	{
		vertex_descriptorN src = source(*first, g), targ = target(*first, g);
		
		d[src][targ] = w[*first]; 	
		p[src][targ] = src;  
		
		d[targ][src] = w[*first];
		p[targ][src] = targ; 	
		
		//cerr << "[FW::Initialization] (" << source(*first, g) << "," << target(*first, g) << ") Parent map: " << p[source(*first, g)][target(*first, g)];
		//cerr << " Weight Map: " << d[source(*first, g)][target(*first, g)] << endl;
	}
    
	VertexSet::const_iterator i, lasti, j, lastj, k, lastk;
	for(k = V.begin(), lastk = V.end(); k != lastk; k++)
	{
		for(i = V.begin(), lasti = V.end(); i != lasti; i++)
		{
			for(j = V.begin(), lastj = V.end(); j != lastj; j++)
			{
				if(d[*i][*j] > d[*i][*k] + d[*k][*j])
					p[*i][*j] = p[*k][*j];

				d[*i][*j] = std::min(d[*i][*j], d[*i][*k] + d[*k][*j]);
			}
      	}
	}
}



}; // namespace TreeAlgorithms


