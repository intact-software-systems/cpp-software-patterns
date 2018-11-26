/***************************************************************************
                          rcsph.cc  -  description
                             -------------------
    begin                : Tue Sep 26 2007
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "rcsph.h"
#include "../treealgs/treealgs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
	Algorithm(): Radius Constrained Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
RCShortestPathHeuristic::Algorithm(vertex_descriptorN zsource, double radiusBound)
{
	// -- init --
	init(radiusBound);		
	if(num_zvertices <= 0)
	{
		cout << "[RCSPH::Algorithm] Error: No Z-vertices." << endl; ASSERTING(num_zvertices > 0);
	}
	cerr << "[RCSPH::Algorithm] Start RCSPH zsource " << zsource << ", " << num_zvertices << " znodes and radius limit " << radiusBound << endl ;
	// -- init end --
	
	// -- Start RCSPH Algorithm --
	T_rcsph_.insertVertex(zsource, g); 			// add source to the RCSPH tree
	ZVertSet.erase(zsource);

	RunDijkstraForEveryNode();						// find SP for every z-node and store in SPKeeper
	
	// adding low weight radius bounded paths
	while(!ZVertSet.empty())
	{	
		radiusBroken.clear();
		PathVector newPath; 
		if(ExtractClosestZ(newPath, zsource)) 		// extract the z-node closest to T_rcsph_
		{
			AddTree(newPath); 						// add the new path to T_rcsph_
		}
		else 
		{
			if(!TreeAlgorithms::relaxRadius(radiusBroken, radius_bound)) break;
		}
	}
	
	// -- start debug/ fail safe --
	//cerr << WRITE_FUNCTION << " Produced Steiner Tree: " << endl; //T_rcsph_ << endl;
	//T_rcsph_.printGraph();
	//T_rcsph_.printVertexState(cerr);
	//dumpTreeStructure(T_rcsph_);
	//TreeAlgorithms::checkTree(T_rcsph_);
	// -- end debug/ fail safe end --
}


/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_rcsph_
		
		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the v return the closest z-vertex and add the path to the T_rcsph_

 --------------------------------------------------------------------------*/
bool
RCShortestPathHeuristic::ExtractClosestZ(PathVector &newPath, vertex_descriptorN src)
{
	double distToTree	= (std::numeric_limits<double>::max)();
	int z_closest = -1, z_in = -1;

	// iterate through the z-vertices and check how close they are to T_rcsph_
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		if(T_rcsph_.V.contains(zAddToTree)) continue;
		
		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
		for(VertexSet::iterator tit_sph = T_rcsph_.V.begin(), tit_sph_end = T_rcsph_.V.end(); tit_sph != tit_sph_end; ++tit_sph) 
		{	
			vertex_descriptorN zInTree = *tit_sph;
			double new_dist 	= spk.zdistance[zInTree];

			ShortestPathKeeper &spk_inTree = findSPMaps(zInTree); 
			double new_src_dist = spk.zdistance[zInTree] + spk_inTree.zdistance[src];

			if(distToTree > new_dist && new_src_dist <= radius_bound[zAddToTree])		// is the new z-node closer than the previous -> then update cost/distance etc.
			{
				//cerr << zAddToTree << " new closest " << zInTree << " with distance : " << new_dist << " radius: " << new_src_dist << " bound : " << radius_bound[zAddToTree] << endl;

				distToTree 		= new_dist;  			// update distance
				z_closest		= zAddToTree;
				z_in			= zInTree;
			}

			if(new_src_dist > radius_bound[zAddToTree]) radiusBroken.insert(zAddToTree);
		}
	}

	if(distToTree < MAXIMUM_WEIGHT)
	{
		ShortestPathKeeper &spk = findSPMaps(z_closest); 
		FindPath(newPath, z_in, z_closest, spk.zparent);
		newPath.second = z_in;
		return true;
	}

	return false;
}

void
RCShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	 
		newPath.first.push_front(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_front(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);
}

/* -------------------------------------------------------------------------

	AddSPHTree(): Returns the vertex that is closest to the source
		if it is not already in the SPH-tree.

	all_vertex_info.first: path (vertex_descriptors) to new z_node 
	all_vertex_info.second: node in the T_rcsph (steiner or z-node) that 
		links the new z-node to T_rcsph
 --------------------------------------------------------------------------*/
void 
RCShortestPathHeuristic::AddTree(PathVector &newPath)
{
	bool done = false;
	list<vertex_descriptorN>::iterator vit, vit_end; 
	for(vit = newPath.first.begin(), vit_end = newPath.first.end(); vit != vit_end; )
	{
		// iterate
		vertex_descriptorN u = *vit;
		vit++;
		if(vit == vit_end) break;
		vertex_descriptorN v = *vit;

		// -- debug --
		//cerr << "adding (" << u << "," << v << ")" << endl;
		// -- end debug --

		if(T_rcsph_.V.contains(v)) done = true;
		
		T_rcsph_.insertEdge(u, v, g); 
		
		ZVertSet.erase(u); 
		ZVertSet.erase(v);

		if(done) break;
	}
}


/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
/*void
RCShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
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
*/
/* -------------------------------------------------------------------------
		AddTree():
					adds the newPath to the tree
 --------------------------------------------------------------------------*/
/*void
RCShortestPathHeuristic::AddTree(PathVector &newPath)
{
	vsVertexMapConst vsmap	= get(&VertexProp::vertexState, g);
	
	std::vector<int>::iterator vit, vit_end, prev_vit = newPath.first.begin();
	for(vit = (newPath.first.begin()+1), vit_end = newPath.first.end(); vit != vit_end; ++vit)
	{
		T_rcsph_.insertVertex(*vit, g);
		T_rcsph_.insertEdge(*vit, *prev_vit, g);

		ZVertSet.erase(*vit); // erase z-node from set of z-nodes
		ZVertSet.erase(*prev_vit);

		*prev_vit = *vit;
	}
}*/

void
RCShortestPathHeuristic::init(double radiusBound)
{
	vsVertexMapConst vsmap = get(&VertexProp::vertexState, g);
	
	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{	
		if(vsmap[*vit] == GROUP_MEMBER)
		{
			ZVertSet.insert(*vit);
			num_zvertices++;
		}
		
		radius_bound[*vit] = radiusBound;
	}
	ASSERTING(!ZVertSet.empty());
}



