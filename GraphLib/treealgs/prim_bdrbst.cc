/***************************************************************************
                          prim_bdrbst.cc  -  description
                             -------------------
    begin                : Fri Jun 30 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
#include <assert.h>

#include "prim_bdrbst.h"
//#include "../treealgs/fheap.h"
#include "../simtime.h"
//#include "../treealgs/binheap.h"
#include "../graphalgs/graphalgs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
						class PrimBDRBST
------------------------------------------------------------------------- */
void
PrimBDRBST::Algorithm(const TreeStructure &inputT, vertex_descriptorN zsource, int dl, int bf)
{
	balanceFactor_ = bf;
	degreeLimit_ = dl;
	
	ASSERTING(dl >= 2);
	cerr << "[PrimBDRBST::Algorithm] Start BDRBST with source " << zsource << " degree limit " << degreeLimit_ << " balanceFactor " << balanceFactor_ << endl;

	while(!prim_bdrbst(inputT, zsource)) // can't find tree within degree limit
	{
		if(GlobalSimArgs::getRelaxDegree() <= 0) break;
		
		T_bdrbst_ = TreeStructure();	// clear old tree
	}

	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(T_bdrbst_);
	//dumpGraph(T_bdrbst_.g);
	//char c = getchar();	
	// -- debug end --
}

bool 
PrimBDRBST::prim_bdrbst(const TreeStructure &inputT, vertex_descriptorN src) 
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
	
	typedef pair<int, long double> PairIntDbl;
	typedef vector< PairIntDbl > MinVertexVector;

	// MDDBST Tree
	T_bdrbst_.insertVertex(src, inputT.g);
	V.erase(src);

	int num_vertices = V.size();
	while(T_bdrbst_.V.size() < (num_vertices + 1)) 
	{
		int u = findBalanceVertex(T_bdrbst_.g, heap, rparent, rdistance);
		// find balancFactor (= M) smallest items in min-heap, extract the one with smallest out-degree
		if(u < 0)
		{
			if(TreeAlgorithms::relaxDegree(degreeBroken, degree_bound) == false)
				break;

			//for(VertexSet::iterator vit = degreeBroken.begin(), vit_end = degreeBroken.end(); vit != vit_end; ++vit)
			//{
			//	degree_bound[*vit] = degree_bound[*vit] + 1;
				//cerr << *vit << " increased degree limit " << degree_bound[*vit] << endl;
			//}

			// -- increase degreeLimit_ incrementally if failed to add node -- 
			//degreeLimit_ = degreeLimit_ + 1;
			//cerr << WRITE_FUNCTION << " increased degree limit " << degreeLimit_ << endl;
			//char c = getchar();
		}
		else
		{
			// -- debug --
			//cerr << " Adding edge (" << u << "," << rparent[u] << ") ";
			//cerr << " w: " << inputT.g[ep.first].weight ;
			//cerr << " rdistance: "<< rdistance[u] << endl;
			ASSERTING(!V.contains(rparent[u]));
			ASSERTING(!T_bdrbst_.V.contains(u));
			ASSERTING(rparent[u] != u);
			// -- debug end --

			T_bdrbst_.insertVertex(u, inputT.g);
			T_bdrbst_.insertVertex(rparent[u], inputT.g);
			T_bdrbst_.insertEdge(u, rparent[u], inputT.g);
			V.erase(u);

			//cerr << "foreach vertex v in T minus " << u << " update distances " << endl;
			// update distance for v
			for(VertexSet::iterator vit_in = T_bdrbst_.V.begin(), vit_in_end = T_bdrbst_.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vit_in == u) continue;
				rdistance[*vit_in] = std::max(rdistance[*vit_in], vertexDistance(rdistance, rparent, u, *vit_in, src)); 
			
				// -- debug --
				//cerr << "Max( " << rdistance[*vit_in] << "," << vertex_distance << ")" << endl;
				//cerr << "rdistance " << *vit_in << " = " << rdistance[*vit_in] << endl;
			}
		}
		degreeBroken.clear();

		//cerr << "for each vertex v not in T -> update v's parent if rdistance < MAXIMUM_WEIGHT " << endl;
		for(VertexSet::iterator	vit_in = V.begin(), vit_in_end = V.end(); vit_in != vit_in_end; ++vit_in)
		{
			//cerr << "Not in tree " << *vit_in << ": " << endl;
			rdistance[*vit_in] = MAXIMUM_WEIGHT;
			heap->decreaseKey(*vit_in, rdistance[*vit_in]);
			
			VertexSet::iterator vert_it = T_bdrbst_.V.begin(), vert_it_end = T_bdrbst_.V.end();
			for( ; vert_it != vert_it_end; ++vert_it)
			{	
				pair<edge_descriptorN, bool> ep = edge(*vit_in, *vert_it, inputT.g);
				if(!ep.second) continue;
				edge_descriptorN e = ep.first;				
				vertex_descriptorN targ = target(e, inputT.g);
				ASSERTING(targ == *vert_it);
				
				// -- debug --
				//cerr << e << " inputT.g[e].weight " << inputT.g[e].weight << " targ " << targ << " rdistance[targ] " << rdistance[targ] ;
				//cerr << " inputT.g[e].weight + rdistance[targ] < rdistance[*vit_in] " <<  inputT.g[e].weight + rdistance[targ] << " < " << rdistance[*vit_in] ; // << endl;
				//cerr << " OD: " << getOutDegree(targ, T_bdrbst_.g) << " < " << degreeLimit_ ;
				// -- debug end --
				
				//if(getOutDegree(targ, T_bdrbst_.g) < max(getDegreeConstraint(inputT.g, targ), degreeLimit_) && inputT.g[e].weight + rdistance[targ] < rdistance[*vit_in])
				if(getOutDegree(targ, T_bdrbst_.g) < degree_bound[targ] && inputT.g[e].weight + rdistance[targ] < rdistance[*vit_in])
				{
					rdistance[*vit_in] = inputT.g[e].weight + rdistance[targ];
					rparent[*vit_in] = targ;
					heap->decreaseKey(*vit_in, rdistance[*vit_in]);
					//cerr << " -> Decrease Key " << *vit_in << " rdistance : " << rdistance[*vit_in] ;// << endl;
				}
				if(getOutDegree(targ, T_bdrbst_.g) >= degree_bound[targ]) degreeBroken.insert(targ);
			}
		}
	}
	
	delete heap;

	if(T_bdrbst_.V.size() < num_vertices + 1) return false;

	return true;
}

double
PrimBDRBST::vertexDistance(const DistanceVector &rdistance, const ParentVector &rparent, vertex_descriptorN u, vertex_descriptorN v, vertex_descriptorN zsource)
{
	//cerr << WRITE_FUNCTION << " u " << u << " v " << v << " zsource " << zsource << endl;
	vertex_descriptorN traverse_vert = u;
	double dist_uv = 0;

	GraphN uv_g(rdistance.size());
	//cerr << "1. Traversing from " << u << endl;
	while(traverse_vert != v && traverse_vert != zsource)
	{
		//cerr << "Traverse Vert " << traverse_vert << " parent " << rparent[traverse_vert] << endl; 
		pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_bdrbst_.g);
		assert(ep.second);
		pair<edge_descriptorN, bool> e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
		assert(e.second);
		uv_g[e.first] = T_bdrbst_.g[ep.first];
		
		dist_uv = dist_uv + T_bdrbst_.g[ep.first].weight;
		traverse_vert = rparent[traverse_vert];	
	}
	//cerr << endl;

	assert(dist_uv > 0);
	if(traverse_vert == zsource && traverse_vert != v)
	{
		//cerr << "2. Traversing from " << v << endl;
		traverse_vert = v;
		while(traverse_vert != zsource ) //traverse_vert != u && traverse_vert != zsource)
		{
			//cerr << "Traverse Vert " << traverse_vert << " parent " << rparent[traverse_vert] << endl; 
			assert(traverse_vert != u);
			
			pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], T_bdrbst_.g);
			assert(ep.second);
			pair<edge_descriptorN, bool> e = edge(traverse_vert, rparent[traverse_vert], uv_g);
			if(e.second) // remove
			{
				dist_uv = dist_uv - T_bdrbst_.g[ep.first].weight;
				//remove_edge(traverse_vert, rparent[traverse_vert], uv_g);
			}
			else
			{	
				//e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
				//assert(e.second);
				//uv_g[e.first] = T_bdrbst_.g[ep.first];
				dist_uv = dist_uv + T_bdrbst_.g[ep.first].weight;
			}
			traverse_vert = rparent[traverse_vert];	
			//char c = getchar();
		}
	}
	//cerr << endl;	
	
	ASSERTING(dist_uv < MAXIMUM_WEIGHT);
	
	// -- debug --
	//double debug_dist_uv = 0;
	//edge_iteratorN eit, eit_end;
	//for( tie(eit, eit_end) = edges(uv_g); eit != eit_end; ++eit)
	//	debug_dist_uv = debug_dist_uv + uv_g[*eit].weight;	
	//cerr << "Traverse vert " << traverse_vert << " zsource " << zsource << " v " << v << endl;
	//cerr << WRITE_FUNCTION << " Tree distance (" << u << "," << v << "): " << dist_uv << " debug_dist_uv " << debug_dist_uv << endl;
	// -- debug end --
	
	return dist_uv;
}

int
PrimBDRBST::findBalanceVertex(const GraphN &g, Heap* heap, const ParentVector &rparent, const DistanceVector &rdistance)
{
	typedef pair<int, long double> PairIntDbl;
	typedef vector< PairIntDbl > MinVertexVector;
	MinVertexVector minVector;

	int maxAvailableOD = 0; 	
	PairIntDbl minPid(-1, (numeric_limits<long double>::max)());
	
	// TODO: find balancFactor (= M) smallest items in min-heap, extract the one with smallest out-degree
	for(int i = 0; i < balanceFactor_ && heap->nItems() > 0; i++)
	{
		PairIntDbl pid = heap->deleteMinimum();
		minVector.push_back(pid);
		//cerr << " Pair: " << pid.first << " w: " << pid.second << " rdistance " << rdistance[pid.first] << endl;
	
		// can't connect yet becuase: 1) not yet discovered, 2) no neighbors has available links < degreeLimit_
		if(rdistance[pid.first] == MAXIMUM_WEIGHT) continue; 		
		if(pid.second == MAXIMUM_WEIGHT) break;
	
		//int od = max(getDegreeConstraint(g, rparent[pid.first]), degreeLimit_) - getOutDegree(g, rparent[pid.first]);
		int od = degree_bound[rparent[pid.first]] - getOutDegree(g, rparent[pid.first]);
		
		if(od > maxAvailableOD) 
		{
			maxAvailableOD = od;
			minPid = pid;
			//cerr << "MinPid " << minPid.first << "  " << minPid.second << endl;
		}
	
		// -- debug --
		//cerr << "Available od " << od << endl;
		//cerr << "N items " << heap->nItems() ; //<< endl;
		//cerr << " Pair: " << pid.first << " w: " << pid.second << " rdistance " << rdistance[pid.first] << endl;
		// -- debug end --		
	}
		
	MinVertexVector::iterator mit = minVector.begin(), mit_end = minVector.end();
	for( ; mit != mit_end; ++mit)
	{
		if(minPid.first == mit->first) continue; // do not insert minOD vertex again
		
		heap->insert(mit->first, mit->second);
	}
	
	//cerr << WRITE_FUNCTION << " Return " << minPid.first << endl;
	return minPid.first;
}


