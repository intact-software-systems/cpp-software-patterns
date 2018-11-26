/***************************************************************************
                          prim_rgh_bdmst.cc  -  description
                             -------------------
    begin                : Wed May 30 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "prim_rgh_bdmst.h"
#include "treealgs.h"
#include "../graphalgs/coreh.h"
#include "fheap.h"
#include <vector>

using namespace boost;
using namespace std;

namespace TreeAlgorithms
{

bool RGH(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	
//	cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	ASSERTING(treeNodes.contains(src));

	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	DistanceVector depth(num_vertices(g));
		
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
		depth[*vit] = 0;

	VertexSet notInTree, vertsWithinDB;
	
	// -- begin --
	T.insertVertex(src, g[src]);
	vertsWithinDB.insert(src);
	depth[src] = 0;
	notInTree = treeNodes - src;
	
	if(treeNodes.size() % 2 == 1) // is odd -> then add another vertex
	{
		VertexSet newV;
		GraphAlgorithms::findBestLocatedMemberNodeConnectedToV(Tin, VertexSet(src), newV, 1);
		vertex_descriptorN v = *(newV.begin());
		if(Tin.isEdge(v,src))
		{
			ASSERTING(Tin.isEdge(v,src));
			//cerr << "odd, inserting " << v << endl;
			T.insertVertex(v, g[v]);
			T.insertEdge(v, src, g);
			vertsWithinDB.insert(v);
			depth[v] = 0;
			notInTree = notInTree - v;
		}
	}
	
	// -- fail safe --
	double failsafe = 0;
		
	while(!notInTree.empty()) //T.V.size() < treeNodes.size())
	{
		pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
		int z = -1;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			z = *vit;
			nearest_vert = pair<int, double>(-1,(std::numeric_limits<double>::max)());

			bool found = false;
			for(vit_in = vertsWithinDB.begin(), vit_in_end = vertsWithinDB.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(z, *vit_in, g);
				if(!ep.second) continue;
					
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit_in;
					nearest_vert.second = wmap[ep.first];
					found = true;
				}
			}
			
			if(found) break;
		}
		
		if(nearest_vert.first < 0)
		{
			if(!TreeAlgorithms::relaxRadius(D))	return false;
				
			//cerr << " increased diameter bound DB: " << D << endl;
			VertexSet update = T.V - vertsWithinDB;
			for(vit = update.begin(), vit_end = update.end(); vit != vit_end; ++vit)
			{
				z = *vit;
				if(depth[z] < D/2) 
				{
					vertsWithinDB.insert(z);
					//cerr << " update: inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
				}
			}
		}
		else
		{
			ASSERTING(z > -1);
			ASSERTING(nearest_vert.first > -1);
		
			//cerr << " z " << z << " nearest vert " << nearest_vert.first << endl;
			T.insertVertex(z, g[z]);
			T.insertEdge(z, nearest_vert.first, g);
			notInTree = notInTree - z;
			depth[z] = depth[nearest_vert.first] + nearest_vert.second;
		
			if(depth[z] <= D/2) 
			{
				vertsWithinDB.insert(z);
				//cerr << " inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
			}
			else if(GlobalSimArgs::getRelaxDiameter() == 0) return false;
		}
		
		// -- fail safe --
		if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break; 
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;	
	return true;
}

bool dlRGH(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes)
{
	const GraphN &g = Tin.g;
	
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	ASSERTING(treeNodes.contains(src));
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	dEdgeMapConst 	wmap = get(&EdgeProp::weight, g);
	DistanceVector 	depth(num_vertices(g));
	DistanceVector	degree_bound(num_vertices(g));
		
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		depth[*vit] = 0;
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
	}

	VertexSet notInTree, vertsWithinDB;
	
	// -- begin --
	T.insertVertex(src, g[src]);
	vertsWithinDB.insert(src);
	depth[src] = 0;
	notInTree = treeNodes - src;
	
	if(treeNodes.size() % 2 == 1) // is odd -> then add another vertex
	{
		VertexSet newV;
		GraphAlgorithms::findBestLocatedMemberNodeConnectedToV(Tin, VertexSet(src), newV, 1);
		vertex_descriptorN v = *(newV.begin());
		if(Tin.isEdge(v,src))
		{
			ASSERTING(Tin.isEdge(v,src));
			//cerr << "odd, inserting " << v << endl;
			T.insertVertex(v, g[v]);
			T.insertEdge(v, src, g);
			vertsWithinDB.insert(v);
			depth[v] = 0;
			notInTree = notInTree - v;
		}
	}
	
	// -- fail safe --
	double failsafe = 0;
		
	while(!notInTree.empty()) //T.V.size() < treeNodes.size())
	{
		VertexSet degreeBroken;
		pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
		int z = -1;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			z = *vit;
			nearest_vert = pair<int, double>(-1,(std::numeric_limits<double>::max)());
	
			bool found = false;
			for(vit_in = vertsWithinDB.begin(), vit_in_end = vertsWithinDB.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(z, *vit_in, g);
				if(!ep.second) continue;
					
				if(nearest_vert.second > wmap[ep.first] && getOutDegree(T.g, *vit_in) < degree_bound[*vit]) 
				{
					nearest_vert.first = *vit_in;
					nearest_vert.second = wmap[ep.first];
					found = true;
				}
				if(getOutDegree(T.g, *vit_in) >= degree_bound[*vit]) degreeBroken.insert(*vit);
			}
			
			if(found) break;
		}
		
		if(nearest_vert.first < 0) // == -1
		{
			if(!TreeAlgorithms::relaxDegreeAndRadius(degreeBroken, degree_bound, D)) return false; 

			//cerr << " increased diameter bound DB: " << D << endl;
			VertexSet newWithinDB;
			VertexSet update = T.V - vertsWithinDB;
			for(vit = update.begin(), vit_end = update.end(); vit != vit_end; ++vit)
			{
				z = *vit;
				if(depth[z] < D/2) 
				{
					vertsWithinDB.insert(z);
					newWithinDB.insert(z);
					//cerr << " update: inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
				}
			}
			
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, newWithinDB)) return false;
		}
		else
		{
			ASSERTING(z > -1);
			ASSERTING(nearest_vert.first > -1);
		
			//cerr << " z " << z << " nearest vert " << nearest_vert.first << endl;
			T.insertVertex(z, g[z]);
			T.insertEdge(z, nearest_vert.first, g);
			notInTree = notInTree - z;
			depth[z] = depth[nearest_vert.first] + nearest_vert.second;
		
			if(depth[z] < D/2) 
			{
				vertsWithinDB.insert(z);
				//cerr << " inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
			}
			else if(GlobalSimArgs::getRelaxDiameter() == 0) return false;

		}
		
		// -- fail safe --
		if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) 
		{
			cerr << " ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! fail safe??? " << endl;
			break; 
		}
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;	
	return true;
}

bool RGH(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
	//cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl; //" nodes: " << treeNodes << endl;
	//cerr << " graph : " << endl;
	//dumpGraph(g);
	ASSERTING(treeNodes.contains(src));

	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	DistanceVector depth(num_vertices(g));
		
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	
		depth[*vit] = 0;
	}

	VertexSet notInTree, vertsWithinDB;
	TreeStructure T;

	// -- begin --
	T.insertVertex(src, g[src]);
	vertsWithinDB.insert(src);
	depth[src] = 0;
	distance[src] = 0;
	notInTree = treeNodes - src;
	
	if(treeNodes.size() % 2 == 1) // is odd -> then add another vertex
	{
		VertexSet newV;
		GraphAlgorithms::findBestLocatedMemberNodeConnectedToV(g, treeNodes, VertexSet(src), newV, 1);
		vertex_descriptorN v = *(newV.begin());
		if(TreeAlgorithms::isEdge(g,v,src))
		{
			//cerr << "odd, inserting " << v << endl;
			T.insertVertex(v, g[v]);
			T.insertEdge(v, src, g);
			vertsWithinDB.insert(v);
			depth[v] = 0;
			pair<edge_descriptorN, bool> ep = edge(v, src, g);
			ASSERTING(ep.second);
			distance[v] = g[ep.first].weight;
			parent[v] = src;
			notInTree = notInTree - v;
		}
	}
	
	// -- fail safe --
	double failsafe = 0;
		
	while(!notInTree.empty()) //T.V.size() < treeNodes.size())
	{
		pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
		int z = -1;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			z = *vit;
			nearest_vert = pair<int, double>(-1,(std::numeric_limits<double>::max)());

			bool found = false;
			for(vit_in = vertsWithinDB.begin(), vit_in_end = vertsWithinDB.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(z, *vit_in, g);
				if(!ep.second) continue;
					
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit_in;
					nearest_vert.second = wmap[ep.first];
					found = true;
				}
			}
			
			if(found) break;
		}
		
		if(nearest_vert.first < 0)
		{
			if(!TreeAlgorithms::relaxRadius(D))	return false;
			
			//cerr << " increased diameter bound DB: " << D << endl;
			VertexSet update = T.V - vertsWithinDB;
			for(vit = update.begin(), vit_end = update.end(); vit != vit_end; ++vit)
			{
				z = *vit;
				if(depth[z] < D/2) 
				{
					vertsWithinDB.insert(z);
					//cerr << " update: inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
				}
			}
		}
		else
		{
			ASSERTING(z > -1);
			ASSERTING(nearest_vert.first > -1);
		
			//cerr << " z " << z << " nearest vert " << nearest_vert.first << endl;
			T.insertVertex(z, g[z]);
			T.insertEdge(z, nearest_vert.first, g);
	
			depth[z] = depth[nearest_vert.first] + nearest_vert.second;
			if(depth[z] <= D/2) 
			{
				vertsWithinDB.insert(z);
				//cerr << " inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
			}
			else if(GlobalSimArgs::getRelaxDiameter() == 0) return false;

			pair<edge_descriptorN, bool> ep = edge(z, nearest_vert.first, g);
			ASSERTING(ep.second);
			distance[z] = g[ep.first].weight;
			parent[z] = nearest_vert.first;

			notInTree = notInTree - z;
		}
		
		// -- fail safe --
		if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) break; 
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;	
	return true;
}

bool dlRGH(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes)
{
//	cerr << WRITE_FUNCTION << " src " << src  << " num_vertices " << treeNodes.size() << endl;
	ASSERTING(treeNodes.contains(src));
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	dEdgeMapConst 	wmap = get(&EdgeProp::weight, g);
	DistanceVector 	depth(num_vertices(g));
	DistanceVector	degree_bound(num_vertices(g));
		
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();
		parent[*vit] = *vit;	

		depth[*vit] = 0;
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
	}

	VertexSet notInTree, vertsWithinDB;
	TreeStructure T;

	// -- begin --
	T.insertVertex(src, g[src]);
	vertsWithinDB.insert(src);
	depth[src] = 0;
	distance[src] = 0;

	notInTree = treeNodes - src;
	
	if(treeNodes.size() % 2 == 1) // is odd -> then add another vertex
	{
		VertexSet newV;
		GraphAlgorithms::findBestLocatedMemberNodeConnectedToV(g, treeNodes, VertexSet(src), newV, 1);
		vertex_descriptorN v = *(newV.begin());
		if(isEdge(g,v,src))
		{
			//cerr << "odd, inserting " << v << endl;
			T.insertVertex(v, g[v]);
			T.insertEdge(v, src, g);
			vertsWithinDB.insert(v);
			depth[v] = 0;
			pair<edge_descriptorN, bool> ep = edge(v, src, g);
			ASSERTING(ep.second);
			distance[v] = g[ep.first].weight;
			parent[v] = src;
			notInTree = notInTree - v;
		}
	}
	
	// -- fail safe --
	double failsafe = 0;
		
	while(!notInTree.empty()) //T.V.size() < treeNodes.size())
	{
		VertexSet degreeBroken;
		pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
		int z = -1;
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			z = *vit;
			nearest_vert = pair<int, double>(-1,(std::numeric_limits<double>::max)());
	
			bool found = false;
			for(vit_in = vertsWithinDB.begin(), vit_in_end = vertsWithinDB.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(z, *vit_in, g);
				if(!ep.second) continue;
					
				if(nearest_vert.second > wmap[ep.first] && getOutDegree(T.g, *vit_in) < degree_bound[*vit]) 
				{
					nearest_vert.first = *vit_in;
					nearest_vert.second = wmap[ep.first];
					found = true;
				}
				if(getOutDegree(T.g, *vit_in) >= degree_bound[*vit]) degreeBroken.insert(*vit);
			}
			
			if(found) break;
		}
		
		if(nearest_vert.first < 0) // == -1
		{
			if(!TreeAlgorithms::relaxDegreeAndRadius(degreeBroken, degree_bound, D)) return false; 

			//cerr << " increased diameter bound DB: " << D << endl;
			VertexSet newWithinDB;
			VertexSet update = T.V - vertsWithinDB;
			for(vit = update.begin(), vit_end = update.end(); vit != vit_end; ++vit)
			{
				z = *vit;
				if(depth[z] < D/2) 
				{
					vertsWithinDB.insert(z);
					newWithinDB.insert(z);
					//cerr << " update: inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
				}
			}
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, newWithinDB)) return false;
		}
		else
		{
			ASSERTING(z > -1);
			ASSERTING(nearest_vert.first > -1);
		
			//cerr << " z " << z << " nearest vert " << nearest_vert.first << endl;
			T.insertVertex(z, g[z]);
			T.insertEdge(z, nearest_vert.first, g);

			pair<edge_descriptorN, bool> ep = edge(z, nearest_vert.first, g);
			ASSERTING(ep.second);
			distance[z] = g[ep.first].weight;
			parent[z] = nearest_vert.first;

			depth[z] = depth[nearest_vert.first] + nearest_vert.second;
		
			if(depth[z] < D/2) 
			{
				vertsWithinDB.insert(z);
				//cerr << " inserting " << z << " to vertswithin DB : " << vertsWithinDB << " depth[z] " << depth[z] << endl;
			}
			else if(GlobalSimArgs::getRelaxDiameter() == 0) return false;

			notInTree = notInTree - z;
		}
		
		// -- fail safe --
		if(failsafe++ >= ((std::numeric_limits<double>::max)() - 1)) 
		{
			cerr << " ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! fail safe??? " << endl;
			break; 
		}
	}
	
	//cerr << WRITE_PRETTY_FUNCTION << " tree " << T << endl;	
	return true;
}


}; // namespace TreeAlgorithms

