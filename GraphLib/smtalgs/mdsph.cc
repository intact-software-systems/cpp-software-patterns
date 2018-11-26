/***************************************************************************
                          mdsph.cc  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "mdsph.h"
#include <fstream>
#include "../simtime.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
	Algorithm(): Minimum Diameter Shortest Path Heuristic start function

------------------------------------------------------------------------- */
void
MDShortestPathHeuristic::Algorithm(vertex_descriptorN zsource)
{
	init(zsource);	// identify z-nodes and store them in VertexSet ZVertSet (and vector<MyWrapper> ZVert)
	if(num_zvertices <= 0)
	{
		cout << "[MDSPHAlgorithm] Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << "[MDSPHAlgorithm] Start SPH zsource " << zsource << " and " << num_zvertices << " znodes" << endl ;
	//dumpGraph(g);

	// -- Start SPH Algorithm --
	T_mdsph.insertVertex(zsource, g); 					// add source to the SPH tree
	ZVertSet.erase(zsource);

	RunDijkstraForEveryZ();								// find SP for every z-node and store in SPKeeper

	while(!ZVertSet.empty())
	{	
		int z = -1;
		PathVector newPath; 							// .first contains path to new z-node to be added to T_mdsph
														// .second is the tree-vertex link -> TODO: probably not needed
		ExtractClosestZ(newPath, z); 					// extract the z-node closest to T_mdsph
		if(newPath.first.empty()) break;
		
		AddTreeSPH(newPath); 							// add the new path to T_mdsph
		updateDist(z);
	}
	
	// -- start debug --
	//if(T_mdsph.V.size() <= T_mdsph.E.size())
	//{
	//	cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << T_mdsph << endl;
	//	T_mdsph.printVertexState(cerr);
	//}
	// -- end debug --
}

/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_mdsph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_mdsph return the closest z-vertex and add the path to the T_mdsph

 --------------------------------------------------------------------------*/
void
MDShortestPathHeuristic::ExtractClosestZ(PathVector &newPath, int &z_closest)
{
	double diameterTree	= MAXIMUM_WEIGHT; //(numeric_limits<double>::max)(); 
	int z_in = -1;
	
	// zit - z-node not-in-tree iterator, tit_mdsph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_mdsph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		ASSERTING(!T_mdsph.V.contains(*zit));
		//cerr << WRITE_FUNCTION  << " Z : " << *zit << endl;

		vertex_descriptorN zAddToTree = *zit;
		ShortestPathKeeper &spk = findSPMaps(zAddToTree);
			
		VertexSet_it tit_mdsph, tit_mdsph_end;
		for(tit_mdsph = T_mdsph.V.begin(), tit_mdsph_end = T_mdsph.V.end(); tit_mdsph != tit_mdsph_end; ++tit_mdsph) // check distance between new znode zit to every tree node tit_mdsph
		{
			vertex_descriptorN zInTree = *tit_mdsph;
			double new_diameter = ecc[zInTree] + spk.zdistance[zInTree];
			
			if(T_mdsph.V.size() > 1) ASSERTING(ecc[zInTree] > 0);
			ASSERTING(spk.zdistance[zInTree] > 0);

			// is the new z-node closer than the previous -> then update distance etc.
			if(diameterTree > new_diameter)
			{
				//cerr << zAddToTree << " new connect node " << zInTree << " diameter " << new_diameter << endl;
				diameterTree		= new_diameter;
				z_closest			= zAddToTree;
				z_in				= zInTree;
			}
		}
	}
	// #1: Run FindPath here
	if(diameterTree < MAXIMUM_WEIGHT)
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
MDShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
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
	all_vertex_info.second: node in the T_mdsph (steiner or z-node) that 
		links the new z-node to T_mdsph
 --------------------------------------------------------------------------*/
void
MDShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
{
	ASSERTING(newPath.first.size() > 1);
	for(list<vertex_descriptorN>::reverse_iterator vit = newPath.first.rbegin(), vit_end = newPath.first.rend(); vit != vit_end; )
	{
		// iterate
		vertex_descriptorN u = *vit;
		vit++;
		if(vit == vit_end) break;
		vertex_descriptorN v = *vit;
		
		pair<edge_descriptorN, bool> ep = edge(u, v, g);
		ASSERTING(ep.second);

		ASSERTING(near_[u] > -1);
	
		//cerr << "1 (" << u << "," << v << ")" << " near[" << u << "] " << near[u] << " near[" << v << "] " << near[v] << " ecc[" << v << "] " << ecc[v] << endl;

		if(near_[v] < 0) 
		{
			near_[v] = u;
			ecc[v] = ecc[u] + g[ep.first].weight;
		}
	
		//cerr << "2 (" << u << "," << v << ")" << " near[" << u << "] " << near[u] << " near[" << v << "] " << near[v] << " ecc[" << v << "] " << ecc[v] << endl;
	}

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

		if(T_mdsph.V.contains(v)) done = true;
		
		T_mdsph.insertEdge(u, v, g); 
		
		ZVertSet.erase(u); 
		ZVertSet.erase(v);

		if(done) break;
	}
}

void
MDShortestPathHeuristic::updateDist(vertex_descriptorN z)
{
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	ShortestPathKeeper &spk = findSPMaps(z); 
	
	//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << " spk.zdistance[near[z]]" << spk.zdistance[near[z]] << endl;
	ASSERTING(near_[z] > -1);
	ASSERTING(ecc[near_[z]] >= 0);

	// set dist(z,u) and ecc(z)
	for(vit = T_mdsph.V.begin(), vit_end = T_mdsph.V.end(); vit != vit_end; ++vit)
	{
		if(dist(near_[z],*vit) > 0) dist(z,*vit) = dist(near_[z],*vit) + spk.zdistance[near_[z]]; 
	}
	
	dist(z,z) = 0;
	ecc[z] = ecc[near_[z]] + spk.zdistance[near_[z]]; 
	
	// update dist(near(z), u) and ecc(near(z))
	dist(near_[z],z) = spk.zdistance[near_[z]];
	if(ecc[near_[z]] <= 0) ecc[near_[z]] = spk.zdistance[near_[z]]; 
	
	// update other nodes' values of dist and ecc
	for(vit = T_mdsph.V.begin(), vit_end = T_mdsph.V.end(); vit != vit_end; ++vit)
	{
		ASSERTING(dist(*vit, near_[z]) >= 0);
		ASSERTING(dist(*vit, z) >= 0);
		ASSERTING(ecc[*vit] >= 0);

		dist(*vit,z) = dist(*vit, near_[z]) + spk.zdistance[near_[z]];
		ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
	}
	
	//cerr << " update the near values for other nodes in G " << endl;
	for(vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		ShortestPathKeeper &spk = findSPMaps(*vit); 
	
		double curr_ecc = (std::numeric_limits<double>::max)(); 
		//if(near[*vit] > -1) curr_ecc = ecc[near[*vit]] + spk.zdistance[near[*vit]];  
		//cerr << " examine all nodes in T_mdsph to determine near(" << *vit << ")	" << endl;
		for(vit_in = T_mdsph.V.begin(), vit_in_end = T_mdsph.V.end(); vit_in != vit_in_end; ++vit_in)
		{
			double new_ecc = ecc[*vit_in] + spk.zdistance[*vit_in];
			//cerr << "node: " << *vit_in << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
			
			ASSERTING(ecc[*vit_in] >= 0);
			ASSERTING(spk.zdistance[*vit_in] >= 0);
			
			if(new_ecc < curr_ecc)
			{ 
				//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
				near_[*vit] = *vit_in;
				curr_ecc = new_ecc;
			}
			//else cerr << " keeping near [" << *vit << "] = " << near[*vit] << " curr_ecc " << curr_ecc << endl;
		}
	}
}

void
MDShortestPathHeuristic::init(vertex_descriptorN src)
{
	vsVertexMapConst 	vsmap = get(&VertexProp::vertexState, g);
	
	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{	
		if(vsmap[*vit] == GROUP_MEMBER)
		{
			ZVertSet.insert(*vit);
			num_zvertices++;
		}
		
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) near_[*vit] = src;
			else near_[*vit] = -1;
		}
		else 
		{
			near_[*vit] = *vit;
			ecc[*vit] = 0;
		}
		for(vit_in = inputT.V.begin(), vit_in_end = inputT.V.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	ASSERTING(!ZVertSet.empty());
}

