/***************************************************************************
                          sph.cc  -  description
                             -------------------
    begin                : Thu Sep 14 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "sph.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
		Algorithm(): Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
ShortestPathHeuristic::Algorithm(vertex_descriptorN zsource)
{
	Initialize(zsource);	// identify z-nodes and store them in VertexSet ZVertSet (and vector<MyWrapper> ZVert)
	if(num_zvertices <= 0)
	{
		cout << "[SPHAlgorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << "[SPHAlgorithm] Start SPH zsource " << zsource << " and " << num_zvertices << " znodes" << endl ;
	//dumpGraph(g);

	// -- Start SPH Algorithm --
	T_sph.insertVertex(zsource, g); 					// add source to the SPH tree
	ZVertSet.erase(zsource);

	RunDijkstraForEveryZ();								// find SP for every z-node and store in SPKeeper

	for(int i = 1; i < num_zvertices; i++)	    		// do not count source
	{	
		PathVector newPath; 							// .first contains path to new z-node to be added to T_sph
														// .second is the tree-vertex link -> TODO: probably not needed
		ExtractClosestZ(newPath); 						// extract the z-node closest to T_sph
		AddTreeSPH(newPath); 							// add the new path to T_sph
	}
	
	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_sph.print();
	//T_sph.printVertexState(cerr);
	//if(!T_sph.S.empty()) cerr << "s: " << T_sph.S << endl;
	//ASSERTING(T_sph.S.empty());
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
void
ShortestPathHeuristic::ExtractClosestZ(PathVector &newPath)
{
	double distToTree	= MAXIMUM_WEIGHT; 
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
			
			if(distToTree > new_dist)	// is the new z-node closer than the previous -> then update distance etc.
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
	}
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
void
ShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
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
void
ShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
{
	ParentVector::iterator vit, vit_end, prev_vit = newPath.first.begin();
	for(vit = (newPath.first.begin()+1), vit_end = newPath.first.end(); vit != vit_end; ++vit)
	{
		T_sph.insertVertex(*vit, g);
		T_sph.insertEdge(*vit, *prev_vit, g);
		ZVertSet.erase(*vit); // erase z-node from set of z-nodes

		*prev_vit = *vit;
	}
}

/* -------------------------------------------------------------------------
	
	NB! Below are functions that use Prim MST instead of Dijkstra's SP.


	ExtractClosestZ_prim(): Returns the vertex that is closest to the T_sph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_sph return the closest z-vertex and add the path to the T_sph

 --------------------------------------------------------------------------*/
/*void
ShortestPathHeuristic::ExtractClosestZ_prim(PathVector &newPath) 
{
    iVertexMap idmap 	= get(&VertexProp::id, g);
	//iVertexMap zmap 	= get(&VertexProp::znode, g);
	//iVertexMap stmap	= get(&VertexProp::steiner, g);
	
	bool added_znode	= false;  
	double distToTree	= MAXIMUM_WEIGHT; 
	vertex_descriptorN 	 new_zvertex, link_to_zvertex;
	ShortestPathKeeper 	 spk;
	PathVector temp_newPath; 
	
	vector<MyWrapper>::iterator zit, zit_end, zit_added;		// zit - z-node not-in-tree iterator, tit_sph - tree node iterator
	for(zit = ZVert.begin(), zit_end = ZVert.end(); zit != zit_end; ++zit)		// iterate through the z-vertices and check how close they are to T_sph
	{
		if((*zit).intree == true) continue;			// is *zit already in the T_sph? 
		
		findSPMaps((*zit).v, spk); 
		if(spk.zid < 0) // fail safe
		{	
			cerr << "[SPHExtractClosestZ] findSPMaps returned NULL - exiting " << endl; exit(0);
		} // end fail safe
			
		//set<vertex_descriptorN>::iterator tit_sph, tit_sph_end;
		VertexSet_it tit_sph, tit_sph_end;
		for(tit_sph = T_sph.V.begin(), tit_sph_end = T_sph.V.end(); tit_sph != tit_sph_end; ++tit_sph) // check distance between new znode zit to every tree node tit_sph
		{
			//cerr << "[SPHExtractClosestZ] (" << idmap[*tit_sph] << "," << idmap[(*zit).v] << ")" << endl;
			double returned_distance = FindPathDistance_prim(temp_newPath, *tit_sph, (*zit).v, spk.zparent, spk.zdistance);
			//cerr << "[SPHExtractClosestZ] Distance (" << idmap[*tit_sph] << "," << idmap[(*zit).v] << ") : " << returned_distance << endl;
			
			if(distToTree > returned_distance)	// is the new z-node closer than the previous -> then update distance etc.
			{
				link_to_zvertex		= (*tit_sph); 			// debug 
				distToTree 			= returned_distance;	// update shortest distance
				temp_newPath.second = (*tit_sph);			// sph-vertex that links the new z-vertex to the tree :)
				newPath			 	= temp_newPath;
				new_zvertex			= (*zit).v;
				added_znode 		= true;
				zit_added			= zit;
			}
			
			temp_newPath.first.clear(); // clears vector
		}
	}
	
	if(added_znode)
	{
		//cerr << "[SPHExtractClosestZ] Shortest Distance " << distToTree << " (" << idmap[link_to_zvertex] << "," << idmap[new_zvertex] << ")" << endl;
		(*zit_added).intree = true;		// flag z-vertex as added to tree
		//ZVert.erase(zit_added);  		// remove added z-node from vector
	}		
}
*/
/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from PrimMST(z_vert)

------------------------------------------------------------------------- */
/*double
ShortestPathHeuristic::FindPathDistance_prim(PathVector &newPath, 
						vertex_descriptorN tree_vert, vertex_descriptorN z_vert, ParentVector zparent, DistanceVector zdistance)
{
    iVertexMap idmap 	 = get(&VertexProp::id, g);
	//iVertexMap zmap 	 = get(&VertexProp::znode, g);
	//iVertexMap stmap	 = get(&VertexProp::steiner, g);
	double path_distance = 0;
	vertex_descriptorN 	   traverse_vert = tree_vert;
	//cerr << "[SH::FindDistance] Tree Vertex: ("  << idmap[tree_vert] << "," << idmap[z_vert] << ")" << endl;

	do{	 
		// traverse the path from tree_vert to z_vert and add the vertices to newPath
		path_distance += zdistance[idmap[traverse_vert]];
		newPath.first.push_back(traverse_vert);
		//cerr << "[SH::FindDistance] ("  << idmap[traverse_vert] << "," << zparent[traverse_vert] << ") path_distance " << zdistance[idmap[traverse_vert]] << endl;
		traverse_vert = zparent[traverse_vert];	
	}while(idmap[traverse_vert] != idmap[z_vert]);
	
	newPath.first.push_back(traverse_vert); 		// add last node
	return path_distance;
}

*/


