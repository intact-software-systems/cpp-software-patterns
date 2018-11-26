/***************************************************************************
                          mesh_construction.cc  -  description
                             -------------------
    begin                : Thu Mar 09 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include "mesh_construction.h"
#include "complete_graph.h"
#include "graphalgs.h"
#include "coreh.h"
#include "steiner_set.h"
#include <iostream>
#include <fstream>

#include "../treealgs/treealgs.h"
#include "../treealgs/dijkstra_sp.h"
#include "../treealgs/prim_mst.h"
#include "../treealgs/prim_ottc.h"
#include "../treealgs/prim_rgh_bdmst.h"
#include "../simtime.h"
#include "./meshalgs.h"
#include <cmath>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

namespace GraphAlgorithms
{
/*-----------------------------------------------------------------------
			start function for building meshes
----------------------------------------------------------------------- */
bool buildMesh(const TreeStructure &inputT, TreeStructure& newT, GraphAlgo algo, int k, int src, clock_t &alg_time, uint64_t &cpu_cycles)
{
    uint64_t a, b;
#ifdef SYSTEM_SUPPORTS_PUSH_POP

	__asm __volatile( 	"push %eax\n"
						"push %ebx\n"
						"push %ecx\n"
						"push %edx\n"
						"cpuid\n"
						"pop %edx\n"
						"pop %ecx\n"
						"pop %ebx\n"
						"pop %eax\n" );

	__asm __volatile(".byte 0x0f,0x31" : "=A" (a));
#endif

	alg_time = buildMesh(inputT, newT, k, src, algo);
	
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile(".byte 0x0f,0x31" : "=A" (b));
	cpu_cycles = b - a;
#endif

	return (isValidMesh(newT, inputT));
}
		

clock_t buildMesh(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	// -- start debug --
	cerr << WRITE_FUNCTION << SimTime::Now() << " " << algo << " src " << src << " k " << k << endl; 
	ASSERTING(!inputT.V.empty());
	ASSERTING(inputT.Ep.size() == ((inputT.V.size() * (inputT.V.size() - 1))/ 2));
	// -- end debug --
		
	clock_t start_time = ::clock();		// start algorithm timer
	
	if(inputT.V.size() == 1)
	{
		newT = inputT;
		return (::clock() - start_time);
	}

	// 	-- Execute Mesh Algorithm --
	switch(algo)
	{
		// k trees algorithms
		case K_MINIMUM_SPANNING_TREE :
			kMinimumSpanningTree(inputT, newT, k);
			break;
		case K_DEGREE_LIMITED_MINIMUM_SPANNING_TREE :
			kDLMinimumSpanningTree(inputT, newT, k, src);
			break;
		case K_SHORTEST_PATH_TREE :
			kShortestPathTree(inputT, newT, k);
			break;
		case K_DEGREE_LIMITED_SHORTEST_PATH_TREE :
			kDLShortestPathTree(inputT, newT, k, src);
			break;
		case DEGREE_LIMITED_BEST_LINKS:
			DLBestLinks(inputT, newT);
			break;
		case K_MINIMUM_DIAMETER_OTTC_TREE:
			kMDOTTCTree(inputT, newT, k);
			break;
		case K_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE:
			kMDDLOTTCTree(inputT, newT, k, src);
			break;
		case K_DEGREE_LIMITED_RGH_TREE:
			kDLRGHTree(inputT, newT, k, src);
			break;
		// k parallel algorithms
		case KP_DEGREE_LIMITED_MINIMUM_SPANNING_TREE :
			kParallelDLMinimumSpanningTree(inputT, newT, k, src);
			break;
		case KP_DEGREE_LIMITED_SHORTEST_PATH_TREE :
			kParallelDLShortestPathTree(inputT, newT, k, src);
			break;
		case KP_MINIMUM_DIAMETER_DEGREE_LIMITED_OTTC_TREE:
			kParallelMDDLOTTCTree(inputT, newT, k, src);
			break;
		case KP_DEGREE_LIMITED_RGH_TREE:
			kParallelDLRGHTree(inputT, newT, k, src);
			break;
		// k combined algorithms
		case K_COMBINED_DLMST_DLRGH :
		case K_COMBINED_DLMST_MDDLOTTC :
		case KP_COMBINED_DLMST_DLRGH :
		case KP_COMBINED_DLMST_MDDLOTTC :
			kCombinedTreeAlgorithms(inputT, newT, k, src, algo);
			break;
		// edge pruning algorithms
		case K_BEST_LINKS:
		case ADD_CORE_LINKS :
		case ADD_CORE_LINKS_OPTIMIZED :
		case ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED : // TODO: does not always find a solution!!!!!
			edgePruningAlgs(inputT, newT, k);
			break;
		// enhanced tree algorithms
		case REDUCE_DIAMETER_DLMST :
		case REDUCE_DIAMETER_DLSPT :
		case REDUCE_DIAMETER_MDDL_OTTC :
		case REDUCE_DIAMETER_DL_RGH :
		case REDUCE_ECCENTRICITY_DLMST :
		case REDUCE_ECCENTRICITY_DLSPT :
		case REDUCE_ECCENTRICITY_MDDL_OTTC :
		case REDUCE_ECCENTRICITY_DL_RGH :
		case ADD_LONG_LINKS_TO_DLMST :
		case ADD_LONG_LINKS_TO_DLSPT :
		case ADD_LONG_LINKS_TO_MDDL_OTTC :
		case ADD_LONG_LINKS_TO_DL_RGH :
		case ADD_CORE_LINKS_TO_DLMST :
		case ADD_CORE_LINKS_TO_DLSPT :
		case ADD_CORE_LINKS_TO_MDDL_OTTC :
		case ADD_CORE_LINKS_TO_DL_RGH :
			enhancedTreeAlgorithms(inputT, newT, k, src, algo);
			break;
		default:
			cerr << WRITE_PRETTY_FUNCTION << " algorithm not supported : " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
	
	if(!newT.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(newT);
	//cerr << WRITE_FUNCTION << " newT V num : " << newT.V.size() << " E num " << newT.E.size() << " average out-degree : " << newT.E.size()/newT.V.size() << endl;

	return (::clock() - start_time); 	// execution time
}

void kCombinedTreeAlgorithms(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	int mc_edges = 0;
	int mddl_edges = 0;
	int i = k % 2;

	if(i == 0) // even number
	{
		mc_edges = k/2;
		mddl_edges = k/2;
	}
	else
	{
		mc_edges = (k + 1)/2;
		mddl_edges = (k - mc_edges);
	}
	
	switch(algo)
	{
		case K_COMBINED_DLMST_DLRGH :
		case K_COMBINED_DLMST_MDDLOTTC :
			kDLMinimumSpanningTree(inputT, newT, mc_edges, src);
			break;
		case KP_COMBINED_DLMST_DLRGH :
		case KP_COMBINED_DLMST_MDDLOTTC :
			kParallelDLMinimumSpanningTree(inputT, newT, mc_edges, src);
			break;
		default:
			break;
	}

	VertexSet excludeSet;
	if(newT.getOutDegree(src) >= getDegreeConstraint(newT.g, src))
	{
		for(VertexSet::iterator it = newT.V.begin(), it_end = newT.V.end(); it != it_end; ++it)
		{
			int v = *it;
			if(newT.getOutDegree(v) >= getDegreeConstraint(newT.g, v))
				excludeSet.insert(v);
		}


		//ASSERTING(num_edges(inputT.g) == ((inputT.V.size() * (inputT.V.size() - 1))/ 2));
		ASSERTING(!(inputT.V - (excludeSet + inputT.S)).empty());
		VertexSet newSrc;
		findWCNodes(inputT.g, inputT.g, inputT.V, inputT.V, excludeSet + inputT.S, newSrc, 1, GlobalSimArgs::getSimWCNAlgo()); 
		ASSERTING(!newSrc.empty());
		src = *(newSrc.begin());
	}

	if(newT.V.size() - excludeSet.size() <= 1) return;

	switch(algo)
	{
		case K_COMBINED_DLMST_DLRGH :
			kDLRGHTree(inputT, newT, mddl_edges, src);
			break;
		case K_COMBINED_DLMST_MDDLOTTC :
			kMDDLOTTCTree(inputT, newT, mddl_edges, src);
			break;
		case KP_COMBINED_DLMST_DLRGH :
			kParallelDLRGHTree(inputT, newT, mddl_edges, src);
			break;
		case KP_COMBINED_DLMST_MDDLOTTC :
			kParallelMDDLOTTCTree(inputT, newT, mddl_edges, src);
			break;
		default :
			kDLShortestPathTree(inputT, newT, mddl_edges, src);
			break;
	}
}

void enhancedTreeAlgorithms(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	// build tree
	switch(algo)
	{
		case REDUCE_ECCENTRICITY_DLMST:
		case REDUCE_DIAMETER_DLMST :
		case ADD_LONG_LINKS_TO_DLMST :
		case ADD_CORE_LINKS_TO_DLMST :
			getDBPrimMST(inputT, newT, src);
			break;
		case REDUCE_ECCENTRICITY_DLSPT :
		case REDUCE_DIAMETER_DLSPT:
		case ADD_LONG_LINKS_TO_DLSPT :
		case ADD_CORE_LINKS_TO_DLSPT :
			getDBDijkstraSPT(inputT, newT, src);
			break;
		case REDUCE_ECCENTRICITY_MDDL_OTTC :
		case REDUCE_DIAMETER_MDDL_OTTC :
		case ADD_LONG_LINKS_TO_MDDL_OTTC :
		case ADD_CORE_LINKS_TO_MDDL_OTTC :
			mddlOTTC(inputT, newT, src, inputT.V);
			break;
		case REDUCE_ECCENTRICITY_DL_RGH:
		case REDUCE_DIAMETER_DL_RGH :
		case ADD_LONG_LINKS_TO_DL_RGH :
		case ADD_CORE_LINKS_TO_DL_RGH :
			dlRGH(GlobalSimArgs::getDiameterBound(), inputT, newT, src, inputT.V);
			break;
		default:
			getDBPrimMST(inputT, newT, src);
			break;
	}

	// enhance tree
	switch(algo)
	{
		// enhanced tree algorithms
		case REDUCE_DIAMETER_DLMST :
		case REDUCE_DIAMETER_DLSPT :
		case REDUCE_DIAMETER_MDDL_OTTC :
		case REDUCE_DIAMETER_DL_RGH :
			reduceDiameterOfTree(inputT, newT, k, src, algo);		
			break;
		case REDUCE_ECCENTRICITY_DLMST :
		case REDUCE_ECCENTRICITY_DLSPT :
		case REDUCE_ECCENTRICITY_MDDL_OTTC :
		case REDUCE_ECCENTRICITY_DL_RGH :
			reduceEccentricityOfNodes(inputT, newT, k, src, algo);
			break;
		case ADD_LONG_LINKS_TO_DLMST :
		case ADD_LONG_LINKS_TO_DLSPT :
		case ADD_LONG_LINKS_TO_MDDL_OTTC :
		case ADD_LONG_LINKS_TO_DL_RGH :
			addLongLinksToTree(inputT, newT, k, src, algo);
			break;
		case ADD_CORE_LINKS_TO_DLMST :
		case ADD_CORE_LINKS_TO_DLSPT :
		case ADD_CORE_LINKS_TO_MDDL_OTTC :
		case ADD_CORE_LINKS_TO_DL_RGH :
			addCoreLinksToTree(inputT, newT, k, src, algo);
			break;
		default :
			cerr << WRITE_FUNCTION << " ERROR! graph algorithms is not supported" << endl;
			ASSERTING(0);
			break;
	}
}

// find k core nodes
// add k links to these core nodes or between them?
// How to find nodes to add links to?
void addCoreLinksToTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	const GraphN &g = inputT.g;
	multimap<double, int> rankMap;
	double sum_weight = 0;
	
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	
	for(VertexSet::iterator vit = newT.V.begin(), vit_end = newT.V.end(); vit != vit_end; ++vit)
	{
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, g); eit != eit_end; ++eit)
		{
			sum_weight += wmap[*eit];		
		}
	
		if(getOutDegree(g, *vit) > 0) 
			rankMap.insert(pair<double, int>((sum_weight/getOutDegree(*vit, g)), *vit));
		sum_weight = 0;
	}
	
	//NOW: add k links between (k-worst nodes, k-median nodes)
	int added_edges = 0;

	for(multimap<double, int>::reverse_iterator mit = rankMap.rbegin(), mit_end = rankMap.rend(); mit != mit_end; ++mit)
	{
		if(added_edges >= k) break;

		int worst = mit->second;
		if(getOutDegree(g, worst) >= getDegreeConstraint(g, worst)) continue;

		for(multimap<double, int>::iterator mit_in = rankMap.begin(), mit_in_end = rankMap.end(); mit_in != mit_in_end; ++mit_in)
		{
			int best = mit->second;
			if(best == worst) continue;
			if(inputT.isEdge(worst, best)) continue;
			if(getOutDegree(g, best) >= getDegreeConstraint(g, best)) continue;
			if(getOutDegree(g, worst) >= getDegreeConstraint(g, worst)) break;

			newT.insertEdge(worst, best, inputT.g);
			added_edges++;
			if(added_edges >= k) break;
		}
	}
}

void addLongLinksToTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	int added_edges = 0;
	int previous_added_edges = 0;

	while(added_edges < k)
	{
		map<int, int> mapDegreeId;
		for(VertexSet::const_iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
			mapDegreeId.insert(pair<int, int>(getOutDegree(*vi, newT.g), *vi));  

		for(map<int, int>::iterator vi = mapDegreeId.begin(), vi_end = mapDegreeId.end(); added_edges < k && vi != vi_end; ++vi)
		{  
			int u = vi->second;
			if(getDegreeConstraint(inputT.g, u) <= getOutDegree(u, newT.g)) continue;

			std::multimap<double, edge_descriptorN> addEdges;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(u, inputT.g); oit != oit_end; ++oit)
				addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
			std::multimap<double, edge_descriptorN>::reverse_iterator mit = addEdges.rbegin(), mit_end = addEdges.rend();
			for( ; mit != mit_end; ++mit)
			{
				if(getDegreeConstraint(inputT.g, u) <= getOutDegree(u, newT.g)) break;

				int v = target(mit->second, inputT.g);
		
				ASSERTING(u != v);
				if(u == v) continue;
				if(getDegreeConstraint(inputT.g, v) <= getOutDegree(v, newT.g)) continue;
				
				bool added = false;
				if(newT.isEdge(u, v) == false)
				{
					added_edges++;		
					newT.insertEdge(u, v, inputT.g);
					added = true;
					break;
				}
				if(added == true) break;
			}
		}
		
		if(previous_added_edges == added_edges) break;
		previous_added_edges = added_edges;
	}
	
	//cerr << WRITE_FUNCTION << " added " << added_edges << " edges " << endl;
}
void reduceDiameterOfTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	int i = 0;
	for(i = 0; i < k; i++)
	{
		PathList diameter_path;
		double diameter = 0;
		GraphAlgorithms::diameter_distance(newT, diameter_path, diameter);
		
		PathList::iterator diameter_it = diameter_path.begin();
		PathList::reverse_iterator diameter_rit = diameter_path.rbegin();

		int source = *diameter_it;
		int targ = *diameter_rit;
		
		//cerr << "Suggested edge : (" << source << "," << targ << ")" << " outdegree source : " << getOutDegree(source, newT.g) << " out degree targ " << getOutDegree(targ, newT.g) << endl;
		//cerr << "Diameter " << diameter << " and diameter path : " ;
		//for(PathList::iterator it = diameter_path.begin(), it_end = diameter_path.end(); it != it_end; ++it)
		//	cerr << *it << "," ;
		//cerr << endl;

		do
		{
			bool traversing = false;
			while(getDegreeConstraint(inputT.g, source) <= getOutDegree(source, newT.g)) 
			{
				diameter_it++;
				if(diameter_it == diameter_path.end()) break;
				source = *diameter_it;
				traversing = true;
			}
		
			if(source == targ) break;
			if(diameter_it == diameter_path.end()) break;

			while(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g))
			{
				diameter_rit++;
				if(diameter_rit == diameter_path.rend()) break;
				targ = *diameter_rit;			
				traversing = true;
			}

			if(diameter_rit == diameter_path.rend()) break;
			if(source == targ) break;
			if(!traversing) break;

		}while(newT.isEdge(source, targ) == true);

		if(diameter_rit == diameter_path.rend()) break;
		if(diameter_it == diameter_path.end()) break;
		if(source == targ) break;
		if(newT.isEdge(source, targ) == true) break;
		if(getDegreeConstraint(inputT.g, source) <= getOutDegree(source, newT.g)) break;
		if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) break;

		//cerr << "Adding edge : (" << source << "," << targ << ")" << endl;
		newT.insertEdge(source, targ, inputT.g);
	}

	//cerr << WRITE_FUNCTION << " added " << i << " edges " << " v size : " << newT.V.size() << endl;
	//if(newT.V.size() > 80) 
	//char c = getchar();
}

void reduceEccentricityOfNodes(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo)
{
	cerr << WRITE_FUNCTION << " function is not implemented " << endl;
	ASSERTING(0); 

	/*DistanceVectorMatrix distance(num_vertices(inputT.g), num_vertices(inputT.g));
	DistanceVectorMatrix hops(num_vertices(inputT.g), num_vertices(inputT.g));
	ParentVectorMatrix parent(num_vertices(inputT.g), num_vertices(inputT.g));

	for(int i = 0; i < k; i++)
	{
		distanceMatrix(newT, distance, hops, parent);
		// TODO: I don't know what to do here!
	}*/
}

void kDLRGHTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}

	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		dlRGH(GlobalSimArgs::getDiameterBound(), meshT, T, src, meshT.V);
				
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}

void kParallelDLRGHTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}

	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		dlRGH(GlobalSimArgs::getDiameterBound(), meshT, T, src, meshT.V);
				
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		//meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}


void kMDDLOTTCTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}

	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		mddlOTTC(meshT, T, src, meshT.V);
		
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
	
		// -- debug --
		if(!meshT.V.contains(src)) cerr << WRITE_FUNCTION << " src " << src << " not in meshT.V " << meshT.V << endl;
		ASSERTING(meshT.V.contains(src));
		// -- end debug --
	}
}

void kParallelMDDLOTTCTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}

	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		mddlOTTC(meshT, T, src, meshT.V);
		
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		//meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
	
		// -- debug --
		if(!meshT.V.contains(src)) cerr << WRITE_FUNCTION << " src " << src << " not in meshT.V " << meshT.V << endl;
		ASSERTING(meshT.V.contains(src));
		// -- end debug --
	}
}

void kMDOTTCTree(const TreeStructure &inputT, TreeStructure &newT, int k)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) meshT.diffTrees(newT);

	VertexSet coreSet;
	findWCNodes(inputT, coreSet, k, GlobalSimArgs::getSimWCNAlgo()); 

	for(VertexSet::const_iterator vi = coreSet.begin(), vi_end = coreSet.end(); vi != vi_end; ++vi)
	{
		//cerr << WRITE_FUNCTION << " mesh : " << meshT << " coreSet " << coreSet << endl;
		int src = *vi;
		if(!meshT.V.contains(src))
		{
			VertexSet S = meshT.V - coreSet;	// choose one from meshT.V not in coreSet
			if(S.empty()) src = -1;
			src = *(S.begin());
		}

		// found valid src?
		if(src < 0) continue;
		if(!meshT.V.contains(src)) continue;

		TreeStructure T;
		mdOTTC(meshT, T, src, meshT.V);

		//cerr << WRITE_FUNCTION << " T : " << T << endl;
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		if(meshT.E.size() <= 1) break;
	}
}

void kShortestPathTree(const TreeStructure &inputT, TreeStructure &newT, int k)
{
	TreeStructure meshT = inputT;
	k = std::max(1, k);

	VertexSet coreSet;
	findWCNodes(inputT, coreSet, k, GlobalSimArgs::getSimWCNAlgo()); 

	for(VertexSet::const_iterator vi = coreSet.begin(), vi_end = coreSet.end(); vi != vi_end; ++vi)
	{
		//cerr << WRITE_FUNCTION << " mesh : " << meshT << " coreSet " << coreSet << endl;
		int src = *vi;
		if(!meshT.V.contains(src))
		{
			VertexSet S = meshT.V - coreSet;	// choose one from meshT.V not in coreSet
			if(S.empty()) src = -1;
			src = *(S.begin());
		}

		// found valid src?
		if(src < 0) continue;
		if(!meshT.V.contains(src)) continue;

		TreeStructure T;
		getDijkstraSPT(meshT, T, src);

		//cerr << WRITE_FUNCTION << " T : " << T << endl;
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		if(meshT.E.size() <= 1) break;
	}
}

void kDLShortestPathTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}

	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		getDBDijkstraSPT(meshT, T, src);
		
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}

void kParallelDLShortestPathTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}
	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		getDBDijkstraSPT(meshT, T, src);
		
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		//meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}

		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}

void kMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &newT, int k)
{
	TreeStructure meshT = inputT;

	VertexSet coreSet;
	findWCNodes(inputT, coreSet, k, GlobalSimArgs::getSimWCNAlgo()); 

	for(VertexSet::const_iterator vi = coreSet.begin(), vi_end = coreSet.end(); vi != vi_end; ++vi)
	{
		//cerr << WRITE_FUNCTION << " mesh : " << meshT << " coreSet " << coreSet << endl;

		int src = *vi;
		if(!meshT.V.contains(src))
		{
			VertexSet S = meshT.V - coreSet;	// choose one from meshT.V not in coreSet
			if(S.empty()) src = -1;
			src = *(S.begin());
		}

		// found valid src?
		if(src < 0) continue;
		if(!meshT.V.contains(src)) continue;

		TreeStructure T;
		getPrimMST(meshT, T, src);

		//cerr << WRITE_FUNCTION << " T : " << T << endl;
		
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		if(meshT.E.size() <= 1) break;
	}
}

void kDLMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}
	
	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		getDBPrimMST(meshT, T, src);
	
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}
	
		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}

void kParallelDLMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src)
{
	TreeStructure meshT = inputT;
	if(!newT.V.empty()) 
	{
		meshT.diffTrees(newT);
		if(!meshT.V.contains(src)) return;
		if(meshT.getOutDegree(src) <= 0) return;
	}
	VertexSet srcSet;
	for(int i = 0; i < k; i++)
	{
		TreeStructure T;
		getDBPrimMST(meshT, T, src);
	
		if(!T.S.empty()) TreeAlgorithms::pruneNonTerminalLeaves(T);

		newT.mergeTrees(T);		
		//meshT.diffTrees(T);

		for(VertexSet::const_iterator vi = newT.V.begin(), vi_end = newT.V.end(); vi != vi_end; ++vi)
		{  
			if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g) && meshT.isVertex(*vi))
			{
				meshT.removeVertex(*vi); // remove from meshT
			}
			else
			{
				meshT.g[*vi].degree_limit = getDegreeConstraint(inputT.g, *vi) - getOutDegree(*vi, newT.g);
			}
		}
	
		srcSet.insert(src);
		VertexSet V = meshT.V;
		meshT.V = meshT.V - srcSet;
		if(meshT.E.size() <= 1) break;

		src = findWCNodes(meshT, GlobalSimArgs::getSimWCNAlgo());
		if(src < 0) break;
		ASSERTING(getOutDegree(meshT.g, src) > 0);

		meshT.V = V;
		ASSERTING(meshT.V.contains(src));
	}
}


void DLBestLinks(const TreeStructure &inputT, TreeStructure &newT)
{
	for(VertexSet::const_iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{  
		int src = *vi;
		int degree_limit = getDegreeConstraint(inputT.g, src);

		ASSERTING(getOutDegree(src, inputT.g) > 0);
		newT.insertVertex(src, inputT.g[src], inputT.g[src].vertexState);	
		
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(src, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
		
		std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
		for(int i = 0; i < degree_limit && mit != mit_end; ++mit)
		{
			int targ = target(mit->second, inputT.g);
			ASSERTING(src != targ);
			if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;

			if(newT.isEdge(src, targ) == false)
			{	
				newT.insertEdge(src, targ, inputT.g);
				//cerr << endl << "Adding " << mit->second << endl;
				i++;
			}
		}
	}

	// ensure graph is connected
	connectPartitionedGraph(inputT, newT);
}


void edgePruningAlgs(const TreeStructure &inputT, TreeStructure &newT, int k)
{
	GraphAlgo algo = GlobalSimArgs::meshAlgo();

	if(algo == K_BEST_LINKS)
	{
		kBL(inputT, newT, k);
		return;
	}

	VertexSet coreSet;
	if(COMPLETE_MEMBER_GRAPH == GlobalSimArgs::getSimGraphAlgo())
	{
		int numToAdd = (int) (ceil( (double) (inputT.V.size())/ (double) GlobalSimArgs::getDegreeConstraint()));
		numToAdd = numToAdd + (int) (ceil( (double) (numToAdd + 2)/ (double) GlobalSimArgs::getDegreeConstraint()));
		
		findWCNodes(inputT, coreSet, numToAdd, GlobalSimArgs::getSimWCNAlgo()); 
		ASSERTING(!coreSet.empty());
	}
	else
	{
		ASSERTING(!inputT.S.empty());
		coreSet = inputT.S;
	}
		
	if(algo == ADD_CORE_LINKS)
		aCL_noSP(inputT, coreSet, newT, k);
	else if(algo == ADD_CORE_LINKS_OPTIMIZED)
		aCLO_noSP(inputT, coreSet, newT, k, GlobalSimArgs::getDegreeConstraint());
	else if(algo == ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED)
		dlaCLO_noSP(inputT, coreSet, newT, k, GlobalSimArgs::getDegreeConstraint()); 	

	// -- hack to test core selection heuristic variations --
	newT.C = coreSet;
}

/*-----------------------------------------------------------------------
				kBestLinksGraph():		
----------------------------------------------------------------------- */
void kBL(const TreeStructure& inputT, TreeStructure &newT, int k)
{
	if(k < 2) 
	{
		int src = *(inputT.V.begin());
		getPrimMST(inputT, newT, src);
		return;
	}

	for(VertexSet::const_iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		int src = *vi;
		ASSERTING(getOutDegree(src, inputT.g) > 0);
		newT.insertVertex(src, inputT.g[src], inputT.g[src].vertexState);	
		
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(src, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
		for(int i = 0; i < k && mit != mit_end; ++mit)
		{
			int targ = target(mit->second, inputT.g);
			ASSERTING(src != targ);
			i++;
		
			if(newT.isEdge(src, targ) == false)
			{	
				newT.insertEdge(src, targ, inputT.g);
				//cerr << endl << "Adding " << mit->second << endl;
			}
		}
	}
	
	// ensure graph is connected
	connectPartitionedGraph(inputT, newT);
}

void aCL_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure& newT, int k)
{
	//cerr << WRITE_FUNCTION << " WCN member set size " << coreSet.size() << endl;
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
		
		// well connected group members are connected to all group members
		if(coreSet.contains(*vi))  
		{
			for(VertexSet::const_iterator vit_in = inputT.V.begin(), vit_in_end = inputT.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
	
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
		}
		else // group members
		{
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN> addEdges;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
				addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;

				if(newT.isEdge(*vi, targ) == false)
					newT.insertEdge(*vi, targ, inputT.g);
				//cerr << endl << "Adding " << mit->second << endl;
				i++;				
			}
		}
	}
}


void aCLO_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	//cerr << WRITE_FUNCTION << " k: " << k << " sk: " << sk << endl;
	if(coreSet.size() <= 1) sk = inputT.V.size();
	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// well connected group members are connected to sk group members
		if(coreSet.contains(*vi))  
		{
			// fully meshed within coreSet
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
			
			// then, connect to	sk other members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				
				// check if we want to add
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					if(*vit_in == *vi) continue;

					if(newT.isEdge(*vit_in, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				// end check
				
				// now: safe to add, if possible		
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;

				if(newT.isEdge(*vi, targ) == false)
					newT.insertEdge(*vi, targ, inputT.g);

				//cerr << endl << "Adding " << mit->second << endl;
				i++;
			}
		}
	}
}

void dlaCLO_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	double core_set_meshify_factor = 0.33;
	if(coreSet.size() <= 1) sk = inputT.V.size();
	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		int src = *vi;

		ASSERTING(getOutDegree(src, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(src, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// well connected group members are connected to sk group members
		if(coreSet.contains(src))  
		{
			// meshify_factor influences the density of the coreSet
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				int targ = *vit_in;
				if(src == targ) continue;
				if((double) ((double)getDegreeConstraint(inputT.g, src) * core_set_meshify_factor) <= getOutDegree(src, newT.g)) continue;
				if((double) ((double)getDegreeConstraint(inputT.g, targ) * core_set_meshify_factor) <= getOutDegree(targ, newT.g)) continue;

				if(newT.isEdge(src, targ) == false && inputT.isEdge(src, targ) == true)
				{	
					newT.insertEdge(src, targ, inputT.g);
					//cerr << endl << "Adding " << targ << endl;
				}
			}
			
			// then, connect to	sk other members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(src != targ);
		
				if(getDegreeConstraint(inputT.g, src) <= getOutDegree(src, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;
				
				// check if we want to add
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					int core = *vit_in;
					if(core == src) continue;

					if(newT.isEdge(core, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				// end check
		
				// now: safe to add, if possible		
				if(newT.isEdge(src, targ) == false)
				{	
					newT.insertEdge(src, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(src, inputT.g[src], inputT.g[src].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(src != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;
				if(getDegreeConstraint(inputT.g, src) <= getOutDegree(src, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;

				if(newT.isEdge(src, targ) == false)
				{
					newT.insertEdge(src, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
				}
				i++;
			}
		}
	}

	// ensure graph is connected
	connectPartitionedGraph(inputT, newT);
}


/*-----------------------------------------------------------------------
	pruneGraph(g,k):
		Start function for pruning a complete graph.

TODO: use VertexSet merge to find out whether pruned graph is disjoint
Algorithm:
VertexSet V[(num_vertices(inputG))];
1. start with a forest of trees V[i] contains only itself
2. merge two and two trees -> merge VertexSet
3. at the end V[0-num_vertices(inputG)] are all equal

TODO: make a kBL that takes half and half group members and new steiner points
TODO: k-MSTs 
TODO: k-MSTs combined with adding core linkes
----------------------------------------------------------------------- */
void pruneGraph(const TreeStructure &inputT, TreeStructure &newT, int k)
{
	GraphAlgo algo = GlobalSimArgs::getSimPruneAlgo();
	//cerr << WRITE_FUNCTION << algo << endl;
	if(algo == NO_GRAPH_ALGO) 
		return;
	
	if(algo == K_BEST_LINKS)
	{
		kBL(inputT, newT, k);
		return;
	}

	// -- Prune Graph Algorithms --
	if(inputT.S.empty())
	{
		VertexSet coreSet;
		findWCNodes(inputT, coreSet, generateSizeOfSteinerSet(inputT.V.size()), GlobalSimArgs::getSimWCNAlgo()); 
	
		ASSERTING(!coreSet.empty());
		
		if(algo == ADD_CORE_LINKS)
			kBestLinksGraphNoSP(inputT, coreSet, newT, k);
		else if(algo == ADD_CORE_LINKS_OPTIMIZED)
			kBestLinksOptimizedGraphNoSP(inputT, coreSet, newT, k, GlobalSimArgs::getDegreeConstraint());
		else if(algo == ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED)
			kBestLinksOptimizedDLGraphNoSP(inputT, coreSet, newT, k, GlobalSimArgs::getDegreeConstraint()); 	

		// -- hack to test core selection heuristic variations --
		newT.C = coreSet;
	}
	else // steiner heuristic
	{	
		VertexSet coreSet = inputT.S;
		
		// -- this is really a hack -> adds group members to be among the core nodes! --
		int numToAdd = generateSizeOfSteinerSet(inputT.V.size() - inputT.S.size());
		if(GlobalSimArgs::getSteinerMemberSize() > 0) 
		{
			int core_size = GlobalSimArgs::getSteinerMemberSize() - coreSet.size();
			if(core_size > 0)
				findWCNodes(inputT.g, inputT.g, inputT.V, inputT.V, inputT.S, coreSet, numToAdd, GlobalSimArgs::getSimWCNAlgo()); 
		}
		else if(numToAdd > (int)inputT.S.size())
		{
			 int uncovered = (inputT.V.size() - inputT.S.size()) - (inputT.S.size() * GlobalSimArgs::getDegreeConstraintSP());
			 if(uncovered > 0)
			 {
			 	//cerr << "Uncovered member nodes " << uncovered << endl;
			 	numToAdd = generateSizeNonSteiner(uncovered);
				if(numToAdd > 0) 
				{
					findWCNodes(inputT.g, inputT.g, inputT.V, inputT.V, inputT.S, coreSet, numToAdd, GlobalSimArgs::getSimWCNAlgo()); 
					ASSERTING(!coreSet.empty());
				}
			 }
		}
		// -- end hack --
	
		//cerr << WRITE_FUNCTION << " coreSet " << coreSet << " size() " << coreSet.size() << " numToAdd " << numToAdd << endl;
		if(algo == ADD_CORE_LINKS)
		{
			kBestLinksGraph(inputT, coreSet, newT, k);
		}
		else if(algo == ADD_CORE_LINKS_OPTIMIZED)
		{
			int sk = min(int ((int) (inputT.V.size()/inputT.S.size())), GlobalSimArgs::getDegreeConstraintSP() - 1);
			kBestLinksOptimizedGraph(inputT, coreSet, newT, k, sk); 
		}
		else if(algo == ADD_CORE_LINKS_OPTIMIZED_DEGREE_LIMITED)
		{
			//int sk = min(int ((int) (inputT.V.size()/inputT.S.size())), GlobalSimArgs::getDegreeConstraintSP() - 1);
			kBestLinksOptimizedDLGraphNoSP(inputT, coreSet, newT, k, GlobalSimArgs::getDegreeConstraintSP()); 	
		}
		else // NO_GRAPH_ALGO
		{
			//cerr << "No prune algorithm given " << algo << endl; 
			ASSERTING(algo == NO_GRAPH_ALGO);
		}

		newT.C = coreSet;
	}
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(inputT);	
}

/*-----------------------------------------------------------------------
				kBestLinksGraph():		
----------------------------------------------------------------------- */
void
kBestLinksGraph(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k)
{
	for(VertexSet::const_iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
		newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
		
		// steiner points are connected to all group members
		if(coreSet.contains(*vi))
		{
			for(VertexSet::const_iterator vit_in = inputT.V.begin(), vit_in_end = inputT.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
		}
		else // group members
		{
			if(k <= 0) continue;
			
			std::multimap<double, edge_descriptorN> addEdges;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
				addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;
				
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}

// TODO: connect steiner points to |sk| best group member edges, where sk >> k
void
kBestLinksOptimizedGraph(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	if(inputT.S.size() <= 1) sk = inputT.V.size();
	//cerr << WRITE_FUNCTION << " k : " << k << " sk " << sk << " S " << inputT.S << endl;
	
	for(VertexSet::const_iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// steiner points are connected to sk group members
		if(coreSet.contains(*vi))
		{
			// steiner points are fully meshed
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
				//	cerr << endl << "Adding " << *vit_in << endl;
				}
			}
	
			// core points are connected to sk group members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				
				// check if we want to add
				if(coreSet.contains(targ)) continue;
				
				// ensures that group members are only connected to a single steiner point
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					if(*vit_in == *vi) continue;
					if(newT.isEdge(*vit_in, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				
				// now: safe to add if not already there
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;

				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}

void kBestLinksOptimizedDLGraph(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	//if(inputT.S.size() <= 1) sk = inputT.V.size();
	//cerr << WRITE_FUNCTION << " k : " << k << " sk " << sk << " S " << inputT.S << endl;

	int core_set_meshify_factor = 3;
	if(coreSet.size() <= 1) sk = inputT.V.size();
	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// well connected group members are connected to sk group members
		if(coreSet.contains(*vi))  
		{
			// meshify_factor influences the density of the coreSet
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				if(getDegreeConstraint(inputT.g, *vi)/core_set_meshify_factor <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, *vit_in)/core_set_meshify_factor <= getOutDegree(*vit_in, newT.g)) continue;

				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
			
			// then, connect to	sk other members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
		
				if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;
				
				// check if we want to add
				if(coreSet.contains(targ)) continue;

				// check if we want to add
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					if(*vit_in == *vi) continue;

					if(newT.isEdge(*vit_in, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				// end check
		
				// now: safe to add, if possible		
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;
				if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;

				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------
			kBestLinksGraphNoSP(): for non-SMT algorithms
----------------------------------------------------------------------- */
void 
kBestLinksGraphNoSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure& newT, int k)
{
	//cerr << WRITE_FUNCTION << " WCN member set size " << coreSet.size() << endl;
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
		
		// well connected group members are connected to all group members
		if(coreSet.contains(*vi))  
		{
			for(VertexSet::const_iterator vit_in = inputT.V.begin(), vit_in_end = inputT.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
		}
		else // group members
		{
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN> addEdges;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
				addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;

				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}

void
kBestLinksOptimizedGraphNoSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	//cerr << WRITE_FUNCTION << " k: " << k << " sk: " << sk << endl;
	if(coreSet.size() <= 1) sk = inputT.V.size();
	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// well connected group members are connected to sk group members
		if(coreSet.contains(*vi))  
		{
			// fully meshed within coreSet
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				
				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
			
			// then, connect to	sk other members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				
				// check if we want to add
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					if(*vit_in == *vi) continue;

					if(newT.isEdge(*vit_in, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				// end check
				
				// now: safe to add, if possible		
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;

				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}

void kBestLinksOptimizedDLGraphNoSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk)
{
	int core_set_meshify_factor = 3;
	if(coreSet.size() <= 1) sk = inputT.V.size();
	
	for(VertexSet::iterator vi = inputT.V.begin(), vi_end = inputT.V.end(); vi != vi_end; ++vi)
	{
		ASSERTING(getOutDegree(*vi, inputT.g) > 0);
	
		std::multimap<double, edge_descriptorN> addEdges;
		out_edge_iteratorN oit, oit_end;  
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, inputT.g); oit != oit_end; ++oit)
			addEdges.insert(pair<double, edge_descriptorN>(get(&EdgeProp::weight, inputT.g)[*oit], *oit));
			
		// well connected group members are connected to sk group members
		if(coreSet.contains(*vi))  
		{
			// meshify_factor influences the density of the coreSet
			for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vi == *vit_in) continue;
				if(getDegreeConstraint(inputT.g, *vi)/core_set_meshify_factor <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, *vit_in)/core_set_meshify_factor <= getOutDegree(*vit_in, newT.g)) continue;

				if(newT.isEdge(*vi, *vit_in) == false && inputT.isEdge(*vi, *vit_in) == true)
				{	
					newT.insertEdge(*vi, *vit_in, inputT.g);
					//cerr << endl << "Adding " << *vit_in << endl;
				}
			}
			
			// then, connect to	sk other members
			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < sk && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
		
				if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;
				
				// check if we want to add
				bool doAdd = true;
				for(VertexSet::iterator vit_in = coreSet.begin(), vit_in_end = coreSet.end(); vit_in != vit_in_end; ++vit_in)
				{
					if(*vit_in == *vi) continue;

					if(newT.isEdge(*vit_in, targ) == true)
					{
						doAdd = false;
						break;
					}
				}
				if(!doAdd) continue;
				// end check
		
				// now: safe to add, if possible		
				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
		else // group members
		{	
			newT.insertVertex(*vi, inputT.g[*vi], inputT.g[*vi].vertexState);	
			if(k <= 0) continue;

			std::multimap<double, edge_descriptorN>::iterator mit = addEdges.begin(), mit_end = addEdges.end();
			for(int i = 0; i < k && mit != mit_end; ++mit)
			{
				int targ = target(mit->second, inputT.g);
				ASSERTING(*vi != targ);
				if(get(&VertexProp::vertexState, inputT.g)[targ] != GROUP_MEMBER) continue;
				if(getDegreeConstraint(inputT.g, *vi) <= getOutDegree(*vi, newT.g)) continue;
				if(getDegreeConstraint(inputT.g, targ) <= getOutDegree(targ, newT.g)) continue;

				if(newT.isEdge(*vi, targ) == false)
				{	
					newT.insertEdge(*vi, targ, inputT.g);
					//cerr << endl << "Adding " << mit->second << endl;
					i++;
				}
			}
		}
	}
}

}; // namespace GraphAlgorithms

/*-----------------------------------------------------------------------
	kRandomLinksRemoveGraph():		
		receives a global complete graph, removes k out edges for each
	vertex. MINIMUM getOutDegree is 2.
----------------------------------------------------------------------- */
/*void
MeshConstruct::kRandomLinksRemoveGraph(GraphN& g, int k)
{
	vertex_iteratorN vi, vi_end;
    for(boost::tuples::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
	{
		int deg = getOutDegree(*vi, g);
		//cerr << *vi << " out-degree " << deg << endl;
		for(int i = 0; i < k && deg > 2; i++)
		{
			int pick = generateRandomNumber((double) deg);
			//cerr << "Pick " << pick << endl;

			out_edge_iteratorN oit, oit_end;  int count = 0;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, g); oit != oit_end; ++oit, count++)
			{
				//cerr << *oit ;
				int targ = target(*oit, g);
				if(count >= pick && getOutDegree(targ, g) > 2)
				{					
					//cerr << endl << "Removing " << *oit << endl;
					remove_edge(oit, g);
					break;
				}
			}
			deg = getOutDegree(*vi, g);
		}
	}
	
	//cerr << WRITE_FUNCTION<< endl;
	//print_graph(g, get(vertex_index, g));
	//char c = getchar();
}
*/
/*-----------------------------------------------------------------------
	kRandomLinksRemoveGraph():		
		receives a global complete graph, removes k out edges for each
	vertex. MINIMUM getOutDegree is min.
----------------------------------------------------------------------- */
/*void
MeshConstruct::kRandomLinksRemoveMinOutDegreeGraph(GraphN& g, int k, int min)
{
	vertex_iteratorN vi, vi_end;
    for(boost::tuples::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
	{
		int deg = getOutDegree(*vi, g);
		//cerr << *vi << " out-degree " << deg << endl;
		for(int i = 0; i < k && deg > min; i++)
		{
			int pick = generateRandomNumber((double) deg);
			//cerr << "Pick " << pick << endl;

			out_edge_iteratorN oit, oit_end;  int count = 0;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, g); oit != oit_end; ++oit, count++)
			{
				//cerr << *oit ;
				int targ = target(*oit, g);
				if(count >= pick && getOutDegree(targ, g) > min)
				{					
					//cerr << endl << "Removing " << *oit << endl;
					remove_edge(oit, g);
					break;
				}
			}

			deg = getOutDegree(*vi, g);
		}
	}
	
	//cerr << WRITE_FUNCTION << endl;
	//print_graph(g, get(vertex_index, g));
	//char c = getchar();
}
*/
/*-----------------------------------------------------------------------
	kRandomLinksRemoveGraph():		
		receives a global complete graph, removes k out edges for each
	vertex. MINIMUM getOutDegree is min. MAXIMUM getOutDegree is max.
----------------------------------------------------------------------- */
/*void
MeshConstruct::kRandomLinksRemoveMinMaxOutDegreeGraph(GraphN& g, int k, int min, int max)
{
	vertex_iteratorN vi, vi_end;
    for(boost::tuples::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
	{
		int deg = getOutDegree(*vi, g);
		//cerr << *vi << " out-degree " << deg << endl;
		
		for(int i = 0; deg > min; i++)
		{
			int pick = generateRandomNumber((double) deg);
			//cerr << "Pick " << pick << endl;
			
			out_edge_iteratorN oit, oit_end;  int count = 0;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, g); oit != oit_end; ++oit, count++)
			{
				cerr << *oit ;
				int targ = target(*oit, g);
				if(count >= pick && getOutDegree(targ, g) > min)
				{					
					//cerr << endl << "Removing " << *oit << endl;
					remove_edge(oit, g);
					break;
				}
			}
			
			deg = getOutDegree(*vi, g);
			//cerr << "Vertex " << *vi << " i " << i << " deg " << deg << " max " << max << endl;
			if( i >= k && deg <= max || count >= deg) break;
		}
	}
	
	//cerr << WRITE_FUNCTION << endl;
	//print_graph(g, get(vertex_index, g));
	//char c = getchar();
}
*/

/*-----------------------------------------------------------------------
	kRandomLinksAddGraph():		
		receives a global complete graph, tries to add k out edges for each
	vertex. MINIMUM getOutDegree is k. MAXIMUM getOutDegree is num_vertices.
----------------------------------------------------------------------- */
/*void
MeshConstruct::kRandomLinksAddGraph(GraphN& g, int k)
{
	GraphN newG(num_vertices(g));

	vertex_iteratorN vi, vi_end;
    for(boost::tuples::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
	{
		int deg = getOutDegree(*vi, g);
		//cerr << *vi << " out-degree " << deg << endl;
		for(int i = 0; i < k && deg > 0; i++)
		{
			bool addedVertex = false;
			out_edge_iteratorN oit, oit_end;  
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, g); oit != oit_end; ++oit)
			{
				cerr << *oit ;
				int targ = target(*oit, g);
				pair<edge_descriptorN, bool> eG = edge(*vi, targ, newG);
				
				//if(eG.second == true) 
				//	cerr << "Edge Already in newG: " << eG.first << endl;
				
				if(generateRandomNumber(3.0) && eG.second == false)
				{				
					//cerr << endl << "Adding " << *oit << endl;
					edge_descriptorN e = add_edge(*vi, targ, newG).first;
					newG[*vi] = g[*vi];
					newG[targ] = g[targ];
					newG[e] = g[*oit];
					addedVertex = true;
					break;
				}
			}
			
			if(addedVertex == false) // couldn't add randomly, then add whatever it gets
			{
                for(boost::tuples::tie(oit, oit_end) = out_edges(*vi, g); oit != oit_end; ++oit)
				{
					cerr << *oit ;
					int targ = target(*oit, g);
					pair<edge_descriptorN, bool> eG = edge(*vi, targ, newG);

					//if(eG.second == true) 
					//	cerr << "Edge Already in newG: " << eG.first << endl;

					if(eG.second == false)
					{				
						//cerr << endl << "Adding " << *oit << endl;
						edge_descriptorN e = add_edge(*vi, targ, newG).first;
						newG[*vi] = g[*vi];
						newG[targ] = g[targ];
						newG[e] = g[*oit];
						break;
					}
				}
			}
		}
	}
	
	g = newG;
	
	//cerr << WRITE_FUNCTION << endl;
	//print_graph(g, get(vertex_index, g));
	//char c = getchar();	
}


void
MeshConstruct::minWeightkConnectedSubGraphs(GraphN& g, int k)
{


}
*/


