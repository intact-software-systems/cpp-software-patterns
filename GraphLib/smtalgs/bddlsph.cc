/***************************************************************************
                          bddlsph.cc  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "bddlsph.h"
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
BDDLShortestPathHeuristic::Algorithm(vertex_descriptorN zsource, double D)
{
	DB = D;
	init(zsource, DB);	// identify z-nodes and store them in VertexSet ZVertSet
	if(num_zvertices <= 0)
	{
		cout << WRITE_FUNCTION << " Error: No Z-vertices." << endl;	exit(0);
	}
	cerr << WRITE_FUNCTION << " Start SPH zsource " << zsource << " and " << num_zvertices << " znodes" << endl ;

	// -- Start SPH Algorithm --
	T_bddlsph.insertVertex(zsource, g); 				// add source to the SPH tree
	ZVertSet.erase(zsource);
	RunDijkstraForEveryZ(g);							// find SP for every z-node and store in SPKeeper

	while(!ZVertSet.empty())
	{	
		diameterBroken.clear();
		degreeBroken.clear();

		int z = -1;
		PathVector newPath; 							// .first contains path to new z-node to be added to T_bddlsph
														// .second is the tree-vertex link -> TODO: probably not needed
		if(!ExtractClosestZ(newPath, z)) 				// extract the z-node closest to T_bddlsph
		{
			if(!TreeAlgorithms::relaxDegreeAndDiameter(degreeBroken, degree_bound, diameterBroken, diameter_bound)) break; 
			if(!TreeAlgorithms::isRelaxWorking(degreeBroken, diameterBroken)) break;
		}
		else
		{
			ASSERTING(z > -1);

			AddTreeSPH(newPath); // add the new path to T_bddlsph
			updateDist(z);
			//char c = getchar();
		}
	}
	
	// -- start debug --
	//cerr << WRITE_FUNCTION << "Produced Steiner Tree: " << endl;
	//T_bddlsph.print();
	//T_bddlsph.printVertexState(cerr);
	// -- end debug --
}

/* -------------------------------------------------------------------------

	ExtractClosestZ(): Returns the vertex that is closest to the T_bddlsph

		src: Dijkstra_shortest_path from this node
		p: next/parent pointers
		distances: distances from src to each other node in the graph

	IDEA: iterate through the z-vertices and check how close they are to 
	the T_bddlsph return the closest z-vertex and add the path to the T_bddlsph

 --------------------------------------------------------------------------*/
bool
BDDLShortestPathHeuristic::ExtractClosestZ(PathVector &newPath, int &z_closest)
{
	double distToTree	= MAXIMUM_WEIGHT; //(numeric_limits<double>::max)(); 
	int z_in = - 1;
	z_closest = -1;
	PathVector tempPath;

	VertexSet diameterNotBroken;
	// iterate through the z-vertices and check how close they are to T_bddlsph
	for(VertexSet::iterator zit = ZVertSet.begin(), zit_end = ZVertSet.end(); zit != zit_end; ++zit)
	{
		vertex_descriptorN zAddToTree = *zit;
		
		ASSERTING(!T_bddlsph.V.contains(zAddToTree));
		//cerr << WRITE_FUNCTION  << " Z : " << zAddToTree << endl;

		ShortestPathKeeper &spk = findSPMaps(zAddToTree); 
		
		VertexSet_it tit_bddlsph, tit_bddlsph_end;
		for(tit_bddlsph = T_bddlsph.V.begin(), tit_bddlsph_end = T_bddlsph.V.end(); tit_bddlsph != tit_bddlsph_end; ++tit_bddlsph) // check distance between new znode zit to every tree node tit_bddlsph
		{
			vertex_descriptorN zInTree = *tit_bddlsph;
			double new_dist = spk.zdistance[zInTree];
			double new_diameter = ecc[zInTree] + spk.zdistance[zInTree];
			
			//cerr << zInTree << ": new dist " << new_dist << " new diameter " << new_diameter << " out degree zintree " << getOutDegree(T_bddlsph.g, zInTree) << " bound " << degree_bound[zInTree] << endl;
			// is the new z-node closer than the previous and within diameter bound -> then update distance etc.
			//if(distToTree > new_dist && CheckPath(tempPath, zInTree, zAddToTree, spk.zparent))	// is the new z-node closer than the previous -> then update distance etc.
			if(distToTree > new_dist && new_diameter <= diameter_bound[zAddToTree] && getOutDegree(T_bddlsph.g, zInTree) < degree_bound[zInTree]) // is the new z-node closer than the previous -> then update distance etc. 
			{
				diameterNotBroken.insert(zAddToTree);
				//diameterBroken.erase(zAddToTree);
				//if(CheckPath(tempPath, zInTree, zAddToTree, spk.zparent))
				if(FindPath(tempPath, zInTree, zAddToTree, spk.zparent, spk.zdistance))
				{
					distToTree 	= new_dist;  					// update shortest distance
					z_closest	= zAddToTree;
					z_in		= zInTree;
					newPath		= tempPath;
					//cerr << zAddToTree << " new closest: " << zInTree << ": new dist " << new_dist << " distToTree " << distToTree << " new diameter " << new_diameter << " diameter bound " << diameter_bound[zAddToTree] << " degree limit " << degree_bound[zInTree] << endl;
				}
			}
	
			if(distToTree == MAXIMUM_WEIGHT)
			{
				if(new_diameter > diameter_bound[zAddToTree]) diameterBroken.insert(zAddToTree);
				if(getOutDegree(T_bddlsph.g, zInTree) >= degree_bound[zInTree]) degreeBroken.insert(zInTree);
			}
				
			tempPath = PathVector(); 
		
		}
	}
	
	if(distToTree < MAXIMUM_WEIGHT) return true;
	
	//cerr << " diameterbroken " << diameterBroken << endl;
	diameterBroken = diameterBroken - diameterNotBroken;
	//if(!diameterBroken.empty() || !degreeBroken.empty())	
	//cerr << " diameterbroken " << diameterBroken << " degreebroken " << degreeBroken << " diameterNotBroken " << diameterNotBroken << endl;

	return false;
}

bool 
BDDLShortestPathHeuristic::CheckPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent)
{
	double tree_depth = ecc[tree_vert];
	
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	
		bool intact = true;	
		if(getOutDegree(T_bddlsph.g, traverse_vert) >= degree_bound[traverse_vert]) 
		{
			//cerr << traverse_vert << " degree broken " << degree_bound[traverse_vert] << endl;
			degreeBroken.insert(traverse_vert);
			intact = false;
		}

		pair<edge_descriptorN, bool> ep = edge(traverse_vert, zparent[traverse_vert], g);
		ASSERTING(ep.second);
		//cerr << ep.first << " tree_depth[" << traverse_vert << "] = " << tree_depth << " + " << g[ep.first].weight << endl;
		//if(tree_depth > diameter_bound[traverse_vert])
		if(ZVertSet.contains(traverse_vert) && tree_depth > diameter_bound[traverse_vert])
		{
			//cerr << traverse_vert << " diameter broken " << tree_depth << " > " << diameter_bound[traverse_vert] << endl;
			diameterBroken.insert(traverse_vert);
			intact = false;
		}

		if(!intact) return intact;
		
		tree_depth = tree_depth + g[ep.first].weight;

		newPath.first.push_front(traverse_vert);
		traverse_vert = zparent[traverse_vert];	
	}while(traverse_vert != z_vert);
	
	newPath.first.push_front(traverse_vert); 		// add last node
	ASSERTING(newPath.first.size() > 1);

	//if(tree_depth > diameter_bound[traverse_vert])
	if(ZVertSet.contains(traverse_vert) && tree_depth > diameter_bound[traverse_vert])
	{
		//cerr << traverse_vert << " diameter broken " << tree_depth << " > " << diameter_bound[traverse_vert] << endl;
		diameterBroken.insert(traverse_vert);
		return false;
	}

	return true;
}

/* -------------------------------------------------------------------------

	FindDistance():	Find distance from z_vert to tree_vert using zparent 
			and zdistance from DijkstraSP(z_vert)

------------------------------------------------------------------------- */
bool
BDDLShortestPathHeuristic::FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, const DistanceVector &zdistance) 
{
	bool intact = true;
	ASSERTING(tree_vert != z_vert);
	vertex_descriptorN traverse_vert = tree_vert;
	do{	 
		if(getOutDegree(T_bddlsph.g, traverse_vert) >= degree_bound[traverse_vert]) 
		{
			degreeBroken.insert(traverse_vert);
		    //cerr << WRITE_FUNCTION << " can't add path violating degree bound " << getOutDegree(T_bddlsph.g, traverse_vert) << ">=" <<  degree_bound[traverse_vert] << endl;
			intact = false;
		}
		if(!T_bddlsph.V.contains(traverse_vert) && ecc[traverse_vert] + zdistance[traverse_vert] > diameter_bound[traverse_vert])
		{
		 	//cerr << WRITE_FUNCTION << " can't add path violating diameter bound " << ecc[traverse_vert] + zdistance[traverse_vert] << ">" <<  diameter_bound[traverse_vert] << endl;
			diameterBroken.insert(traverse_vert);
			intact = false;
		}
		
		if(!intact) return intact;
		
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
	all_vertex_info.second: node in the T_bddlsph (steiner or z-node) that 
		links the new z-node to T_bddlsph
 --------------------------------------------------------------------------*/
void
BDDLShortestPathHeuristic::AddTreeSPH(PathVector &newPath)
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

		if(T_bddlsph.V.contains(v)) done = true;
		
		T_bddlsph.insertEdge(u, v, g); 
		
		ZVertSet.erase(u); 
		ZVertSet.erase(v);
	
		// -- debug --
		/*if(getOutDegree(T_bddlsph.g, u) > degree_bound[u]) 
			cerr << " inserted " << u << " degree bound " << degree_bound[u] << " degree is " << getOutDegree(T_bddlsph.g, u) << endl;
		if(getOutDegree(T_bddlsph.g, v) > degree_bound[v]) 
			cerr << " inserted " << v << " degree bound " << degree_bound[v] << " degree is " << getOutDegree(T_bddlsph.g, v) << endl;
		if(ecc[u] > diameter_bound[u]) 
			cerr << " inserted " << u << " diameter bound " << diameter_bound[u] << " ecc is " << ecc[u] << endl;
		if(ecc[v] > diameter_bound[v]) 
			cerr << " inserted " << v << " diameter bound " << diameter_bound[v] << " ecc is " << ecc[v] << endl;*/
		// -- debug end --

		if(done) break;
	}
}

void
BDDLShortestPathHeuristic::updateDist(int z)
{
	ASSERTING(z > -1);
	ASSERTING(inputT.V.contains(z));
	//cerr << " z : " << g[z] << endl;

	ASSERTING(g[z].vertexState == GROUP_MEMBER);

	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
	ShortestPathKeeper &spk_z = findSPMaps(z); 
	
	//cerr << WRITE_FUNCTION << " z " << z << " near[z] " << near[z] << " spk.zdistance[near[z]]" << spk_z.zdistance[near[z]] << endl;
	ASSERTING(near_[z] > -1);
	ASSERTING(ecc[near_[z]] >= 0);

	// set dist(z,u) and ecc(z)
	for(vit = T_bddlsph.V.begin(), vit_end = T_bddlsph.V.end(); vit != vit_end; ++vit)
	{
		if(dist(near_[z],*vit) > 0) dist(z,*vit) = dist(near_[z],*vit) + spk_z.zdistance[near_[z]]; 
	}
	
	dist(z,z) = 0;
	ecc[z] = ecc[near_[z]] + spk_z.zdistance[near_[z]];  
	
	// update dist(near(z), u) and ecc(near(z))
	dist(near_[z],z) = spk_z.zdistance[near_[z]];  
	if(ecc[near_[z]] <= 0) ecc[near_[z]] = spk_z.zdistance[near_[z]];  
	
	// update other nodes' values of dist and ecc
	for(vit = T_bddlsph.V.begin(), vit_end = T_bddlsph.V.end(); vit != vit_end; ++vit)
	{
		ASSERTING(dist(*vit, near_[z]) >= 0);
		ASSERTING(dist(*vit, z) >= 0);
		ASSERTING(ecc[*vit] >= 0);

		dist(*vit,z) = dist(*vit, near_[z]) + spk_z.zdistance[near_[z]]; 
		ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
	}
	
	//cerr << " update the near values for other nodes in G " << endl;
	for(vit = ZVertSet.begin(), vit_end = ZVertSet.end(); vit != vit_end; ++vit)
	{
		ShortestPathKeeper &spk = findSPMaps(*vit); 
	
		double dc_near_vit = (numeric_limits<double>::max)(), od_near_vit = 0; 
		if(near_[*vit] > -1) 
		{
			dc_near_vit = degree_bound[near_[*vit]]; 
			od_near_vit = getOutDegree(T_bddlsph.g, near_[*vit]);
		}
			
		double curr_ecc = (std::numeric_limits<double>::max)(); 
		if(near_[*vit] > -1 && od_near_vit < dc_near_vit) curr_ecc = ecc[near_[*vit]] + spk.zdistance[near_[*vit]];  

		if(curr_ecc > diameter_bound[*vit] || (getOutDegree(T_bddlsph.g, z) >= degree_bound[z]))
		{
			//cerr << " examine all nodes in T_bddlsph to determine near(" << *vit << ")	" << endl;
			for(vit_in = T_bddlsph.V.begin(), vit_in_end = T_bddlsph.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				double new_ecc = ecc[*vit_in] + spk.zdistance[*vit_in];
				
				//cerr << " curr ecc " << curr_ecc << " new ecc " << new_ecc << endl;
				ASSERTING(ecc[*vit_in] >= 0);
				ASSERTING(spk.zdistance[*vit_in] >= 0);
				
				if(new_ecc < curr_ecc && getOutDegree(T_bddlsph.g, *vit_in) < degree_bound[*vit_in]) 
				{ 
					//cerr << " new near [" << *vit << "] = " << *vit_in << endl;
					near_[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		else
		{
			//cerr << "Compare w(" << *vit << "," << near[*vit] << ") = " << spk.zdistance[near[*vit]] << " to w(" << *vit << "," <<  z << ")= " << spk_z.zdistance[near[*vit]] << endl;
				
			if(spk_z.zdistance[*vit] <= spk.zdistance[near_[*vit]])
			{
				//cerr << " new near [" << *vit << "] = " << z << endl;
				near_[*vit] = z;
			}
		}

		//if(near[*vit] > -1)
		//{
			//if(curr_ecc >= diameter_bound[*vit]) diameterBroken.insert(*vit);
			//if(getOutDegree(T_bddlsph.g, near[*vit]) >= degree_bound[ near[*vit] ] ) degreeBroken.insert(near[*vit]);

			//cerr << " diameterbroken " << diameterBroken << " degreebroken " << degreeBroken << endl;
		//}
	}
}

void
BDDLShortestPathHeuristic::init(vertex_descriptorN src, const double &DB)
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
		
		diameter_bound[*vit] = DB;
		degree_bound[*vit] = getDegreeConstraint(g, *vit);

		ecc[*vit] = 0;		
		if(*vit != (int)src)
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

