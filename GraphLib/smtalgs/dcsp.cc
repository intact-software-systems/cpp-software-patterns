/***************************************************************************
                          dcsp.cc  -  description
                             -------------------
    begin                : Thu Mar 22 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dcsp.h"
#include "../treealgs/fheap.h"
#include <assert.h>
#include <fstream>
#include "../simtime.h"
#include "../treealgs/treealgs.h"
#include "../graphalgs/graphalgs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
					Initialize all the vectors
------------------------------------------------------------------------- */
void 
DelayConstrainedShortestPath::init()
{
	// init
	zdistanceDCSP_  = DistanceVector(num_vertices(g));
	zcostDCSP_     	= DistanceVector(num_vertices(g));
	zparentDCSP_ 	= ParentVector(num_vertices(g));
	colorDCSP_ 	 	= ColorVector(num_vertices(g));
	zparentZ_ 		= ParentVector(num_vertices(g));
	zdistanceZ_ 	= DistanceVector(num_vertices(g));
	colorZ_			= ColorVector(num_vertices(g));
	zintree_		= BoolVector(num_vertices(g));
	T_dcsp_.g		= GraphN(num_vertices(g));

	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		zdistanceDCSP_[*vit] = (std::numeric_limits<double>::max)();
		zcostDCSP_[*vit]     = (std::numeric_limits<double>::max)();
		zparentDCSP_[*vit] 	 = *vit; 
		colorDCSP_[*vit] 	 = WHITE;
		
		zparentZ_[*vit] 	 = *vit; 
		zdistanceZ_[*vit] 	 = (std::numeric_limits<double>::max)();
		colorZ_[*vit]		 = WHITE;
		
		zintree_[*vit]		 = false;
	}
}

/*-----------------------------------------------------------------------
							Start Function
------------------------------------------------------------------------- */
void
DelayConstrainedShortestPath::Algorithm(vertex_descriptorN zsource, double delayLimit)
{
	Initialize(zsource);	 // identify z-nodes and store them in vector<MyWrapper> ZVert
	
	// -- fail safe --
	if(num_zvertices <= 0) 
	{
		cerr << "[DSCP::Algorithm] Error: No Z-vertices." << endl;	
		ASSERTING(num_zvertices > 0);
	} 
	ASSERTING(delayLimit >= 0); 	// delayLimit can be zero?
	// -- end fail safe -- 
	
	cerr << "[DSCP::Algorithm] Start DCSP with source " << zsource << " and " << num_zvertices << " z-nodes, and delay limit " << delayLimit << endl;
	
	TreeStructure treeDCSP;
	TreeStructure treeZ; 
	init();
	
	// -- Start DSCP Algorithm --
	DCSP(zsource, delayLimit); 								// compute Delay Constrained Shortest Path tree
	if(createTree(zsource, treeDCSP, true, zparentDCSP_)) 	// if tree contains all Z-vertices -> done
	{
		combineTrees(treeDCSP, treeZ);
		
		// -- start debug --
		//cerr << "[DCSP::Algorithm] 1st Produced Steiner Tree: " << endl;
		//T_dcsp_.print();
		// -- end debug --
		return;	
	}
	
	DijkstraZ(zsource);   										// compute Dijkstra for rest of Z-nodes
	if(createTree(zsource, treeZ, false, zparentZ_))			// if tree contains all Z-vertices --> delaylimit is too high
	{
		cerr << "[DCSP::Algorithm] Delay limit too high " << endl;
	}
	
	combineTrees(treeDCSP, treeZ);
	removeLoops(zsource, delayLimit, treeDCSP, treeZ);
	
	// -- start debug --
	//cerr << "[DCSP::Algorithm] 2nd Produced Steiner Tree: " << endl;
	//T_dcsp_.print();
	// -- end debug --
}	
/*-----------------------------------------------------------------------
		After running DCSP or DijkstraZ -> call createTree
------------------------------------------------------------------------- */
bool 
DelayConstrainedShortestPath::createTree(vertex_descriptorN src, TreeStructure& treeStructure, bool zintree, const ParentVector &zparent)
{
	ASSERTING(zparent.size() > 0);
	treeStructure.insertVertex(src, g);
	int numZvertInTree = 1;  	// src alreayd in tree

	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		vertex_descriptorN v = *vit;

		if(v == src) continue;
		
		if(zintree_[v] == zintree)
		{
			//cerr << " Z : " << v << endl;
			ASSERTING(zparent[v] != v);
			numZvertInTree++;
			
			// traverse the path from z_vert to src and add the vertices to treeStructure
			vertex_descriptorN traverse_vert = v;
			do{					
				if(!T_dcsp_.isEdge(traverse_vert, zparent[traverse_vert]))
				{
					pair<edge_descriptorN, bool> e = edge(traverse_vert, zparent[traverse_vert], g);
					ASSERTING(e.second);
			
					treeStructure.insertVertex(traverse_vert, g);
					treeStructure.insertVertex(zparent[traverse_vert], g);
					treeStructure.insertEdge(traverse_vert, zparent[traverse_vert], g);
				}
				traverse_vert = zparent[traverse_vert];
			}while(traverse_vert != src);
		}
	}
	
	//cerr << " numZvertInTree == num_zvertices? " << numZvertInTree << " " << num_zvertices << endl;
	ASSERTING(numZvertInTree <= num_zvertices);
		
	if(numZvertInTree == num_zvertices)	return true; 		// final tree
	
	return false;	// not done
}

/*-----------------------------------------------------------------------
		combine DCSP and Z tree to one tree and graph
------------------------------------------------------------------------- */
void
DelayConstrainedShortestPath::combineTrees(const TreeStructure &treeDCSP, const TreeStructure &treeZ)
{
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(treeDCSP.g); eit != eit_end; ++eit)
	{
		edge_descriptorN se = *eit;
		vertex_descriptorN src = source(se, g), targ = target(se, g);

		// -- debug --
		pair<edge_descriptorN, bool> ep = edge(src, targ, g);
		ASSERTING(ep.second);
		// -- debug end --
		
		if(T_dcsp_.isEdge(src,targ))
		{
			//cerr << " treeDCSP: Found duplicate " << *eit << endl;
		}
		else
		{
			T_dcsp_.insertVertex(src, g);
			T_dcsp_.insertVertex(targ, g);
			T_dcsp_.insertEdge(src, targ, g);
		}
	}

    for(boost::tuples::tie(eit, eit_end) = edges(treeZ.g); eit != eit_end; ++eit)
	{
		edge_descriptorN se = *eit;
		vertex_descriptorN src = source(se, g), targ = target(se, g);

		// -- debug --
		pair<edge_descriptorN, bool> ep = edge(src, targ, g);
		ASSERTING(ep.second);
		// -- debug end --
		
		if(T_dcsp_.isEdge(src,targ))
		{
			//cerr << " treeZ: Found duplicate " << *eit << endl;
		}
		else
		{
			T_dcsp_.insertVertex(src, g);
			T_dcsp_.insertVertex(targ, g);
			T_dcsp_.insertEdge(src, targ, g);
		}
	}
}

/*-----------------------------------------------------------------------
	remove loops that may occur when combining DCSP and Z tree
	 Now: T_dcsp_ contains both treeDCSP and treeZ
------------------------------------------------------------------------- */
void
DelayConstrainedShortestPath::removeLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ)
{	
	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		if(zintree_[*vit] == false)   // for each z in TreeZ (not in TreeDCSP)
		{
			double distance_to_intersection = 0;						
			vertex_descriptorN z = *vit; 
			
			// -- debug/fail safe --
			ASSERTING(treeZ.V.contains(z));
			ASSERTING(!(treeDCSP.V.contains(z)));
			ASSERTING(zparentZ_[z] != z);
			//cerr << " Z: " << z ;
			// -- debug/fail safe end --
			
			// walk towards src using parent map of TreeZ
			// traverse the path from z_vert to src and add the vertices to treeStructure
			vertex_descriptorN traverse_vert = z;
			while((traverse_vert != src) && !(treeDCSP.V.contains(traverse_vert)))
			{
				pair<edge_descriptorN, bool> e = edge(traverse_vert, zparentZ_[traverse_vert], T_dcsp_.g);
				ASSERTING(e.second);
				distance_to_intersection += T_dcsp_.g[e.first].weight;
				traverse_vert = zparentZ_[traverse_vert];				
			} 

			//cerr << " traverse_vert == src " << traverse_vert << " == " << src << "? " << endl;

			if(traverse_vert == src)
			{
				//cerr << " distance_to_src" << distance_to_intersection << " == " << zdistanceZ_[z] << endl;
				//ASSERTING(distance_to_intersection == zdistanceZ_[z]); // found path back to src
				continue; // treeZ has disjoint path to z
			}
			
			// else there is a common vertex in treeZ and treeDCSP -> potential loop
			vertex_descriptorN common_vert = traverse_vert;
			//cerr << " distance_to_common_vert " << distance_to_intersection << endl;
						
			// walk towards src (again) using zparentDCSP_ and zparentZ_ if path to source is different -> loop
			vertex_descriptorN traverseZ = common_vert, traverseDCSP = common_vert;
			while(traverseZ != src && traverseDCSP != src)
			{	
				if(zparentZ_[traverseZ] != zparentDCSP_[traverseDCSP])
				{
					//cerr << traverseZ << " and " << traverseDCSP ;
					//cerr << " differing parents " << zparentZ_[traverseZ] << " != " << zparentDCSP_[traverseDCSP] << endl;
					break;
				}
				
				pair<edge_descriptorN, bool> ez = edge(traverseZ, zparentZ_[traverseZ], T_dcsp_.g);
				pair<edge_descriptorN, bool> edcsp = edge(traverseDCSP, zparentDCSP_[traverseDCSP], T_dcsp_.g);
			
				ASSERTING(ez.second);
				ASSERTING(edcsp.second);
				ASSERTING(ez.first == edcsp.first);
				
				distance_to_intersection += T_dcsp_.g[ez.first].weight;
				
				traverseZ = zparentZ_[traverseZ];
				traverseDCSP = zparentDCSP_[traverseDCSP];
			}
			ASSERTING(traverseZ == traverseDCSP);
			if(traverseZ == src) continue;
			
			vertex_descriptorN intersection = traverseZ; // == traverseDCSP;		
			
			// -- debug --
			//cerr << " delayLimit " << delayLimit ;
			//cerr << " intersection " << intersection ;
			//cerr << " distance_to_intersection " << distance_to_intersection ;
			//cerr << " zdistanceDCSP " << zdistanceDCSP_[intersection] + distance_to_intersection << endl;
			//cerr << " zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit " << zdistanceDCSP_[intersection] + distance_to_intersection << " < " << delayLimit << endl;
			// -- debug end --
			
			// start removing edges either in treeZ or treeDCSP
			if((zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit)  // then remove edges in treeZ
			{
				// -- debug --
				// this should never happen!!!
				cerr << "I thought this never happened!!! " << endl;
				ASSERTING((zdistanceDCSP_[intersection] + distance_to_intersection) >= delayLimit);
				// -- debug end --

				pair<edge_descriptorN, bool> ez = edge(intersection, zparentZ_[intersection], T_dcsp_.g);
				if(ez.second) removeFromTree(zparentZ_, intersection, src);			
			}
			else // remove edges in treeDCSP_
			{
				pair<edge_descriptorN, bool> edcsp = edge(intersection, zparentDCSP_[intersection], T_dcsp_.g);
				if(edcsp.second) removeFromTree(zparentDCSP_, intersection, src);			
			}		
		}		
	}
}

/*-----------------------------------------------------------------------
	Found intersection point in the combined tree, where the paths 
			differ, but they both go to the source
------------------------------------------------------------------------- */
void
DelayConstrainedShortestPath::removeFromTree(const ParentVector &zparent, vertex_descriptorN intersection, vertex_descriptorN src)
{
	vector<edge_descriptorN> loopEdges;

	//cerr << WRITE_FUNCTION << " src " << src << endl;
	vertex_descriptorN traverse_vert = intersection;
	while(traverse_vert != src)	
	{
		pair<edge_descriptorN, bool> e = edge(traverse_vert, zparent[traverse_vert], T_dcsp_.g);
		assert(e.second);
					
		traverse_vert = zparent[traverse_vert];
		loopEdges.push_back(e.first);
		
		if(getOutDegree(traverse_vert, T_dcsp_.g) <= 1 && T_dcsp_.g[traverse_vert].vertexState != GROUP_MEMBER)
		{
			//cerr << "Removing Vertex : " << traverse_vert << endl;
			T_dcsp_.removeVertex(traverse_vert);
		}

		if(T_dcsp_.g[traverse_vert].vertexState == GROUP_MEMBER)	break;
		else if(getOutDegree(traverse_vert, T_dcsp_.g) >= 2) break;
	}				
	
	//cerr << WRITE_FUNCTION << "removing loop edges : " ;
	// remove loop edges from T_dcsp_.E
	vector<edge_descriptorN>::iterator vit = loopEdges.begin(), vit_end = loopEdges.end();
	for( ; vit != vit_end; ++vit)
	{
		//cerr << *vit << ", " ;
		T_dcsp_.removeEdge(*vit);
	}
	//cerr << endl;
}
/*-----------------------------------------------------------------------
DCSP is the same as dijkstra but with delay constraints, and using cost
				as a tie breaker
------------------------------------------------------------------------- */
void 
DelayConstrainedShortestPath::DCSP(vertex_descriptorN src, double delayLimit)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	out_edge_iteratorN oit, oit_end;

	colorDCSP_[src] 	= GRAY;
	zdistanceDCSP_[src] = 0;
	zcostDCSP_[src]     = 0;
	zintree_[src]		= true;
	
	heap->insert(src, 0.0);
	
	while(heap->nItems() > 0)
	{
		vertex_descriptorN u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = target(*oit, g);
			ASSERTING(targ != u);
			
			//cerr << " g[*oit].weight + zdistanceDCSP_[u]) < delayLimit " <<  g[*oit].weight + zdistanceDCSP_[u] << " < " << delayLimit << "?" << endl;
			if(((g[*oit].weight + zdistanceDCSP_[u]) < delayLimit) && colorDCSP_[targ] != BLACK) 
			{
				double new_cost = g[*oit].cost + zcostDCSP_[u];
				if(new_cost < zcostDCSP_[targ])
				{
					zcostDCSP_[targ]     = new_cost;
					zdistanceDCSP_[targ] = g[*oit].weight + zdistanceDCSP_[u];
					zparentDCSP_[targ] 	 = u;
					zintree_[targ]   	 = true;
					
					if(colorDCSP_[targ] == WHITE)
					{
						colorDCSP_[targ] = GRAY;
						heap->insert(targ, zcostDCSP_[targ]);
					}
					else if(colorDCSP_[targ] == GRAY)
					{
						heap->decreaseKey(targ, zcostDCSP_[targ]);
					}
				}
			}
		}
		colorDCSP_[u] = BLACK;
	}	

	delete heap;
}

/*-----------------------------------------------------------------------
	DijkstraZ -> is the same as Dijkstra shortest path
------------------------------------------------------------------------- */
void 
DelayConstrainedShortestPath::DijkstraZ(vertex_descriptorN src)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	out_edge_iteratorN oit, oit_end;

	colorZ_[src] = GRAY;
	zdistanceZ_[src] = 0;
	heap->insert(src, 0.0);
	
	while(heap->nItems() > 0)
	{
		vertex_descriptorN u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = target(*oit, g);
			ASSERTING(targ != u);
			if(((g[*oit].weight + zdistanceZ_[u]) < zdistanceZ_[targ]) && colorZ_[targ] != BLACK)
			{
				zdistanceZ_[targ] = (g[*oit].weight + zdistanceZ_[u]);
				zparentZ_[targ] = u;
				if(colorZ_[targ] == WHITE)
				{
					colorZ_[targ] = GRAY;
					heap->insert(targ, zdistanceZ_[targ]);
				}
				else if(colorZ_[targ] == GRAY)
				{
					heap->decreaseKey(targ, zdistanceZ_[targ]);
				}
			}
		}
		colorZ_[u] = BLACK;
	}	
	delete heap;
}


