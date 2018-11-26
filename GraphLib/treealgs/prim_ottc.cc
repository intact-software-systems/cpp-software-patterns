/***************************************************************************
                          prim_ottc.cc  -  description
                             -------------------
    begin                : Wed May 30 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "prim_ottc.h"
#include "treealgs.h"
#include "fheap.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost;
using namespace std;	

namespace TreeAlgorithms
{

bool OTTC(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	
	vector<int> near(num_vertices(g));
	DistanceVector ecc(num_vertices(g));
	DistanceVector		diameter_bound(num_vertices(g));
	using namespace boost::numeric::ublas;
	matrix<double> dist(num_vertices(g), num_vertices(g));
	
	pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
	
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{	
		diameter_bound[*vit] = D;
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) //T.V.size() < treeNodes.size())
	{
		int z = nearest_vert.first;
		
		ASSERTING(z > -1);
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		notInTree = notInTree - z;
		if(notInTree.empty()) break;

		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double weight_ep = (std::numeric_limits<double>::max)();
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			if(near[*vit] > -1) 
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);

				curr_ecc = ecc[near[*vit]] + wmap[ep.first];
				weight_ep = wmap[ep.first];
			}

			if(curr_ecc > diameter_bound[*vit])
			{
				//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
				for(vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
					if(!ep.second) continue;
					
					double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
					//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
					
					if(new_ecc < curr_ecc)
					{ 
						//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
						near[*vit] = *vit_in;
						curr_ecc = new_ecc;
					}	
				}
			}	
			else
			{
				pair<edge_descriptorN, bool> epz = edge(*vit, z, g);
				if(epz.second)
				{
					//cerr << "Compare w(" << *vit << "," << near[*vit] << ") = " << weight_ep << " to w(" << *vit << "," <<  z << ")= " << wmap[epz.first] << endl;
					if(wmap[epz.first] <= weight_ep)
					{
						//cerr << " new near [" << *vit << "] = " << z << endl;
						near[*vit] = z;
					}
				}
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());

		while(nearest_vert.first == -1)
		{	
			VertexSet diameterBroken;
			// find new z
			for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);
				
					if(ecc[near[*vit]] + wmap[ep.first] <= diameter_bound[*vit] && nearest_vert.second > wmap[ep.first])
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = wmap[ep.first]; 
					}
					
					if(ecc[near[*vit]] + wmap[ep.first] > diameter_bound[*vit]) diameterBroken.insert(*vit);
				}
			}
			if(nearest_vert.first > -1) break;
			
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
			
			if(diameterBroken.empty()) return false;
			if(TreeAlgorithms::relaxDiameter(diameterBroken, diameter_bound) == false) return false;
			
			//ASSERTING(!diameterBroken.empty());
			// try to increase broken limits
			//for(VertexSet::iterator vit = diameterBroken.begin(), vit_end = diameterBroken.end(); vit != vit_end; ++vit)
			//{
			//	diameter_bound[*vit] = diameter_bound[*vit] + 0.1;
				//cerr << *vit << " increased diameter bound " <<  diameter_bound[*vit] << endl; 
			//}

			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;

	return true;
}


bool dlOTTC(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	
	dEdgeMapConst 		wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN 	oit, oit_end;
	
	vector<int> 		near(num_vertices(g));
	DistanceVector 		ecc(num_vertices(g));
	DistanceVector		diameter_bound(num_vertices(g));
	DistanceVector		degree_bound(num_vertices(g));
	using namespace 	boost::numeric::ublas;
	matrix<double> 		dist(num_vertices(g), num_vertices(g));
	pair<int, double> 	nearest_vert(-1,(std::numeric_limits<double>::max)());
	int 				degreeLeverage = 0;
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
		diameter_bound[*vit] = D;
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(VertexSet::iterator vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 
	{
		int z = nearest_vert.first;
	
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;

		ASSERTING(z > -1);
		ASSERTING(notInTree.contains(z));
		ASSERTING(T.V.contains(near[z]));
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		ASSERTING(degree_bound[near[z]] >= getOutDegree(T.g, near[z]));
		notInTree = notInTree - z;
		if(notInTree.empty()) break;
		
		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double dc_near_vit = (numeric_limits<double>::max)(), od_near_vit = 0; 
			if(near[*vit] > -1) 
			{
				dc_near_vit = degree_bound[near[*vit]]; 
				od_near_vit = getOutDegree(T.g, near[*vit]);
			}

			double weight_ep = (std::numeric_limits<double>::max)();
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			if(near[*vit] > -1 && od_near_vit < dc_near_vit) 
			{
				//cerr << " edge (" << *vit << "," << near[*vit] << ")" << endl;
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);

				curr_ecc = ecc[near[*vit]] + wmap[ep.first];
				weight_ep = wmap[ep.first];
			}
		
			//if(curr_ecc > diameter_bound[*vit] || (od_near_vit >= dc_near_vit && getOutDegree(T.g, z) >= degree_bound[z]) )  
			if(curr_ecc > diameter_bound[*vit] || getOutDegree(T.g, z) >= degree_bound[z])
			{
				//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
				for(VertexSet::iterator vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
					if(!ep.second) continue;
					
					double new_ecc = ecc[*vit_in] + wmap[ep.first]; //1;
					//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
				
					if(new_ecc < curr_ecc && getOutDegree(T.g, *vit_in) < degree_bound[*vit_in] )  
					{ 
						//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
						near[*vit] = *vit_in;
						curr_ecc = new_ecc;
					}	
				}
			}	
			else
			{
				pair<edge_descriptorN, bool> epz = edge(*vit, z, g);
				if(epz.second)
				{				
					//cerr << "Compare w(" << *vit << "," << near[*vit] << ") = " << weight_ep << " to w(" << *vit << "," <<  z << ")= " << wmap[epz.first] << endl;
					if(wmap[epz.first] <= weight_ep && getOutDegree(T.g, z) < degree_bound[z])
					{
						//cerr << " new near [" << *vit << "] = " << z << endl;
						near[*vit] = z;
					}
				}
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
		while(nearest_vert.first == -1)
		{	
			VertexSet diameterBroken, degreeBroken;	
			// find new z
			for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);

					double dc_near_vit = degree_bound[near[*vit]];
					double od_near_vit = getOutDegree(T.g, near[*vit]);

					if(ecc[near[*vit]] + wmap[ep.first] <= diameter_bound[*vit] && nearest_vert.second > wmap[ep.first] && od_near_vit < dc_near_vit)
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = wmap[ep.first]; 
					}

					if(ecc[near[*vit]] + wmap[ep.first] > diameter_bound[*vit]) diameterBroken.insert(*vit);
					if(od_near_vit >= dc_near_vit) degreeBroken.insert(near[*vit]);
				}
			}
			if(nearest_vert.first > -1) break;
		
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());

			if(!TreeAlgorithms::relaxDegreeAndDiameter(degreeBroken, degree_bound, diameterBroken, diameter_bound)) return false; 
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, diameterBroken)) return false;
			
			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;
	return true;
}


bool mdOTTC(const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	
	vector<int> near(num_vertices(g));
	DistanceVector ecc(num_vertices(g));
	using namespace boost::numeric::ublas;
	matrix<double> dist(num_vertices(g), num_vertices(g));
	
	pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
	
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{	
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 	
	{
		int z = nearest_vert.first;
		ASSERTING(z > -1);
		
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		notInTree = notInTree - z;
		if(notInTree.empty()) break;

		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
			for(vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
				//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
					
				if(new_ecc < curr_ecc)
				{ 
					//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
					near[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());

		// find new z
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			if(near[*vit] > -1) 
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);
					
				double new_ecc = ecc[near[*vit]] + wmap[ep.first];
				if(nearest_vert.second > new_ecc) 
				{
					nearest_vert.first = *vit;
					nearest_vert.second = new_ecc; 
				}
			}
		}

		if(nearest_vert.first < 0) break;
		ASSERTING(nearest_vert.first > -1);
	}

	if(!notInTree.empty()) return false;

	return true;
}

bool mddlOTTC(const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << " treeNodes " << treeNodes << endl;
	
	dEdgeMapConst 		wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN 	oit, oit_end;
	
	vector<int> 		near(num_vertices(g));
	DistanceVector 		ecc(num_vertices(g));
	DistanceVector		degree_bound(num_vertices(g));
	using namespace 	boost::numeric::ublas;
	matrix<double> 		dist(num_vertices(g), num_vertices(g));
	pair<int, double> 	nearest_vert(-1,(std::numeric_limits<double>::max)());
	int 				degreeLeverage = 0;
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(VertexSet::iterator vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}
	
	ASSERTING(nearest_vert.first > -1);

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	VertexSet notInTree = treeNodes - src;

	while(1) 
	{
		int z = nearest_vert.first;
	
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << " out degree : " << getOutDegree(T.g, near[z]) << endl;

		ASSERTING(z > -1);
		ASSERTING(notInTree.contains(z));
		ASSERTING(T.V.contains(near[z]));
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		ASSERTING(degree_bound[near[z]] >= getOutDegree(T.g, near[z]));

		notInTree = notInTree - z;
		if(notInTree.empty()) break;
		
		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
			for(VertexSet::iterator vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
				if(T.V.size() > 1) ASSERTING(ecc[*vit_in] > 0);
				
				//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
				
				if(new_ecc < curr_ecc && getOutDegree(T.g, *vit_in) < degree_bound[*vit_in] ) 
				{ 
					//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
					near[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
		while(nearest_vert.first < 0)
		{	
			VertexSet degreeBroken;	
			// find new z
			for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);

					double dc_near_vit = degree_bound[near[*vit]];
					double od_near_vit = getOutDegree(T.g, near[*vit]);
				
					if(T.V.size() > 1) ASSERTING(od_near_vit > 0);
					
					double new_ecc = ecc[near[*vit]] + wmap[ep.first];
					if(nearest_vert.second > new_ecc && od_near_vit < dc_near_vit) 
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = new_ecc; 
					}

					if(od_near_vit >= dc_near_vit) degreeBroken.insert(near[*vit]);
				}
			}
			if(nearest_vert.first > -1) break;
		
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
	
			//ASSERTING(!degreeBroken.empty());
			if(degreeBroken.empty()) return false;
			if(TreeAlgorithms::relaxDegree(degreeBroken, degree_bound) == false)
				return false;

			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}

	if(!notInTree.empty()) return false;

	return true;
}


/* -------------------------------------------------------------------------
	
				OTTC(): distance and parent

------------------------------------------------------------------------- */

bool OTTC(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << " "  << num_vertices(g) << endl;
	
	VertexSet inTree;
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	
	vector<int> 	near(num_vertices(g));
	DistanceVector 	ecc(num_vertices(g));
	DistanceVector	diameter_bound(num_vertices(g));
	using namespace boost::numeric::ublas;
	matrix<double> 	dist(num_vertices(g), num_vertices(g));
	
	pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
	
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	
		
		diameter_bound[*vit] = D;
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	inTree.insert(src);
	ecc[src] = 0;
	dist(src,src) = 0;
	distance[src] = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) //T.V.size() < treeNodes.size())
	{
		int z = nearest_vert.first;
		
		ASSERTING(z > -1);
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;
		
		inTree.insert(z);
				
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);		
		
		// distance/parent
		distance[z] = g[edge_z_near.first].weight;
		parent[z] = near[z];
		
		notInTree = notInTree - z;
		if(notInTree.empty()) break;

		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		//cerr << WRITE_FUNCTION << " in tree " << T.V << endl;
		
		// set dist(z,u) and ecc(z)
		for(vit = inTree.begin(), vit_end = inTree.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(vit = inTree.begin(), vit_end = inTree.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double weight_ep = (std::numeric_limits<double>::max)();
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			if(near[*vit] > -1) 
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);

				curr_ecc = ecc[near[*vit]] + wmap[ep.first];
				weight_ep = wmap[ep.first];
			}

			if(curr_ecc > diameter_bound[*vit])
			{
				//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
				for(vit_in = inTree.begin(), vit_in_end = inTree.end(); vit_in != vit_in_end; ++vit_in)
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
					if(!ep.second) continue;
					
					double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
					//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
					
					if(new_ecc < curr_ecc)
					{ 
						//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
						near[*vit] = *vit_in;
						curr_ecc = new_ecc;
					}	
				}
			}	
			else
			{
				pair<edge_descriptorN, bool> epz = edge(*vit, z, g);
				if(epz.second)
				{
					//cerr << "Compare w(" << *vit << "," << near[*vit] << ") = " << weight_ep << " to w(" << *vit << "," <<  z << ")= " << wmap[epz.first] << endl;
					if(wmap[epz.first] <= weight_ep)
					{
						//cerr << " new near [" << *vit << "] = " << z << endl;
						near[*vit] = z;
					}
				}
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		// -- end fail safe --
		
		while(nearest_vert.first == -1)
		{	
			bool hit = false;
			VertexSet diameterBroken;
			// find new z
			for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);
				
					if(ecc[near[*vit]] + wmap[ep.first] <= diameter_bound[*vit] && nearest_vert.second > wmap[ep.first])
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = wmap[ep.first]; 
					}
					
					if(ecc[near[*vit]] + wmap[ep.first] > diameter_bound[*vit]) diameterBroken.insert(*vit);
						
					hit = true;
				}
			}
			if(nearest_vert.first > -1) break;
			
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
			
			ASSERTING(hit);
			ASSERTING(!diameterBroken.empty());
			if(diameterBroken.empty()) return false;
			if(TreeAlgorithms::relaxDiameter(diameterBroken, diameter_bound) == false) return false;
			
			// try to increase broken limits
			//for(VertexSet::iterator vit = diameterBroken.begin(), vit_end = diameterBroken.end(); vit != vit_end; ++vit)
			//{
			//	diameter_bound[*vit] = diameter_bound[*vit] + 0.1;
				//cerr << *vit << " increased diameter bound " <<  diameter_bound[*vit] << endl; 
			//}

			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;
	return true;
}

bool dlOTTC(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
//	cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	
	TreeStructure T;
	dEdgeMapConst 		wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN 	oit, oit_end;
	
	vector<int> 		near(num_vertices(g));
	DistanceVector 		ecc(num_vertices(g));
	DistanceVector		diameter_bound(num_vertices(g));
	DistanceVector		degree_bound(num_vertices(g));
	using namespace 	boost::numeric::ublas;
	matrix<double> 		dist(num_vertices(g), num_vertices(g));
	pair<int, double> 	nearest_vert(-1,(std::numeric_limits<double>::max)());
	int 				degreeLeverage = 0;
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	
		
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
		diameter_bound[*vit] = D;
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(VertexSet::iterator vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	distance[src] = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 
	{
		int z = nearest_vert.first;
	
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;

		ASSERTING(z > -1);
		ASSERTING(notInTree.contains(z));
		ASSERTING(T.V.contains(near[z]));
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		ASSERTING(degree_bound[near[z]] >= getOutDegree(T.g, near[z]));
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);		
		
		// distance/parent
		distance[z] = g[edge_z_near.first].weight;
		parent[z] = near[z];
		
		notInTree = notInTree - z;
		if(notInTree.empty()) break;
		
		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		//pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		//ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double dc_near_vit = (numeric_limits<double>::max)(), od_near_vit = 0; 
			if(near[*vit] > -1) 
			{
				dc_near_vit = degree_bound[near[*vit]]; 
				od_near_vit = getOutDegree(T.g, near[*vit]);
			}

			double weight_ep = (std::numeric_limits<double>::max)();
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			if(near[*vit] > -1 && od_near_vit < dc_near_vit) 
			{
				//cerr << " edge (" << *vit << "," << near[*vit] << ")" << endl;
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);

				curr_ecc = ecc[near[*vit]] + wmap[ep.first];
				weight_ep = wmap[ep.first];
			}
		
			//if(curr_ecc > diameter_bound[*vit] || (od_near_vit >= dc_near_vit && getOutDegree(T.g, z) >= degree_bound[z]) )  
			if(curr_ecc > diameter_bound[*vit] || getOutDegree(T.g, z) >= degree_bound[z])
			{
				//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
				for(VertexSet::iterator vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
					if(!ep.second) continue;
					
					double new_ecc = ecc[*vit_in] + wmap[ep.first]; //1;
					//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
				
					if(new_ecc < curr_ecc && getOutDegree(T.g, *vit_in) < degree_bound[*vit_in] )  
					{ 
						//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
						near[*vit] = *vit_in;
						curr_ecc = new_ecc;
					}	
				}
			}	
			else
			{
				pair<edge_descriptorN, bool> epz = edge(*vit, z, g);
				if(epz.second)
				{				
					//cerr << "Compare w(" << *vit << "," << near[*vit] << ") = " << weight_ep << " to w(" << *vit << "," <<  z << ")= " << wmap[epz.first] << endl;
					if(wmap[epz.first] <= weight_ep && getOutDegree(T.g, z) < degree_bound[z])
					{
						//cerr << " new near [" << *vit << "] = " << z << endl;
						near[*vit] = z;
					}
				}
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
		while(nearest_vert.first == -1)
		{	
			VertexSet diameterBroken, degreeBroken;	
			// find new z
			for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);

					double dc_near_vit = degree_bound[near[*vit]];
					double od_near_vit = getOutDegree(T.g, near[*vit]);

					if(ecc[near[*vit]] + wmap[ep.first] <= diameter_bound[*vit] && nearest_vert.second > wmap[ep.first] && od_near_vit < dc_near_vit)
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = wmap[ep.first]; 
					}

					if(ecc[near[*vit]] + wmap[ep.first] > diameter_bound[*vit]) diameterBroken.insert(*vit);
					if(od_near_vit >= dc_near_vit) degreeBroken.insert(near[*vit]);
				}
			}
			if(nearest_vert.first > -1) break;
		
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
			if(!TreeAlgorithms::relaxDegreeAndDiameter(degreeBroken, degree_bound, diameterBroken, diameter_bound)) return false; 
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, diameterBroken)) return false;

			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;
	return true;
}


bool mdOTTC(const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
//	cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;

	TreeStructure T;	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	
	vector<int> near(num_vertices(g));
	DistanceVector ecc(num_vertices(g));
	using namespace boost::numeric::ublas;
	matrix<double> dist(num_vertices(g), num_vertices(g));
	
	pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
	
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{	
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	
		
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	distance[src] = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 	
	{
		int z = nearest_vert.first;
		ASSERTING(z > -1);
		
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << endl;
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);		
		
		// distance/parent
		distance[z] = g[edge_z_near.first].weight;
		parent[z] = near[z];
		
		notInTree = notInTree - z;
		if(notInTree.empty()) break;

		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		//pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		//ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
			for(vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
				//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
					
				if(new_ecc < curr_ecc)
				{ 
					//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
					near[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());

		// find new z
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			if(near[*vit] > -1) 
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);
					
				double new_ecc = ecc[near[*vit]] + wmap[ep.first];
				if(nearest_vert.second > new_ecc) 
				{
					nearest_vert.first = *vit;
					nearest_vert.second = new_ecc; 
				}
			}
		}
		ASSERTING(nearest_vert.first > -1);
	}
	return true;
}

bool mddlOTTC(const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
//	cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	
	TreeStructure T;
	dEdgeMapConst 		wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN 	oit, oit_end;
	
	vector<int> 		near(num_vertices(g));
	DistanceVector 		ecc(num_vertices(g));
	DistanceVector		degree_bound(num_vertices(g));
	using namespace 	boost::numeric::ublas;
	matrix<double> 		dist(num_vertices(g), num_vertices(g));
	pair<int, double> 	nearest_vert(-1,(std::numeric_limits<double>::max)());
	int 				degreeLeverage = 0;
	
	for(VertexSet::iterator vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	

		degree_bound[*vit] = getDegreeConstraint(g, *vit);
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(VertexSet::iterator vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	distance[src] = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 
	{
		int z = nearest_vert.first;
	
		//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << endl; 
		//cerr << WRITE_FUNCTION << " inserting edge (" << z << "," << near[z] << ")" << " out degree : " << getOutDegree(T.g, near[z]) << endl;

		ASSERTING(z > -1);
		ASSERTING(notInTree.contains(z));
		ASSERTING(T.V.contains(near[z]));
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		ASSERTING(degree_bound[near[z]] >= getOutDegree(T.g, near[z]));
		
		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);		
		
		// distance/parent
		distance[z] = g[edge_z_near.first].weight;
		parent[z] = near[z];

		notInTree = notInTree - z;
		if(notInTree.empty()) break;
		
		//cerr << WRITE_FUNCTION << " not in tree " << notInTree << endl;
		
		//pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		//ASSERTING(edge_z_near.second);
		
		// set dist(z,u) and ecc(z)
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		// update dist(near(z), u) and ecc(near(z))
		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		// update other nodes' values of dist and ecc
		for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			ASSERTING(dist(*vit, near[z]) >= 0);
			ASSERTING(dist(*vit, z) >= 0);
			ASSERTING(ecc[*vit] >= 0);

			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		//cerr << " update the near values for other nodes in G " << endl;
		for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			//cerr << " examine all nodes in T to determine near(" << *vit << ")	" << endl;
			for(VertexSet::iterator vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
				if(T.V.size() > 1) ASSERTING(ecc[*vit_in] > 0);
				
				//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
				
				if(new_ecc < curr_ecc && getOutDegree(T.g, *vit_in) < degree_bound[*vit_in] ) 
				{ 
					//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
					near[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		
		// -- fail safe --
		double failsafe = 0;
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
		while(nearest_vert.first == -1)
		{	
			VertexSet degreeBroken;	
			// find new z
			for(VertexSet::iterator vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
			{
				if(near[*vit] > -1) 
				{
					pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
					ASSERTING(ep.second);

					double dc_near_vit = degree_bound[near[*vit]];
					double od_near_vit = getOutDegree(T.g, near[*vit]);
				
					if(T.V.size() > 1) ASSERTING(od_near_vit > 0);
					
					double new_ecc = ecc[near[*vit]] + wmap[ep.first];
					if(nearest_vert.second > new_ecc && od_near_vit < dc_near_vit) 
					{
						//cerr << " new z " << *vit << endl;
						nearest_vert.first = *vit;
						nearest_vert.second = new_ecc; 
					}

					if(od_near_vit >= dc_near_vit) degreeBroken.insert(near[*vit]);
				}
			}
			if(nearest_vert.first > -1) break;
		
			nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());
		
			ASSERTING(!degreeBroken.empty());
			if(degreeBroken.empty()) return false;
			if(TreeAlgorithms::relaxDegree(degreeBroken, degree_bound) == false) return false;

			// try to increase broken limits
			//for(VertexSet::iterator vit = degreeBroken.begin(), vit_end = degreeBroken.end(); vit != vit_end; ++vit)
			//{
			//	degree_bound[*vit] = degree_bound[*vit] + 1;
				//cerr << *vit << " increased degree limit " << degree_bound[*vit] << endl;
			//}
			// -- fail safe --
			if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break;
		}	
	}
	return true;
}
}; // namespace TreeAlgorithms



