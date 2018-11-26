/***************************************************************************
                          mddlsph.cc  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "mddlsph.h"
#include "../treealgs/dijkstra_sp.h"
#include <fstream>
#include "../simtime.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/* -------------------------------------------------------------------------
	
	Algorithm(): Minimum Diameter Degree Limited Shortest Path Heuristic 
							start function

------------------------------------------------------------------------- */
void
MDDLShortestPathHeuristic::Algorithm(vertex_descriptorN zsource)
{
	init(zsource);	// identify z-nodes and store them in VertexSet ZVertSet
	if(num_zvertices <= 0)
	{
		cout << WRITE_FUNCTION << " Error: No Z-vertices." << endl;	exit(0);
	}
	//cerr << WRITE_FUNCTION << " Start SPH zsource " << zsource << " and " << num_zvertices << " znodes" << endl ;

	// -- Start SPH Algorithm --
	T_mddlsph.insertVertex(zsource, g); 				// add source to the SPH tree
	ZVertSet.erase(zsource);
	RunDijkstraForEveryZ(g);							// find SP for every z-node and store in SPKeeper

	// -- fail safe --
	double failsafe = 0;
	// -- end fail safe --

	while(!ZVertSet.empty())
	{	
		degreeBroken.clear();
		int z = -1;
		PathVector newPath; 							// .first contains path to new z-node to be added to T_bddlsph
														// .second is the tree-vertex link -> TODO: probably not needed
		if(!ExtractClosestZ(newPath, z)) 				// extract the z-node closest to T_bddlsph
		{
			if(!TreeAlgorithms::relaxDegree(degreeBroken, degree_bound)) break;
			ASSERTING(!degreeBroken.empty());
		}
		else
		{
			ASSERTING(z > -1);
			AddTreeSPH(newPath); // add the new path to T_bddlsph
			updateDist(z);
		}
	}
	
	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_mddlsph.print();
	//T_mddlsph.printVertexState(cerr);
	// -- end debug --
}

/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_mddlsph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_mddlsph return the closest z-vertex and add the path to the T_mddlsph

 --------------------------------------------------------------------------*/
bool
MDDLShortestPathHeuristic::ExtractClosestZ(PathVector &newPath, int &z_closest)
{
	double diameterTree	= MAXIMUM_WEIGHT; //(numeric_limits<double>::max)(); 
	int z_in = -1;
	PathVector tempPath;
	
	// zit - z-node not-in-tree iterator, tit_mddlsph - tree node iterator
	// iterate through the z-vertices and check how close they are to T_mddlsph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		ASSERTING(!T_mddlsph.V.contains(zAddToTree));
		//cerr << WRITE_FUNCTION  << " Z : " << zAddToTree << endl;
		
		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
		
		VertexSet_it tit_mddlsph, tit_mddlsph_end;
		for(tit_mddlsph = T_mddlsph.V.begin(), tit_mddlsph_end = T_mddlsph.V.end(); tit_mddlsph != tit_mddlsph_end; ++tit_mddlsph) // check distance between new znode zit to every tree node tit_mddlsph
		{
			vertex_descriptorN zInTree = *tit_mddlsph;
			double new_diameter = ecc[zInTree] + spk.zdistance[zInTree];
			
			if(diameterTree > new_diameter && getOutDegree(T_mddlsph.g, zInTree) < degree_bound[zInTree]) //getDegreeConstraint(zInTree))	// is the new z-node closer than the previous -> then update distance etc.
			{
				if(FindPath(tempPath, zInTree, zAddToTree, spk.zparent))
				{
					diameterTree	= new_diameter;
					z_closest		= zAddToTree;
					z_in			= zInTree;
					newPath			= tempPath;
				}
			}
			tempPath = PathVector();

			if(getOutDegree(T_mddlsph.g, zInTree) >= degree_bound[zInTree]) degreeBroken.insert(zInTree);
		}
	}
	
	if(diameterTree < MAXIMUM_WEIGHT)return true;

	return false;
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
bool
MDDLShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	bool intact = true;
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	 
		if(getOutDegree(T_mddlsph.g, traverse_vert) >= degree_bound[traverse_vert]) 
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
	all_vertex_info.second: node in the T_mddlsph (steiner or z-node) that 
		links the new z-node to T_mddlsph
 --------------------------------------------------------------------------*/
void
MDDLShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
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

		if(T_mddlsph.V.contains(v)) done = true;
		
		T_mddlsph.insertEdge(u, v, g); 
		
		ZVertSet.erase(u); 
		ZVertSet.erase(v);
	
		if(done) break;
	}
}

void
MDDLShortestPathHeuristic::updateDist(vertex_descriptorN z)
{
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	ShortestPathKeeper &spk = findSPMaps(z); 
	
	//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << " spk.zdistance[near[z]]" << spk.zdistance[near[z]] << endl;
	ASSERTING(near_[z] > -1);
	ASSERTING(ecc[near_[z]] >= 0);

	// set dist(z,u) and ecc(z)
	for(vit = T_mddlsph.V.begin(), vit_end = T_mddlsph.V.end(); vit != vit_end; ++vit)
	{
		if(dist(near_[z],*vit) > 0) dist(z,*vit) = dist(near_[z],*vit) + spk.zdistance[near_[z]];
	}
	
	dist(z,z) = 0;
	ecc[z] = ecc[near_[z]] + spk.zdistance[near_[z]];  
	
	// update dist(near(z), u) and ecc(near(z))
	dist(near_[z],z) = spk.zdistance[near_[z]];  
	if(ecc[near_[z]] <= 0) ecc[near_[z]] = spk.zdistance[near_[z]]; 
	
	// update other nodes' values of dist and ecc
	for(vit = T_mddlsph.V.begin(), vit_end = T_mddlsph.V.end(); vit != vit_end; ++vit)
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
		
		//cerr << " examine all nodes in T_mddlsph to determine near(" << *vit << ")	" << endl;
		for(vit_in = T_mddlsph.V.begin(), vit_in_end = T_mddlsph.V.end(); vit_in != vit_in_end; ++vit_in)
		{
			double new_ecc = ecc[*vit_in] + spk.zdistance[*vit_in];
			
			//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
			ASSERTING(ecc[*vit_in] >= 0);
			ASSERTING(spk.zdistance[*vit_in] >= 0);
			
			if(new_ecc < curr_ecc && getOutDegree(T_mddlsph.g, *vit_in) < degree_bound[*vit_in] ) 	
			{ 
				//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
				near_[*vit] = *vit_in;
				curr_ecc = new_ecc;
			}
		}

		if(near_[*vit] > -1)
		{
			if(getOutDegree(T_mddlsph.g, near_[*vit]) >= degree_bound[near_[*vit]]) 
			{
				degreeBroken.insert(near_[*vit]);
				//cerr << " inserted degree broken " << degreeBroken << endl;
			}
		}
	}
}

void
MDDLShortestPathHeuristic::init(vertex_descriptorN src)
{
	vsVertexMap	vsmap = get(&VertexProp::vertexState, g);
	
	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{	
		if(vsmap[*vit] == GROUP_MEMBER)
		{
			ZVertSet.insert(*vit);
			num_zvertices++;
		}
		
		degree_bound[*vit] = getDegreeConstraint(g, *vit);
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

