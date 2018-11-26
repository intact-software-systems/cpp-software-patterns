/***************************************************************************
                          rcdlsph.cc  -  description
                             -------------------
    begin                : Tue Sep 26 2007
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "rcdlsph.h"
#include "../treealgs/dijkstra_sp.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
Algorithm(): Radius Constrained Degree Limited Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
RCDLShortestPathHeuristic::Algorithm(vertex_descriptorN zsource, double radiusBound)
{
	init(radiusBound);	// identify z-nodes and store them in VertexSet ZVertSet (or vector<MyWrapper> ZVert)
	if(num_zvertices <= 0)
	{
		cout << "[RCDLSPH::Algorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << "[RCDLSPH::Algorithm] Start DBSPH zsource " << zsource << " and " << num_zvertices << " znodes and radius bound " << radiusBound << endl;
	
	// -- Start SPH Algorithm --
	T_rcdlsph.insertVertex(zsource, g); 					// add source to the SPH tree
	ZVertSet.erase(zsource);
	RunDijkstraForEveryNode();							// find SP for every z-node and store in SPKeeper

	while(!ZVertSet.empty())
	{
		radiusBroken.clear();	
		degreeBroken.clear();
		PathVector newPath; 							// .first contains path to new z-node to be added to T_bddlsph
														// .second is the tree-vertex link -> TODO: probably not needed
		if(!ExtractClosestZ(newPath, zsource)) 				// extract the z-node closest to T_bddlsph
		{
			if(!TreeAlgorithms::relaxDegreeAndRadius(degreeBroken, degree_bound, radiusBroken, radius_bound)) break; 
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, radiusBroken)) break;
		}
		else AddTreeSPH(newPath); 						// add the new path to T_bddlsph
	}

	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_rcdlsph.print();
	//T_rcdlsph.printVertexState(cerr);
	// -- end debug --
}

/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_rcdlsph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_rcdlsph return the closest z-vertex and add the path to the T_rcdlsph

 --------------------------------------------------------------------------*/
bool
RCDLShortestPathHeuristic::ExtractClosestZ(PathVector &newPath, vertex_descriptorN src)
{
	double distToTree = MAXIMUM_WEIGHT;
	int z_closest = -1, z_in = -1;
	PathVector tempPath;
	
	VertexSet radiusNotBroken;
	// iterate through the z-vertices and check how close they are to T_rcdlsph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		ASSERTING(!T_rcdlsph.V.contains(zAddToTree));
		//cerr << WRITE_FUNCTION  << " Z : " << zAddToTree << endl;
		
		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
				
		VertexSet_it tit_sph, tit_sph_end;
		for(tit_sph = T_rcdlsph.V.begin(), tit_sph_end = T_rcdlsph.V.end(); tit_sph != tit_sph_end; ++tit_sph) 
		{
			vertex_descriptorN zInTree = *tit_sph;
			double new_dist = spk.zdistance[zInTree];
			
			ShortestPathKeeper &spk_inTree = findSPMaps(zInTree); 
			double new_src_dist = spk.zdistance[zInTree] + spk_inTree.zdistance[src];
			
			if(distToTree > new_dist && getOutDegree(T_rcdlsph.g, zInTree) < getDegreeConstraint(g, zInTree) && new_src_dist <= radius_bound[zAddToTree])
			{
				radiusNotBroken.insert(zAddToTree);
				if(FindPath(tempPath, zInTree, zAddToTree, spk.zparent))
				{
					distToTree 	= new_dist;  					// update shortest distance
					z_closest	= zAddToTree;
					z_in		= zInTree;
					newPath		= tempPath;
				}
			}
			tempPath = PathVector();

			if(getOutDegree(T_rcdlsph.g, zInTree) >= degree_bound[zInTree]) degreeBroken.insert(zInTree);
			if(new_src_dist > radius_bound[zAddToTree]) radiusBroken.insert(zAddToTree);
		}
	}
	
	if(distToTree < MAXIMUM_WEIGHT) return true;
	
	radiusBroken = radiusBroken - radiusNotBroken;
	
	return false;
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
bool
RCDLShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	bool intact = true;
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	 
		if(getOutDegree(T_rcdlsph.g, traverse_vert) >= degree_bound[traverse_vert]) 
		{
			degreeBroken.insert(traverse_vert);
			return false;
		}

		newPath.first.push_front(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_front(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);
	return intact;
}

/* -------------------------------------------------------------------------

	AddSPHTree(): Returns the vertex that is closest to the source
		if it is not already in the SPH-tree.

	all_vertex_info.first: path (vertex_descriptors) to new z_node 
	all_vertex_info.second: node in the T_rcdlsph (steiner or z-node) that 
		links the new z-node to T_rcdlsph
 --------------------------------------------------------------------------*/
void 
RCDLShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
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

		if(T_rcdlsph.V.contains(v)) done = true;
		
		T_rcdlsph.insertEdge(u, v, g); 
		
		ZVertSet.erase(u); 
		ZVertSet.erase(v);

		if(done) break;
	}
}

void
RCDLShortestPathHeuristic::init(double radiusBound)
{
	vsVertexMap 	vsmap = get(&VertexProp::vertexState, g);
	
	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{	
		if(vsmap[*vit] == GROUP_MEMBER)
		{
			ZVertSet.insert(*vit);
			num_zvertices++;
		}
		
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
		radius_bound[*vit] = radiusBound;
	}
	ASSERTING(!ZVertSet.empty());
}



