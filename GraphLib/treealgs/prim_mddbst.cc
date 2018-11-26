/***************************************************************************
                          prim_mddbst.cc  -  description
                             -------------------
    begin                : Mon Jun 26 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
#include <assert.h>

#include "prim_mddbst.h"
#include "../treealgs/fheap.h"
#include "../simtime.h"
#include "../treealgs/treealgs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
						class PrimMDDBST
------------------------------------------------------------------------- */
void
PrimMDDBST::Algorithm(const TreeStructure &inputT, vertex_descriptorN zsource, int dl)
{
	degreeLimit_ = dl;
	ASSERTING(degreeLimit_ >= 2);
	
	cerr << "[PrimMDDBST::Algorithm] Start MDDBST with source " << zsource << " degree limit " << degreeLimit_ << endl;
	while(!prim_mddbst(inputT, zsource))
	{
		if(GlobalSimArgs::getRelaxDegree() <= 0) break;

		T_mddbst_ = TreeStructure();
		cerr << "[PrimMDDBST::Algorithm] Starting MDDBST again with source " << zsource << " and increased degree limit " << degreeLimit_ << endl;
	}

	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(T_mddbst_);
	//dumpGraph(T_mddbst_.g);
	//char c = getchar();	
	// -- debug end --
}

bool
PrimMDDBST::prim_mddbst(const TreeStructure &inputT, vertex_descriptorN src)
{
	iVertexMapConst idmap = get(&VertexProp::id, inputT.g);
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(inputT.g));
	
	VertexSet V;
	DistanceVector rdistance(num_vertices(inputT.g));
	ParentVector rparent(num_vertices(inputT.g));
	
	// degree stuff
	degree_bound = vector<int>(num_vertices(inputT.g));
	VertexSet degreeBroken;

	// -- Initalize --
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		degree_bound[*vit] = getDegreeConstraint(inputT.g, *vit);
		pair<edge_descriptorN, bool> ep = edge(src, *vit, inputT.g);
		if(ep.second)
			rdistance[*vit] = inputT.g[ep.first].weight;	
		else if(*vit == src)
			rdistance[*vit] = 0;
		else
			rdistance[*vit] =  (std::numeric_limits<double>::max)();
	
		rparent[*vit] = src; // src is parent of each
		
		if(idmap[*vit] > -1) 
		{
			V.insert(*vit);		
			if(*vit != src)
			{
				//cerr << "Inserting heap " << *vit << " Distance " << rdistance[*vit] << endl;
				heap->insert(*vit, rdistance[*vit]);
			}
		}
	}
	
	// MDDBST Tree
	T_mddbst_.insertVertex(src, inputT.g);
	V.erase(src);

	int num_vertices = V.size();
	while(T_mddbst_.V.size() < (num_vertices + 1)) 
	{
		vertex_descriptorN u = heap->deleteMin();
		pair<edge_descriptorN, bool> ep = edge(u, rparent[u], inputT.g);
	
		// -- increase degreeLimit_ incrementally if failed to add node -- 
		if(!ep.second || (inputT.g[ep.first].weight >= MAXIMUM_WEIGHT) || (rdistance[u] >= MAXIMUM_WEIGHT))
		{
			if(TreeAlgorithms::relaxDegree(degreeBroken, degree_bound) == false)
				break;
			
			//for(VertexSet::iterator vit = degreeBroken.begin(), vit_end = degreeBroken.end(); vit != vit_end; ++vit)
			//{
			//	degree_bound[*vit] = degree_bound[*vit] + 1;
				//cerr << *vit << " increased degree limit " << degree_bound[*vit] << endl;
			//}
				
			//degreeLimit_ = degreeLimit_ + 1;
			heap->insert(u, rdistance[u]);
			//cerr << WRITE_FUNCTION << " increased degree limit " << degreeLimit_ << endl;
			//char c = getchar();
		}
		else
		{
			// -- debug --
			ASSERTING(rparent[u] != u);
			//cerr << " Adding edge (" << u << "," << rparent[u] << ") w: " << rdistance[u] << endl;
			// -- debug end --
			
			T_mddbst_.insertVertex(u, inputT.g);
			T_mddbst_.insertVertex(rparent[u], inputT.g);
			T_mddbst_.insertEdge(u, rparent[u], inputT.g);
			V.erase(u);
		
			//cerr << "foreach vertex v in T minus " << u << " update distances " << endl;
			// update distance for v
			for(VertexSet::iterator vit_in = T_mddbst_.V.begin(), vit_in_end = T_mddbst_.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vit_in == u) continue;
				rdistance[*vit_in] = std::max(rdistance[*vit_in], vertexDistance(rdistance, rparent, u, *vit_in, src)); 

				// -- debug --
				//cerr << "Max( " << rdistance[*vit_in] << "," << vertex_distance << ")" << endl;
				//cerr << "rdistance " << *vit_in << " = " << rdistance[*vit_in] << endl;
				// -- debug end --
			}
		}
		degreeBroken.clear();

		//cerr << "for each vertex v not in T -> update v's parent if rdistance < MAXIMUM_WEIGHT " << endl;
		for(VertexSet::iterator vit_in = V.begin(), vit_in_end = V.end(); vit_in != vit_in_end; ++vit_in)
		{
			//cerr << "Not in tree " << *vit_in << ": " << endl;
			rdistance[*vit_in] = MAXIMUM_WEIGHT;
			heap->decreaseKey(*vit_in, rdistance[*vit_in]);

			//cerr << " Key " << *vit_in << " rdistance " << rdistance[*vit_in] << endl;

			VertexSet::iterator vert_it = T_mddbst_.V.begin(), vert_it_end = T_mddbst_.V.end();
			for( ; vert_it != vert_it_end; ++vert_it)
			{	
				pair<edge_descriptorN, bool> ep = edge(*vit_in, *vert_it, inputT.g);
				if(!ep.second) continue;
				edge_descriptorN e = ep.first;				
				vertex_descriptorN targ = target(e, inputT.g);
				
				// -- debug --
				//cerr << e << " inputT.g[e].weight " << inputT.g[e].weight << " targ " << targ << " rdistance[targ] " << rdistance[targ] << endl;
				//cerr << " inputT.g[e].weight + rdistance[targ] < rdistance[*vit_in] " <<  inputT.g[e].weight + rdistance[targ] << " < " << rdistance[*vit_in] << endl;
				// -- debug end --
				
				//int av_degree = max(getDegreeConstraint(inputT.g, targ), degreeLimit_) - getOutDegree(targ, T_mddbst_.g);
				int av_degree = degree_bound[targ] - getOutDegree(targ, T_mddbst_.g);
				ASSERTING(av_degree >= 0);
				if(av_degree > 0 && inputT.g[e].weight + rdistance[targ] < rdistance[*vit_in])
				{
					rdistance[*vit_in] = inputT.g[e].weight + rdistance[targ];
					rparent[*vit_in] = targ;
					heap->decreaseKey(*vit_in, rdistance[*vit_in]);
					//cerr << "Decrease Key " << *vit_in << " rdistance : " << rdistance[*vit_in] << endl;
				}
				if(av_degree <= 0) degreeBroken.insert(targ);
			}
		}
	}
	
	delete heap;

	if(T_mddbst_.V.size() < num_vertices + 1) return false;

	return true;
}
double
PrimMDDBST::vertexDistance(const DistanceVector &rdistance, const ParentVector &rparent, vertex_descriptorN u, vertex_descriptorN v, vertex_descriptorN zsource)
{
	vertex_descriptorN traverse_vert = u;
	double dist_uv = 0;
	GraphN uv_g; 
	
	while(traverse_vert != v && traverse_vert != zsource)
	{
		pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_mddbst_.g);
		ASSERTING(ep.second);
		pair<edge_descriptorN, bool> e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
		ASSERTING(e.second);
		
		dist_uv = dist_uv + T_mddbst_.g[ep.first].weight;
		traverse_vert = rparent[traverse_vert];	
	}

	ASSERTING(dist_uv > 0);
	if(traverse_vert == zsource && traverse_vert != v)
	{
		traverse_vert = v;
		while(traverse_vert != zsource ) 
		{
			ASSERTING(traverse_vert != u);
			
			pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_mddbst_.g);
			ASSERTING(ep.second);
			if(isEdge(uv_g, traverse_vert, rparent[traverse_vert]))
			{
				dist_uv = dist_uv - T_mddbst_.g[ep.first].weight;
			}
			else
			{	
				dist_uv = dist_uv + T_mddbst_.g[ep.first].weight;
			}
			traverse_vert = rparent[traverse_vert];	
		}
	}
	return dist_uv;
}

/*
double
PrimMDDBST::vertexDistance(const DistanceVector &rdistance, const ParentVector &rparent, vertex_descriptorN u, vertex_descriptorN v, vertex_descriptorN zsource)
{
	//cerr << WRITE_FUNCTION << " u " << u << " v " << v << " zsource " << zsource << endl;
	vertex_descriptorN traverse_vert = u;
	double dist_uv = 0;

	GraphN uv_g; //(rdistance.size());
	//cerr << "1. Traversing from " << u << endl;
	while(traverse_vert != v && traverse_vert != zsource)
	{
		//cerr << "Traverse Vert " << traverse_vert << " parent " << rparent[traverse_vert] << endl; 
		pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_mddbst_.g);
		ASSERTING(ep.second);
		pair<edge_descriptorN, bool> e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
		ASSERTING(e.second);
		//uv_g[e.first] = T_mddbst_.g[ep.first];
		
		dist_uv = dist_uv + T_mddbst_.g[ep.first].weight;
		traverse_vert = rparent[traverse_vert];	
	}
	//cerr << endl;

	ASSERTING(dist_uv > 0);
	if(traverse_vert == zsource && traverse_vert != v)
	{
		//cerr << "2. Traversing from " << v << endl;
		traverse_vert = v;
		while(traverse_vert != zsource ) //traverse_vert != u && traverse_vert != zsource)
		{
			//cerr << "Traverse Vert " << traverse_vert << " parent " << rparent[traverse_vert] << endl; 
			ASSERTING(traverse_vert != u);
			
			pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_mddbst_.g);
			ASSERTING(ep.second);
			//pair<edge_descriptorN, bool> e = edge(traverse_vert, rparent[traverse_vert], uv_g);
			//if(e.second) // remove
			if(isEdge(uv_g, traverse_vert, rparent[traverse_vert]))
			{
				dist_uv = dist_uv - T_mddbst_.g[ep.first].weight;
				//remove_edge(traverse_vert, rparent[traverse_vert], uv_g);
			}
			else
			{	
				//e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
				//ASSERTING(e.second);
				//uv_g[e.first] = T_mddbst_.g[ep.first];
				dist_uv = dist_uv + T_mddbst_.g[ep.first].weight;
			}
			traverse_vert = rparent[traverse_vert];	
			//char c = getchar();
		}
	}
	//cerr << endl;	
	
	// -- debug --
	//double debug_dist_uv = 0;
	//edge_iteratorN eit, eit_end;
	//for( tie(eit, eit_end) = edges(uv_g); eit != eit_end; ++eit)
	//	debug_dist_uv = debug_dist_uv + uv_g[*eit].weight;	
	//cerr << "Traverse vert " << traverse_vert << " zsource " << zsource << " v " << v << endl;
	//cerr << WRITE_FUNCTION << " Tree distance (" << u << "," << v << "): " << dist_uv << endl; //" debug_dist_uv " << debug_dist_uv << endl;
	// -- debug end --
	
	return dist_uv;
}

*/

