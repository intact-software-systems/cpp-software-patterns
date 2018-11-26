/***************************************************************************
                          dlsph_bauer.cc  -  description
                             -------------------
    begin                : Wed Nov 22 2006
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dlsph_bauer.h"
#include "../treealgs/dijkstra_sp.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
		Algorithm(): Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
DLBauerShortestPathHeuristic::Algorithm(vertex_descriptorN zsource)
{
	Initialize(zsource);	// identify z-nodes and store them in VertexSet ZVertSet (or vector<MyWrapper> ZVert)
	if(num_zvertices <= 0)
	{
		cout << "[DLBauerSPH::Algorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << "[DLBauerSPH::Algorithm] Start DBSPH zsource " << zsource << " and " << num_zvertices << " znodes" ;
	
	// -- Start SPH Algorithm --
	T_sph.insertVertex(zsource, g); 					// add source to the SPH tree
	ZVertSet.erase(zsource);
	RunDijkstraForEveryZ(g);							// find SP for every z-node and store in SPKeeper

	for(int i = 1; i < num_zvertices; i++)	    		// do not count source
	{	
		PathVector newPath; 							// .first contains path to new z-node to be added to T_sph
														// .second is the tree-vertex link -> TODO: probably not needed
		if(!ExtractClosestZ(newPath))
		{
			break;			                            // extract the z-node closest to T_sph
		}
		if(AddTreeSPH(newPath)) 						// add the new path to T_sph
		{
			removeDCNodes();							// check if shortest paths needs to be recalculated because of degree constraints
														// removes degree constrained (full) nodes from GraphN g
			RunDijkstraForEveryZ(g);					// recalculate SP for z-nodes not in tree and store in SPKeeper
			cerr << ".";
		}
		ASSERTING(num_edges(g) > 0);
		// TODO: add check if znodes are still reachable (feasibility test)
	}
	cerr << endl;

	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_sph.print();
	//T_sph.printVertexState(cerr);
	// -- end debug --
}

/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_sph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_sph return the closest z-vertex and add the path to the T_sph

 --------------------------------------------------------------------------*/
bool
DLBauerShortestPathHeuristic::ExtractClosestZ(PathVector &newPath)
{
	double distToTree = MAXIMUM_WEIGHT;
	int z_closest = -1, z_in = -1;
	
	// zit - z-node not-in-tree iterator, tit_sph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_sph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		ASSERTING(!T_sph.V.contains(zAddToTree));
		//cerr << WRITE_FUNCTION  << " Z : " << zAddToTree << endl;
		
		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
				
		VertexSet_it tit_sph, tit_sph_end;
		for(tit_sph = T_sph.V.begin(), tit_sph_end = T_sph.V.end(); tit_sph != tit_sph_end; ++tit_sph) // check distance between new znode zit to every tree node tit_sph
		{
			vertex_descriptorN zInTree = *tit_sph;
			double new_dist = spk.zdistance[zInTree];
			
			if(distToTree > new_dist && getOutDegree(T_sph.g, zInTree) < getDegreeConstraint(g, zInTree))	// is the new z-node closer than the previous -> then update distance etc.
			{
				distToTree 			= new_dist;  					// update shortest distance
				z_closest			= zAddToTree;
				z_in				= zInTree;
			}
		}
	}
	// #1: Run FindPath here
	if(distToTree < MAXIMUM_WEIGHT)
	{
		ShortestPathKeeper &spk = findSPMaps(z_closest); 
		//cerr << "z_closest " << z_closest << " z_in " << z_in << " spk id " << spk.zid << endl;
		FindPath(newPath, z_in, z_closest, spk.zparent);
		newPath.second = z_in;
		return true;
	}
	return false;
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
void
DLBauerShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	 
		newPath.first.push_back(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_back(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);
}

/* -------------------------------------------------------------------------

	AddSPHTree(): Returns the vertex that is closest to the source
		if it is not already in the SPH-tree.

	all_vertex_info.first: path (vertex_descriptors) to new z_node 
	all_vertex_info.second: node in the T_sph (steiner or z-node) that 
		links the new z-node to T_sph
 --------------------------------------------------------------------------*/
bool
DLBauerShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
{
	bool filled_od = false;
	ParentVector::iterator vit, vit_end, prev_vit = newPath.first.begin();
	for(vit = (newPath.first.begin()+1), vit_end = newPath.first.end(); vit != vit_end; ++vit)
	{
		T_sph.insertVertex(*vit, g);
		T_sph.insertEdge(*vit, *prev_vit, g);
		
		ZVertSet.erase(*vit);			// erase connected node from z-vert set
		
		if(getOutDegree(T_sph.g, *vit) >= getDegreeConstraint(g, *vit) || 
		  getOutDegree(T_sph.g, *prev_vit) >= getDegreeConstraint(g, *prev_vit))	
			filled_od = true;

		*prev_vit = *vit;
	}

	return filled_od;
}

bool DLBauerShortestPathHeuristic::removeDCNodes()
{
	bool cleared = false;
	for(VertexSet::iterator vit = T_sph.V.begin(), vit_end = T_sph.V.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(T_sph.g, *vit) >= getDegreeConstraint(g, *vit)) 
		{
			clear_vertex(*vit, g);
			cleared = true;
		}
	}
	return cleared;	
}




