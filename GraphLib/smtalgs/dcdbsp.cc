/***************************************************************************
                          dcdbsp.cc  -  description
                             -------------------
    begin                : Wed Dec 13 2006
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dcdbsp.h"
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
DCDBShortestPath::init()
{
	// init
	zdistanceDCSP_  	= DistanceVector(num_vertices(g));
	zcostDCSP_     		= DistanceVector(num_vertices(g));
	zparentDCSP_ 		= ParentVector(num_vertices(g));
	colorDCSP_ 	 		= ColorVector(num_vertices(g));
	outdegreeDCSP_  	= vector<int>(num_vertices(g));
	undiscoveredDCSP_   = vector<VertexSet>(num_vertices(g)); 
	zparentZ_ 			= ParentVector(num_vertices(g));
	zdistanceZ_ 		= DistanceVector(num_vertices(g));
	colorZ_				= ColorVector(num_vertices(g));
	outdegreeZ_  		= vector<int>(num_vertices(g));
	degboundsZ_     	= vector<int>(num_vertices(g));
	undiscoveredZ_ 		= vector<VertexSet>(num_vertices(g)); 
	zintree_			= BoolVector(num_vertices(g));
	T_dcsp_.g			= GraphN(num_vertices(g));

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

		// DB - stuff
		outdegreeZ_[*vit] 	 = 0;
		degboundsZ_[*vit]    = getDegreeConstraint(g, *vit);
		outdegreeDCSP_[*vit] = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
		{
			undiscoveredZ_[*vit].insert(target(*oit, g));
			undiscoveredDCSP_[*vit].insert(target(*oit, g));
	
			outedgesDCSP_[*vit].insert(pair<double, vertex_descriptorN>(g[*oit].cost, target(*oit, g)));
			outedgesZ_[*vit].insert(pair<double, vertex_descriptorN>(g[*oit].weight, target(*oit, g)));
		}
	}
}

/*-----------------------------------------------------------------------
							Start Function
------------------------------------------------------------------------- */
void
DCDBShortestPath::Algorithm(vertex_descriptorN zsource, double delayLimit)
{
	Initialize(zsource);	 // identify z-nodes and store them in vector<MyWrapper> ZVert
	
	// -- fail safe --
	if(num_zvertices <= 0) 
	{
		cerr << WRITE_FUNCTION << " Error: No Z-vertices." << endl;	
		ASSERTING(num_zvertices > 0);
	} 
	ASSERTING(delayLimit >= 0); 	// delayLimit can be zero?
	// -- end fail safe -- 
	
	cerr << WRITE_FUNCTION << " Start DCDBSP with source " << zsource << " and " << num_zvertices << " z-nodes, and delay limit " << delayLimit << endl;
	
	TreeStructure treeDCSP, treeZ; 
	init();
	
	// -- Start DSCP Algorithm --
	DCDBDijkstraSP(zsource, delayLimit); 					// compute Delay Constrained Shortest Path tree
	if(createTree(zsource, treeDCSP, true, zparentDCSP_)) 	// if tree contains all Z-vertices -> done
	{
		T_dcsp_.mergeTrees(treeDCSP);
		return;	
	}
	
	DBDijkstraZ(zsource);   									// compute Dijkstra for rest of Z-nodes
	if(createTree(zsource, treeZ, false, zparentZ_))			// if tree contains all Z-vertices --> delaylimit is too tight
	{
		cerr << "[DBDCSP::Algorithm] Delay limit is too tight" << endl;
		T_dcsp_.mergeTrees(treeZ);
		return;
	}
	else
	{
		vector<EdgeId> loopEdges;
		VertexSet ZinTreeZ, ZinTreeDCSP;
		for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
		{
			if(zintree_[*vit]) 	ZinTreeDCSP.insert(*vit);
			else 				ZinTreeZ.insert(*vit);
		}
	
		combineTrees(treeDCSP, ZinTreeZ);
		identifyLoops(zsource, delayLimit, treeDCSP, treeZ, loopEdges);
		removeLoopEdges(treeDCSP, treeZ, loopEdges);
	}
}	

/*-----------------------------------------------------------------------
		After running DCSP or DijkstraZ -> call createTree
------------------------------------------------------------------------- */
bool 
DCDBShortestPath::createTree(vertex_descriptorN src, TreeStructure& treeStructure, bool zintree, const ParentVector &zparent)
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
				treeStructure.insertVertex(traverse_vert, g);
				treeStructure.insertVertex(zparent[traverse_vert], g);

				if(!T_dcsp_.isEdge(traverse_vert, zparent[traverse_vert]))
					treeStructure.insertEdge(traverse_vert, zparent[traverse_vert], g);

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
DCDBShortestPath::combineTrees(const TreeStructure &treeDCSP, const VertexSet &setZ)
{
	T_dcsp_.mergeTrees(treeDCSP);
	
	for(VertexSet::iterator vit = setZ.begin(), vit_end = setZ.end(); vit != vit_end; ++vit) // tree holds treeDCSP -> add setZ
	{
		vertex_descriptorN src = *vit, targ = *vit;
		while(src != zparentZ_[src])	
		{
			targ = zparentZ_[src];
			if(!T_dcsp_.isEdge(src,targ))
			{
				T_dcsp_.insertVertex(src, g);
				T_dcsp_.insertVertex(targ, g);
				T_dcsp_.insertEdge(src, targ, g);
			}
			src = targ;
		}
	}
}

/*-----------------------------------------------------------------------
	remove loops that may occur when combining DCSP and Z tree
	 Now: T_dcsp_ contains both treeDCSP and treeZ
------------------------------------------------------------------------- */
void
DCDBShortestPath::identifyLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ, vector<EdgeId> &loopEdges)
{	
	VertexSet ZinTreeZ, ZinTreeDCSP;
	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		if(zintree_[*vit] == false) ZinTreeZ.insert(*vit);
		else 						ZinTreeDCSP.insert(*vit);
	}
	while(!ZinTreeZ.empty())
	{
	VertexSet reV;
	for(VertexSet::iterator vit = ZinTreeZ.begin(), vit_end = ZinTreeZ.end(); vit != vit_end; ++vit)
	{
		vertex_descriptorN z = *vit;
		double distance_to_intersection = 0;						
		
		//cerr << " Z: " << z ;
		
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
			ASSERTING(distance_to_intersection == zdistanceZ_[z]); // found path back to src
			continue; // treeZ has disjoint path to z
		}
		
		// else there is a common vertex in treeZ and treeDCSP -> potential loop
		vertex_descriptorN common_vert = traverse_vert;
		//cerr << " distance_to_common_vert " << distance_to_intersection << endl;
					
		// walk towards src (again) using zparentDCSP_ and zparentZ_ if path to source is different -> loop
		pair<edge_descriptorN, bool> ez, edcsp; 
		vertex_descriptorN traverseZ = common_vert, traverseDCSP = common_vert;
		while(traverseZ != src && traverseDCSP != src)
		{	
			ez = edge(traverseZ, zparentZ_[traverseZ], T_dcsp_.g);
			edcsp = edge(traverseDCSP, zparentDCSP_[traverseDCSP], T_dcsp_.g);

			if(zparentZ_[traverseZ] != zparentDCSP_[traverseDCSP]) 
			{
				//cerr << traverseZ << " and " << traverseDCSP << " differing parents " << zparentZ_[traverseZ] << " != " << zparentDCSP_[traverseDCSP] << endl;
				break;
			}
			//else cerr << traverseZ << " and " << traverseDCSP << " same parents " << zparentZ_[traverseZ] << " == " << zparentDCSP_[traverseDCSP] << endl;
		
			ASSERTING(T_dcsp_.isEdge(traverseZ, zparentZ_[traverseZ]));
			ASSERTING(T_dcsp_.isEdge(traverseDCSP, zparentDCSP_[traverseDCSP]));
		
			distance_to_intersection += T_dcsp_.g[ez.first].weight;
			
			traverseZ = zparentZ_[traverseZ];
			traverseDCSP = zparentDCSP_[traverseDCSP];
		}
		
		ASSERTING(traverseZ == traverseDCSP);
		if(traverseZ == src) continue;
		vertex_descriptorN intersection = traverseZ; 
		
		// -- debug --
		//cerr << "delayLimit " << delayLimit ;cerr << " intersection " << intersection ; cerr << " distance_to_intersection " << distance_to_intersection ;	cerr << " zdistanceDCSP " << zdistanceDCSP_[intersection] + distance_to_intersection << endl;
		//cerr << "zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit " << zdistanceDCSP_[intersection] + distance_to_intersection << " < " << delayLimit << endl;
		// -- debug end --
		
		// start removing edges either in treeZ or treeDCSP
		//if((zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit)  // then remove edges in treeZ
		//{
			//cerr << " removing from treeZ " << endl;
		//	getLoopEdges(zparentZ_, treeDCSP, intersection, src, loopEdges);
		//	reV.insert(zparentDCSP_[intersection]);
		//}
		//else // remove edges in treeDCSP_
		//{
		//	cerr << " removing from treeDCSP " << endl;
			getLoopEdges(zparentDCSP_, treeZ, intersection, src, loopEdges);
			reV.insert(zparentZ_[intersection]);			
		//}		
	}
	ZinTreeZ = reV;
	}
}

/*-----------------------------------------------------------------------
	Found intersection point in the combined tree, where the paths 
			differ, but they both go to the source
------------------------------------------------------------------------- */
void
DCDBShortestPath::getLoopEdges(const ParentVector &zparent, const TreeStructure &tree, vertex_descriptorN intersection, vertex_descriptorN src, vector<EdgeId> &loopEdges)
{
	//cerr << WRITE_FUNCTION << " " << intersection << ": " ;
	vertex_descriptorN traverse_vert = intersection;
	while(traverse_vert != zparent[traverse_vert])	
	{
		//cerr << EdgeId(traverse_vert, zparent[traverse_vert])  << " " ;
		if(!tree.isEdge(traverse_vert, zparent[traverse_vert]) && tree.getOutDegree(traverse_vert) > 0)
		{
			loopEdges.push_back(EdgeId(traverse_vert, zparent[traverse_vert]));
			//cerr << " inserted, " ;
		}
		//else cerr << " skipped, " ;
		
		traverse_vert = zparent[traverse_vert];
		
		if(getOutDegree(traverse_vert, T_dcsp_.g) <= 1 && T_dcsp_.g[traverse_vert].vertexState != GROUP_MEMBER) 
			T_dcsp_.removeVertex(traverse_vert);
	}
	//cerr << endl;
}

/*-----------------------------------------------------------------------
				remove loop edges
------------------------------------------------------------------------- */
void
DCDBShortestPath::removeLoopEdges(const TreeStructure &treeDCSP, const TreeStructure &treeZ, const vector<EdgeId> &loopEdges)
{
	//cerr << WRITE_FUNCTION << " removing : " << endl;
	for(vector<EdgeId>::const_iterator it = loopEdges.begin(), it_end = loopEdges.end(); it != it_end; ++it)
	{
		vertex_descriptorN src = it->first, targ = it->second; 
		ASSERTING(src != targ);
		ASSERTING(!(treeDCSP.isEdge(src, targ) && treeZ.isEdge(src, targ)));
		
		// edge in only one tree 
		if(T_dcsp_.isEdge(src, targ)) 
		{
			T_dcsp_.removeEdge(src, targ);
			//cerr << *it << " " ;
		}
	}
	//cerr << endl;
}

/*-----------------------------------------------------------------------
DCSP is the same as dijkstra but with delay constraints, and using cost
				as a tie breaker
------------------------------------------------------------------------- */
void 
DCDBShortestPath::DCDBDijkstraSP(vertex_descriptorN src, double delayLimit)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	//out_edge_iteratorN oit, oit_end;
	WeightMap::iterator oit, oit_end;

	colorDCSP_[src] 	= GRAY;
	zdistanceDCSP_[src] = 0;
	zcostDCSP_[src]     = 0;
	zintree_[src]		= true;
	
	heap->insert(src, 0.0);

	while(heap->nItems() > 0)
	{
		vertex_descriptorN u = heap->deleteMin();
		//for(tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		
		for(oit = outedgesDCSP_[u].begin(), oit_end = outedgesDCSP_[u].end(); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = oit->second; //target(*oit, g);
			assert(targ != u);

			pair<edge_descriptorN, bool> ep = edge(u, targ, g);
			ASSERTING(ep.second);
			edge_descriptorN e = ep.first;
			
			if(((g[e].weight + zdistanceDCSP_[u]) < delayLimit) && colorDCSP_[targ] != BLACK  && (outdegreeDCSP_[u] < degboundsZ_[u])) 
			{
				double new_cost = g[e].cost + zcostDCSP_[u];
				if(new_cost < zcostDCSP_[targ])
				{	
					// -- new --
					vertex_descriptorN parentv = zparentDCSP_[targ];
					outdegreeDCSP_[parentv] = max(0, outdegreeDCSP_[parentv] - 1);
					outdegreeDCSP_[targ] = max(0, outdegreeDCSP_[targ] - 1);
					// reducing degree constraint -> insert to heap again to rediscover undiscovered nodes
					if(colorDCSP_[parentv] == BLACK && undiscoveredDCSP_[parentv].size()) 
					{
						//cerr << "re-inserting " << parentv << " to heap. Items: " << heap->nItems() << endl;
						heap->insert(parentv, (std::numeric_limits<double>::max)()); 
					}
					undiscoveredDCSP_[u].erase(targ);
					undiscoveredDCSP_[targ].erase(u);
					//cerr << "Old Parent outdegree " << zparent[targ] << " = " << outdegree[zparent[targ]] << endl;
					// -- new end --

					zcostDCSP_[targ]     = new_cost;
					zdistanceDCSP_[targ] = g[e].weight + zdistanceDCSP_[u];
					zparentDCSP_[targ] 	 = u;
					zintree_[targ]   	 = true;
					
					// -- new --
					outdegreeDCSP_[u] = outdegreeDCSP_[u] + 1;
					outdegreeDCSP_[targ] = outdegreeDCSP_[targ] + 1;
					// -- debug --
					//cerr << "New Parent outdegree " << u << " = " << outdegree[u] << endl;
					//cerr << "New Vertex outdegree " << targ << " = " << outdegree[targ] << endl;
					// -- debug end --
					// -- new end --

					if(colorDCSP_[targ] == WHITE)
					{
						colorDCSP_[targ] = GRAY;
						heap->insert(targ, zdistanceDCSP_[targ]);
					}
					else if(colorDCSP_[targ] == GRAY)
					{
						heap->decreaseKey(targ, zdistanceDCSP_[targ]);
					}

				}
			}
			else if( ! (((g[e].weight + zdistanceDCSP_[u]) < delayLimit) && colorDCSP_[targ] != BLACK) )
			{
				double new_cost = g[e].cost + zcostDCSP_[u];
				if( !(new_cost < zcostDCSP_[targ]) )
				{
					undiscoveredDCSP_[u].erase(targ);
					undiscoveredDCSP_[targ].erase(u);
				}
			}
		}
		colorDCSP_[u] = BLACK;
	}	

	delete heap;
}
/*-----------------------------------------------------------------------
	DBDijkstraZ -> is the same as DBDijkstra shortest path
------------------------------------------------------------------------- */
void 
DCDBShortestPath::DBDijkstraZ(vertex_descriptorN src)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	WeightMap::iterator oit, oit_end;
	
	VertexSet intreeV;

	colorZ_[src] = GRAY;
	zdistanceZ_[src] = 0;
	heap->insert(src, 0.0);
	intreeV.insert(src);
	
	while(heap->nItems() > 0)
	{
		while(heap->nItems() > 0)
		{
			vertex_descriptorN u = heap->deleteMin();
			//for(tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
			for(oit = outedgesZ_[u].begin(), oit_end = outedgesZ_[u].end(); oit != oit_end; ++oit)
			{
				vertex_descriptorN targ = oit->second; //target(*oit, g);
				assert(targ != u);

				pair<edge_descriptorN, bool> ep = edge(u, targ, g);
				ASSERTING(ep.second);
				edge_descriptorN e = ep.first;			
				
				if( (g[e].weight + zdistanceZ_[u] < zdistanceZ_[targ]) && (outdegreeZ_[u] < degboundsZ_[u]) && (colorZ_[targ] != BLACK) ) // -> new
				{
					// -- new --
					vertex_descriptorN parentv = zparentZ_[targ];
					outdegreeZ_[parentv] = max(0, outdegreeZ_[parentv] - 1);
					outdegreeZ_[targ] = max(0, outdegreeZ_[targ] - 1);
					// reducing degree constraint -> insert to heap again to rediscover undiscovered nodes
					if(colorZ_[parentv] == BLACK && undiscoveredZ_[parentv].size()) 
					{
						//cerr << "re-inserting " << parentv << " to heap. Items: " << heap->nItems() << endl;
						heap->insert(parentv, (std::numeric_limits<double>::max)()); 
					}
					undiscoveredZ_[u].erase(targ);
					undiscoveredZ_[targ].erase(u);
					//cerr << "Old Parent outdegree " << zparent[targ] << " = " << outdegree[zparent[targ]] << endl;
					// -- new end --
	
					zdistanceZ_[targ] = (g[e].weight + zdistanceZ_[u]);
					zparentZ_[targ] = u;
		
					// -- new --
					intreeV.insert(targ);
					outdegreeZ_[u] = outdegreeZ_[u] + 1;
					outdegreeZ_[targ] = outdegreeZ_[targ] + 1;
					// -- debug --
					//cerr << "New Parent outdegree " << u << " = " << outdegree[u] << endl;
					//cerr << "New Vertex outdegree " << targ << " = " << outdegree[targ] << endl;
					// -- debug end --
					// -- new end --
	
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
				else if( !( (g[e].weight + zdistanceZ_[u] < zdistanceZ_[targ]) && (colorZ_[targ] != BLACK) ) ) // -> new
				{
					undiscoveredZ_[u].erase(targ);
					undiscoveredZ_[targ].erase(u);
				}
			}
			colorZ_[u] = BLACK;
		}	
		
		VertexSet leftV = inputT.V - intreeV;
		if(leftV.empty()) break;
		
		cerr << "Vertices left " << leftV << endl;
		VertexSet incV = leftV;
		for(VertexSet::iterator vit = leftV.begin(), vit_end = leftV.end(); vit != vit_end; ++vit)
		{
			vertex_descriptorN u = *vit;
			ASSERTING(colorZ_[u] != BLACK);

			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
			{
				vertex_descriptorN targ = target(*oit, g);
				if(!incV.contains(targ)) // && !leftV.contains(targ)) 
				{
					//ASSERTING(colorZ_[targ] == GRAY || colorZ_[targ] == BLACK);
					ASSERTING(intreeV.contains(targ));
					degboundsZ_[targ]++;
					heap->insert(targ, zdistanceZ_[targ]);
				}
				incV.insert(targ);				
			}
		}
		
		cerr << "Increased degree bounds for: " << incV - leftV<< endl;
		//char c = getchar();
	}
	
	delete heap;
}

void
DCDBShortestPath::combineTrees(const TreeStructure &treeDCSP, const TreeStructure &treeZ)
{
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(treeDCSP.g); eit != eit_end; ++eit)
	{
		edge_descriptorN se = *eit;
		vertex_descriptorN src = source(se, g), targ = target(se, g);

		if(T_dcsp_.isEdge(src,targ)) continue;
				
		T_dcsp_.insertVertex(src, g);
		T_dcsp_.insertVertex(targ, g);
		T_dcsp_.insertEdge(src, targ, g);
	}

    for(boost::tuples::tie(eit, eit_end) = edges(treeZ.g); eit != eit_end; ++eit)
	{
		edge_descriptorN se = *eit;
		vertex_descriptorN src = source(se, g), targ = target(se, g);

		if(T_dcsp_.isEdge(src,targ)) continue;

		T_dcsp_.insertVertex(src, g);
		T_dcsp_.insertVertex(targ, g);
		T_dcsp_.insertEdge(src, targ, g);
	}
}

	// remove loop edges from treeZ
	/*for(VertexSet::iterator vit = ZinTreeDCSP.begin(), vit_end = ZinTreeDCSP.end(); vit != vit_end; ++vit)
	{
		vertex_descriptorN z = *vit;
		if(z == src) continue; 
		
		vertex_descriptorN traverseZ = z, traverseDCSP = z;
		while(traverseZ != src && traverseDCSP != src)
		{	
			if(zparentZ_[traverseZ] != zparentDCSP_[traverseDCSP]) break;
			
			ASSERTING(T_dcsp_.isEdge(traverseZ, zparentZ_[traverseZ]));
			ASSERTING(T_dcsp_.isEdge(traverseDCSP, zparentDCSP_[traverseDCSP]));
			
			traverseZ = zparentZ_[traverseZ];
			traverseDCSP = zparentDCSP_[traverseDCSP];
		}
		ASSERTING(traverseZ == traverseDCSP);
		if(traverseZ == src) continue;
		vertex_descriptorN intersection = traverseZ; 
		
		// remove from treeZ
		cerr << " Z: " << z << " -> " << traverseZ << " and " << traverseDCSP << " differing parents " << zparentZ_[traverseZ] << " != " << zparentDCSP_[traverseDCSP] << endl;
		cerr << "Identify loop edges in treeZ " << endl;
		getLoopEdges(zparentZ_, treeDCSP, intersection, src, loopEdges);
	}*/
/*
void
DCDBShortestPath::removeLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ)
{	
	VertexSet ZinTreeZ;
	for(VertexSet::iterator vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
		if(zintree_[*vit] == false)   // for each z in TreeZ (not in TreeDCSP)
			ZinTreeZ.insert(*vit);
	
	while(!ZinTreeZ.empty())
	{
		VertexSet remV, reV;
		for(VertexSet::iterator vit = ZinTreeZ.begin(), vit_end = ZinTreeZ.end(); vit != vit_end; ++vit)
		{
			vertex_descriptorN z = *vit;
			//ASSERTING(zintree_[z] == false);   // for each z in TreeZ (not in TreeDCSP)
			if(zparentZ_[z] == z) 
			{
				remV.insert(z);
				continue;
			}

			double distance_to_intersection = 0;						
			
			// -- debug/fail safe --
			//ASSERTING(treeZ.V.contains(z));
			//ASSERTING(!(treeDCSP.V.contains(z)));
			ASSERTING(zparentZ_[z] != z);
			cerr << " Z: " << z ;
			// -- debug/fail safe end --
			
			// walk towards src using parent map of TreeZ
			// traverse the path from z_vert to src and add the vertices to treeStructure
			vertex_descriptorN traverse_vert = z;
			while((traverse_vert != src) && !(treeDCSP.V.contains(traverse_vert)))
			{
				pair<edge_descriptorN, bool> e = edge(traverse_vert, zparentZ_[traverse_vert], T_dcsp_.g);
				ASSERTING(e.second);
				ASSERTING(T_dcsp_.isEdge(traverse_vert, zparentZ_[traverse_vert]));
				
				distance_to_intersection += T_dcsp_.g[e.first].weight;
				traverse_vert = zparentZ_[traverse_vert];				
			} 

			cerr << " traverse_vert == src " << traverse_vert << " == " << src << "? " << endl;
			
			if(traverse_vert == src)
			{
				//cerr << " distance_to_src" << distance_to_intersection << " == " << zdistanceZ_[z] << endl;
				ASSERTING(distance_to_intersection == zdistanceZ_[z]); // found path back to src
				remV.insert(z);
				continue; // treeZ has disjoint path to z
			}
			
			// else there is a common vertex in treeZ and treeDCSP -> potential loop
			vertex_descriptorN common_vert = traverse_vert;
			cerr << " distance_to_common_vert " << distance_to_intersection << endl;
						
			// walk towards src (again) using zparentDCSP_ and zparentZ_ if path to source is different -> loop
			pair<edge_descriptorN, bool> ez, edcsp; 
			vertex_descriptorN traverseZ = common_vert, traverseDCSP = common_vert;
			while(traverseZ != src && traverseDCSP != src)
			{	
				bool differ = false;			
				ez = edge(traverseZ, zparentZ_[traverseZ], T_dcsp_.g);
				edcsp = edge(traverseDCSP, zparentDCSP_[traverseDCSP], T_dcsp_.g);

				if(zparentZ_[traverseZ] != zparentDCSP_[traverseDCSP]) //  && (ez.second && edcsp.second))
				{
					cerr << traverseZ << " and " << traverseDCSP ;
					cerr << " differing parents " << zparentZ_[traverseZ] << " != " << zparentDCSP_[traverseDCSP] << endl;
					differ = true;
				}
				else
				{
					cerr << traverseZ << " and " << traverseDCSP ;
					cerr << " same parents " << zparentZ_[traverseZ] << " == " << zparentDCSP_[traverseDCSP] << endl;
				}
					
				if(!ez.second || !edcsp.second) differ = true;
		
				// -- debug --		
				//if(!ez.second || !edcsp.second)	GraphAlgorithms::checkGraph(T_dcsp_.g);
				//if(!ez.second) cerr << "No edge: (" << traverseZ << "," << zparentZ_[traverseZ] << ")" << endl;
				//if(!edcsp.second) cerr << "No edge: (" << traverseDCSP << "," << zparentDCSP_[traverseDCSP] << ")" << endl;
				ASSERTING(ez.second || edcsp.second);
				// -- debug end --

				if(differ) break;
	
				// -- debug --
				ASSERTING(ez.second);
				ASSERTING(edcsp.second);
				ASSERTING(ez.first == edcsp.first);
				// -- debug end --
				
				distance_to_intersection += T_dcsp_.g[ez.first].weight;
				
				traverseZ = zparentZ_[traverseZ];
				traverseDCSP = zparentDCSP_[traverseDCSP];
			}
			
			ASSERTING(traverseZ == traverseDCSP);
			if(traverseZ == src || (!ez.second || !edcsp.second)) 
			{
				remV.insert(z);
				continue;
			}

			vertex_descriptorN intersection = traverseZ; // == traverseDCSP;		
			
			// -- debug --
			cerr << "delayLimit " << delayLimit ;cerr << " intersection " << intersection ; cerr << " distance_to_intersection " << distance_to_intersection ;	cerr << " zdistanceDCSP " << zdistanceDCSP_[intersection] + distance_to_intersection << endl;
			cerr << "zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit " << zdistanceDCSP_[intersection] + distance_to_intersection << " < " << delayLimit << endl;
			// -- debug end --
			
			// start removing edges either in treeZ or treeDCSP
			if((zdistanceDCSP_[intersection] + distance_to_intersection) < delayLimit)  // then remove edges in treeZ
			{
				// -- debug --
				cerr << "I thought this never happened!!! " << endl;
				// -- debug end --

				pair<edge_descriptorN, bool> ez = edge(intersection, zparentZ_[intersection], T_dcsp_.g);
				ASSERTING(ez.second);
				
				removeFromTree(zparentZ_, intersection, src, reV, treeDCSP, treeZ);
				zparentZ_[intersection] = zparentDCSP_[intersection];

				cerr << intersection << " same parents " << zparentZ_[intersection] << " == " << zparentDCSP_[intersection] << endl;					

				vertex_descriptorN p = zparentZ_[intersection];
				if(zparentZ_[zparentZ_[intersection]] == intersection)
				{
					zparentZ_[p] = zparentDCSP_[p];	
					cerr << "FIXING!! " << p << " same parents " << zparentZ_[p] << " == " << zparentDCSP_[p] << endl;					
					char c = getchar();
				}

				// -- debug --
				pair<edge_descriptorN, bool> ep = edge(intersection, zparentZ_[intersection], T_dcsp_.g);
				ASSERTING(ep.second);
				ASSERTING(zparentZ_[zparentZ_[intersection]] != intersection);
				ASSERTING(zparentDCSP_[zparentDCSP_[intersection]] != intersection);
				// -- debug end --
			}
			else // remove edges in treeDCSP_
			{
				pair<edge_descriptorN, bool> edcsp = edge(intersection, zparentDCSP_[intersection], T_dcsp_.g);
				ASSERTING(edcsp.second);

				removeFromTree(zparentDCSP_, intersection, src, reV);			
				zparentDCSP_[intersection] = zparentZ_[intersection];

				cerr << intersection << " same parents " << zparentZ_[intersection] << " == " << zparentDCSP_[intersection] << endl;

				vertex_descriptorN p = zparentZ_[intersection];
				if(zparentDCSP_[zparentDCSP_[intersection]] == intersection)
				{
					zparentDCSP_[p] = zparentZ_[p];		
					cerr << "FIXING!! " << p << " same parents " << zparentZ_[p] << " == " << zparentDCSP_[p] << endl;					
				}

				// -- debug --
				pair<edge_descriptorN, bool> ep = edge(intersection, zparentZ_[intersection], T_dcsp_.g);
				ASSERTING(ep.second);
				ASSERTING(zparentZ_[zparentZ_[intersection]] != intersection);
				ASSERTING(zparentDCSP_[zparentDCSP_[intersection]] != intersection);
				// -- debug end --
			}		
		}
		ZinTreeZ = ZinTreeZ - remV + reV;
	}
}

void
DCDBShortestPath::removeFromTree(const ParentVector &zparent, vertex_descriptorN intersection, vertex_descriptorN src, VertexSet &reV, const TreeStructure &treeDCSP, const TreeStructure &treeZ)
{
	vector<edge_descriptorN> loopEdges;

	//cerr << WRITE_FUNCTION << " src " << src << endl;
	vertex_descriptorN traverse_vert = intersection;
	while(traverse_vert != src)	
	{
		pair<edge_descriptorN, bool> e = edge(traverse_vert, zparent[traverse_vert], T_dcsp_.g);
		ASSERTING(e.second);
					
		traverse_vert = zparent[traverse_vert];
		loopEdges.push_back(e.first);
		
		if(T_dcsp_.g[traverse_vert].vertexState == GROUP_MEMBER)
			reV.insert(traverse_vert);
		
		if(getOutDegree(traverse_vert, T_dcsp_.g) <= 1 && T_dcsp_.g[traverse_vert].vertexState != GROUP_MEMBER)
		{
			//cerr << "Removing Vertex : " << traverse_vert << endl;
			T_dcsp_.removeVertex(traverse_vert);
			//char c= getchar();
		}

		if(T_dcsp_.g[traverse_vert].vertexState == GROUP_MEMBER)	break;
		else if(getOutDegree(traverse_vert, T_dcsp_.g) >= 2) break;
	}

	// try to continue traversing to the src -> if cannot then .... 
	bool success = true;
	while(traverse_vert != src)	
	{
		if(!T_dcsp_.isEdge(traverse_vert, zparent[traverse_vert]))
		{
			success = false;
			break;
		}
		traverse_vert = zparent[traverse_vert];
	}

	if(!success)
	{
		cerr << " couldn't traverse to the src from " << intersection << endl;
		char c = getchar();
	}
	else
	{
		cerr << WRITE_FUNCTION << " Removing loop edges : " ;
		// remove loop edges from T_dcsp_.E
		vector<edge_descriptorN>::iterator vit = loopEdges.begin(), vit_end = loopEdges.end();
		for( ; vit != vit_end; ++vit)
		{
			cerr << *vit << ", " ;
			T_dcsp_.removeEdge(*vit);
		}
		cerr << endl;
	}
}
*/


