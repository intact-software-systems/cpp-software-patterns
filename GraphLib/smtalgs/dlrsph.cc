/***************************************************************************
                          dlrsph.cc  -  description
                             -------------------
    begin                : Thu Jul 26 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "dlrsph.h"
#include "../simtime.h"
#include "../graphalgs/coreh.h"

#include <fstream>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
		Algorithm(): Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
DLRandomizedSPH::Algorithm(vertex_descriptorN zsource, const double &DB)
{
	// -- init --
	D = DB;
	Initialize(zsource);	// identify z-nodes and store them in VertexSet ZVertSet (and vector<MyWrapper> ZVert)
	if(num_zvertices <= 0)
	{
		cout << "[DLRSPHAlgorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << "[DLRSPHAlgorithm] Start DLRSPH zsource " << zsource << " and " << num_zvertices << " znodes" << endl ;

	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		depth[*vit] = 0;
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
	}

	// -- init end --
	
	// -- Start SPH Algorithm --
	T_sph.insertVertex(zsource, g); 					// add source to the SPH tree
	ZVertSet.erase(zsource);
	depth[zsource] = 0;
	vertsWithinDB.insert(zsource);
			
	if(ZVertSet.size() % 2 == 1) // is odd -> then add another vertex
	{
		VertexSet newV;
		GraphAlgorithms::findBestLocatedMemberNodeConnectedToV(inputT, VertexSet(zsource), newV, 1);
		vertex_descriptorN v = *(newV.begin());
		if(inputT.isEdge(v,zsource))
		{
			ASSERTING(inputT.isEdge(v, zsource));
			T_sph.insertVertex(v, g[v]);
			T_sph.insertEdge(v, zsource, g);
			vertsWithinDB.insert(v);
			depth[v] = 0;
			ZVertSet.erase(v);
		}
	}

	RunDijkstraForEveryZ();								// find SP for every z-node and store in SPKeeper

	while(!ZVertSet.empty())
	{	
		VertexSet degreeBroken;
		ShortestPathKeeper spk;
		PathVector newPath; 							// .first contains path to new z-node to be added to T_sph
														// .second is the tree-vertex link -> TODO: probably not needed
		if(ExtractClosestZ(newPath, spk, degreeBroken)) // extract the z-node closest to T_sph
		{
			bool ret = AddTreeSPH(newPath, spk); 		// add the new path to T_sph
			if(false == ret) break;						// tree not valid
		}
		else // failed to add -> loosen constraints
		{
			if(!TreeAlgorithms::relaxDegreeAndRadius(degreeBroken, degree_bound, D)) break; 
	
			//cerr << " increased diameter bound DB: " << D << endl;
			VertexSet newWithinDB;
			VertexSet update = T_sph.V - vertsWithinDB;
			for(VertexSet::iterator vit = update.begin(), vit_end = update.end(); vit != vit_end; ++vit)
			{
				vertex_descriptorN z = *vit;
				if(depth[z] < D/2) 
				{
					vertsWithinDB.insert(z);
					newWithinDB.insert(z);
					//cerr << " update: inserting " << z << " to vertswithin DB sz: " << vertsWithinDB.size() << endl; //: " << vertsWithinDB << " depth[z] " << depth[z] << endl;
				}
			}		
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, newWithinDB)) break;
		}
	}
	
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
DLRandomizedSPH::ExtractClosestZ(PathVector &newPath, ShortestPathKeeper &spk_out, VertexSet &degreeBroken)
{
	double distToTree	= MAXIMUM_WEIGHT; 
	int z_closest = -1, z_in = -1;
	bool found = false;

	// zit - z-node not-in-tree iterator, tit_sph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_sph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		ASSERTING(!T_sph.V.contains(zAddToTree));
		//cerr << WRITE_FUNCTION  << " Z : " << zAddToTree << endl;
		
		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
				
		VertexSet_it tit_sph, tit_sph_end;
		for(tit_sph = vertsWithinDB.begin(), tit_sph_end = vertsWithinDB.end(); tit_sph != tit_sph_end; ++tit_sph) // check distance between new znode zit to every tree node tit_sph
		{
			vertex_descriptorN zInTree = *tit_sph;
			double new_dist = spk.zdistance[zInTree];
			
			PathVector checkPath;
			if(distToTree > new_dist && CheckPath(checkPath, zInTree, zAddToTree, spk.zparent, degreeBroken))	// is the new z-node closer than the previous -> then update distance etc.
			{
				distToTree 	= new_dist;  					// update shortest distance
				z_closest	= zAddToTree;
				z_in		= zInTree;
				newPath     = checkPath;
				found 		= true;
			}
		}

		if(found)
		{
			spk_out = findSPMaps(z_closest);
			break;
		}
	}

	return found;
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
void
DLRandomizedSPH::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
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

bool
DLRandomizedSPH::CheckPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, VertexSet &degreeBroken)
{
	double tree_depth = depth[tree_vert];
		
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	
		bool intact = true;	
		if(getOutDegree(T_sph.g, traverse_vert) >= degree_bound[traverse_vert]) 
		{
			//cerr << traverse_vert << " degree broken " << degree_bound[traverse_vert] << endl;
			degreeBroken.insert(traverse_vert);
			intact = false;
		}

		pair<edge_descriptorN, bool> ep = edge(traverse_vert, zparent[traverse_vert], g);
		ASSERTING(ep.second);
		//cerr << ep.first << " tree_depth[" << traverse_vert << "] = " << tree_depth << " + " << g[ep.first].weight << endl;
	
		if(ZVertSet.contains(traverse_vert) && tree_depth > D/2)
		{
			//cerr << traverse_vert << " diameter broken " << tree_depth << " > " << D/2 << endl;
			intact = false;
		}

		if(!intact) return intact;
		
		tree_depth = tree_depth + g[ep.first].weight;

		newPath.first.push_back(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_back(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);

	return true;
}

/* -------------------------------------------------------------------------

	AddSPHTree(): Returns the vertex that is closest to the source
		if it is not already in the SPH-tree.

	all_vertex_info.first: path (vertex_descriptors) to new z_node 
	all_vertex_info.second: node in the T_sph (steiner or z-node) that 
		links the new z-node to T_sph
 --------------------------------------------------------------------------*/
bool
DLRandomizedSPH::AddTreeSPH(PathVector &newPath, ShortestPathKeeper &spk)
{
	ASSERTING(newPath.first.size() > 1);
	for(ParentVector::iterator vit = newPath.first.begin(), vit_end = newPath.first.end(); vit != vit_end;)
	{
		// iterate
		vertex_descriptorN u = *vit;
		vit++;
		if(vit == vit_end) break;
		vertex_descriptorN v = *vit;
		
		pair<edge_descriptorN, bool> ep = edge(u, v, g);
		ASSERTING(ep.second);
		//cerr << "depth " << v << " = " << depth[u] << " + " << g[ep.first].weight << endl;

		depth[v] = depth[u] + g[ep.first].weight; 
	}

	bool done = false;
	for(ParentVector::reverse_iterator vit = newPath.first.rbegin(), vit_end = newPath.first.rend(); vit != vit_end; )
	{
		// iterate
		vertex_descriptorN u = *vit;
		vit++;
		if(vit == vit_end) break;
		vertex_descriptorN v = *vit;

		//cerr << "adding (" << u << "," << v << ")" << endl;
		if(T_sph.V.contains(v)) done = true;
	
		T_sph.insertEdge(u, v, g); 
		ZVertSet.erase(u); 
		ZVertSet.erase(v);
	
		if(depth[v] <= D/2) vertsWithinDB.insert(v);
		if(depth[u] <= D/2) vertsWithinDB.insert(u);
		
		if(depth[v] > D/2 && GlobalSimArgs::getRelaxDiameter() == 0) return false;
		if(depth[u] > D/2 && GlobalSimArgs::getRelaxDiameter() == 0) return false;

		if(done) break;
	}
	return true;
}


