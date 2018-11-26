/***************************************************************************
                          dcsph.cc  -  description
                             -------------------
    begin                : Thu Mar 23 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dcsph.h"
#include "../treealgs/fheap.h"
#include "../treealgs/treealgs.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
	Algorithm(): Delay Constrained Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
DCShortestPathHeuristic::Algorithm(vertex_descriptorN zsource, double delayLimit)
{
	// -- init --
	delayLimit_ = delayLimit;
	Initialize(zsource);	// identify z-nodes and store them in vector<MyWrapper> ZVert
	if(num_zvertices <= 0)
	{
		cout << "[DCSPH::Algorithm] Error: No Z-vertices." << endl; ASSERTING(num_zvertices > 0);
	}
	cerr << "[DCSPH::Algorithm] Start DCSPH zsource " << zsource << ", " << num_zvertices << " znodes and delay limit " << delayLimit_ << endl ;

	int zAdded = 1;	 							// count number of group members (z-points) added
	// -- init end --
	
	// -- Start DCSPH Algorithm --
	T_dcsph_.insertVertex(zsource, g); 			// add source to the DCSPH tree
	ZVertSet.erase(zsource);

	RunDijkstraCDForEveryZ();					// find SP for every z-node and store in DCSPH
	
	// adding low cost delay bounded paths
	for(int i = 1; i < num_zvertices; i++)	
	{	
		// .first contains path to new z-node to be added to T_dcsph_
		// .second is the tree-vertex link -> TODO: probably not needed
		PathVector newPath; 
		ExtractClosestZCost(newPath, zsource); 	// extract the z-node closest to T_dcsph_
		if(newPath.first.size() > 0)
			AddTree(newPath, zAdded); 			// add the new path to T_dcsph_
	}
	
	// adding rest of z nodes through low delay paths
	for(int i = zAdded; i < num_zvertices; i++)	
	{	
		PathVector newPath; 
		ExtractClosestZDist(newPath); 			// extract the z-node closest to T_dcsph_
		if(newPath.first.size() > 0)
			AddTree(newPath, zAdded); 			// add the new path to T_dcsph_
	}
	
	// -- start debug/ fail safe --
	//cerr << WRITE_FUNCTION << " Produced Steiner Tree: " << endl;
	//dumpTreeStructure(T_dcsph_);
	ASSERTING(zAdded == num_zvertices);
	// -- end debug/ fail safe end --
}


/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_dcsph_
		
		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the v return the closest z-vertex and add the path to the T_dcsph_

 --------------------------------------------------------------------------*/
void
DCShortestPathHeuristic::ExtractClosestZCost(PathVector &newPath, vertex_descriptorN src)
{
	double distToTree	= (std::numeric_limits<double>::max)();
	double costToTree   = (std::numeric_limits<double>::max)();
	int z_closest = -1, z_in = -1;

	// zit - z-node not-in-tree iterator, tit_sph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_dcsph_
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		if(T_dcsph_.V.contains(zAddToTree)) continue;
		DCSPH &dcsph = findDCSPH(zAddToTree);
		
		VertexSet::iterator tit_sph, tit_sph_end;
		for(tit_sph = T_dcsph_.V.begin(), tit_sph_end = T_dcsph_.V.end(); tit_sph != tit_sph_end; ++tit_sph) // check distance between new znode zit to every tree node tit_sph
		{	
			vertex_descriptorN zInTree = *tit_sph;
			double new_cost = dcsph.spkCost.zcost[zInTree];
			double new_dist = dcsph.spkCost.zdistance[src];
			
			// is the new z-node closer than the previous -> then update cost/distance etc.
			if(costToTree > new_cost && new_dist < delayLimit_)
			{
				distToTree 		= new_dist;  			// update distance
				costToTree		= new_cost;				// update cost
				z_closest		= zAddToTree;
				z_in			= zInTree;
			}			
		}
	}
	if(distToTree < MAXIMUM_WEIGHT)
	{
		DCSPH &dcsph = findDCSPH(z_closest);
		FindPath(newPath, z_in, z_closest, dcsph.spkCost.zparent);
		newPath.second = z_in;
	}
}

void
DCShortestPathHeuristic::ExtractClosestZDist(PathVector &newPath)
{
	double distToTree	= (std::numeric_limits<double>::max)();
	double costToTree   = (std::numeric_limits<double>::max)();
	int z_closest = -1, z_in = -1;

	// zit - z-node not-in-tree iterator, tit_sph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_dcsph_
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		if(T_dcsph_.V.contains(zAddToTree)) continue;
		DCSPH &dcsph = findDCSPH(zAddToTree);

		VertexSet::iterator tit_sph, tit_sph_end;
		for(tit_sph = T_dcsph_.V.begin(), tit_sph_end = T_dcsph_.V.end(); tit_sph != tit_sph_end; ++tit_sph) // check distance between new znode zit to every tree node tit_sph
		{	
			vertex_descriptorN zInTree = *tit_sph;
			double new_cost = dcsph.spkDistance.zcost[zInTree];
			double new_dist = dcsph.spkDistance.zdistance[zInTree];
					
			if(distToTree > new_dist)	// is the new z-node closer than the previous -> then update distance etc.
			{
				distToTree	= new_dist;  			// update distance
				costToTree	= new_cost;				// update cost
				z_closest 	= zAddToTree;
				z_in		= zInTree;
			}			
		}
	}
	if(distToTree < MAXIMUM_WEIGHT)
	{
		DCSPH &dcsph = findDCSPH(z_closest);
		FindPath(newPath, z_in, z_closest, dcsph.spkDistance.zparent);
		newPath.second = z_in;
	}
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
void
DCShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN 	   traverse_vert = tree_vert;
	do{	 
		newPath.first.push_back(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_back(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);
}

/* -------------------------------------------------------------------------
		AddTree():
					adds the newPath to the tree
 --------------------------------------------------------------------------*/
void
DCShortestPathHeuristic::AddTree(PathVector &newPath, int &zAdded)
{
	vsVertexMapConst vsmap	= get(&VertexProp::vertexState, g);
	
	std::vector<int>::iterator vit, vit_end, prev_vit = newPath.first.begin();
	for(vit = (newPath.first.begin()+1), vit_end = newPath.first.end(); vit != vit_end; ++vit)
	{
		if(vsmap[*vit] == GROUP_MEMBER) zAdded++;
		
		T_dcsph_.insertVertex(*vit, g);
		T_dcsph_.insertEdge(*vit, *prev_vit, g);

		ZVertSet.erase(*vit); // erase z-node from set of z-nodes
		
		*prev_vit = *vit;
	}
}

/*-----------------------------------------------------------------------
			Run DijkstraC and DijkstraD for every Z
------------------------------------------------------------------------- */
void
DCShortestPathHeuristic::RunDijkstraCDForEveryZ()
{
 	DistanceVector		zdistance;
	DistanceVector 		zcost;
	ParentVector		zparent;
	ShortestPathKeeper  spkCost, spkDistance;
	DCSPH				dcsphM;
	
	//cerr << WRITE_FUNCTION << endl;
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		zdistance	= DistanceVector(num_vertices(g));
		zcost     	= DistanceVector(num_vertices(g));
		zparent 	= ParentVector(num_vertices(g));
			
		//cerr << *zit << ", " ;	
		DijkstraC(*zit, zparent, zdistance, zcost);
				
		spkCost = ShortestPathKeeper(zdistance, zparent, zcost, *zit);
		
		zdistance	= DistanceVector(num_vertices(g));
		zcost     	= DistanceVector(num_vertices(g));
		zparent 	= ParentVector(num_vertices(g));

		DijkstraD(*zit, zparent, zdistance, zcost);
				
		spkDistance	= ShortestPathKeeper(zdistance, zparent, zcost, *zit);

		dcsphM 				= DCSPH();
		dcsphM.spkCost 		= spkCost;
		dcsphM.spkDistance 	= spkDistance;
		dcsphM.zid 			= *zit;
		//dcspVector_.push_back(dcsphM);
		dcspMap_[*zit] = dcsphM;
	}
}
/*-----------------------------------------------------------------------
					Dijkstra Least cost
------------------------------------------------------------------------- */
void 
DCShortestPathHeuristic::DijkstraC(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));

	out_edge_iteratorN oit, oit_end;
	ColorVector	color(num_vertices(g));
	
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		color[*vit] 	= WHITE;
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zcost[*vit]     = (std::numeric_limits<double>::max)();
		zparent[*vit]   = *vit;
	}

	color[src] 	   = GRAY;
	zdistance[src] = 0;
	zcost[src]     = 0;
	
	heap->insert(src, 0.0);
	while(heap->nItems() > 0)
	{
		vertex_descriptorN u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = target(*oit, g);
			ASSERTING(targ != u);
			
			double new_cost = g[*oit].cost + zcost[u];
			if(new_cost < zcost[targ])
			{
				zcost[targ]     = new_cost;
				zdistance[targ] = g[*oit].weight + zdistance[u];
				zparent[targ] 	= u;
					
				if(color[targ] == WHITE)
				{
					color[targ] = GRAY;
					heap->insert(targ, zcost[targ]);
				}
				else if(color[targ] == GRAY)
				{
					heap->decreaseKey(targ, zcost[targ]);
				}
			}
		}
		color[u] = BLACK;
	}	
	delete heap;
}

/*-----------------------------------------------------------------------
					Dijkstra shortest path
------------------------------------------------------------------------- */
void DCShortestPathHeuristic::DijkstraD(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost)
{
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g)); //inputT.V.size());
	out_edge_iteratorN oit, oit_end;

	ColorVector	color(num_vertices(g));
	
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		color[*vit] 	= WHITE;
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		zcost[*vit]     = (std::numeric_limits<double>::max)();
		zparent[*vit]   = *vit;
	}

	color[src] 	   = GRAY;
	zdistance[src] = 0;
	zcost[src]     = 0;
	
	heap->insert(src, 0.0);
	while(heap->nItems() > 0)
	{
		vertex_descriptorN u = heap->deleteMin();
		
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			vertex_descriptorN targ = target(*oit, g);
			ASSERTING(targ != u);			
			
			double new_dist = g[*oit].weight + zdistance[u];
			if(new_dist < zdistance[targ])
			{
				zdistance[targ] = new_dist;
				zcost[targ] 	= g[*oit].cost + zcost[u];
				zparent[targ] 	= u;
					
				if(color[targ] == WHITE)
				{
					color[targ] = GRAY;
					heap->insert(targ, zdistance[targ]);
				}
				else if(color[targ] == GRAY)
				{
					heap->decreaseKey(targ, zdistance[targ]);
				}
			}
		}
		color[u] = BLACK;
	}	
	delete heap;
}

void
DCShortestPathHeuristic::findDCSPH(vertex_descriptorN id, DCSPH &dcsph)
{
	dcsph = dcspMap_[id];
}

DCSPH&
DCShortestPathHeuristic::findDCSPH(vertex_descriptorN id)
{
	return dcspMap_[id];
}

