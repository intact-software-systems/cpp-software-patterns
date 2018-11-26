/***************************************************************************
                          treealgs.cc  -  description
                             -------------------
    begin                : Tue Jan 31 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <functional>
#include <fstream>
#include <sys/types.h>

#include "treealgs.h"
#include "../treealgs/dijkstra_sp.h"
#include "../treealgs/prim_mst.h"
#include "../treealgs/prim_ottc.h"
#include "../treealgs/prim_rgh_bdmst.h"
#include "../treealgs/prim_mddbst.h"
#include "../treealgs/prim_bdrbst.h"
#include "../treealgs/fheap.h"
#include "../simtime.h"
#include "../simdefs.h"
//#include "../debug.h"
#include "../graphalgs/steiner_set.h"
#include "../graphalgs/graphalgs.h"
#include "../graphalgs/meshalgs.h"
#include "../smtalgs/sph.h"
#include "../smtalgs/rsph.h"
#include "../smtalgs/dlrsph.h"
#include "../smtalgs/dsph.h"
#include "../smtalgs/dlsph_bauer.h"
#include "../smtalgs/mdsph.h"
#include "../smtalgs/mddlsph.h"
#include "../smtalgs/bdsph.h"
#include "../smtalgs/bddlsph.h"
#include "../smtalgs/adh.h"
#include "../smtalgs/dnh.h"
#include "../smtalgs/dldnh.h"
#include "../smtalgs/diameter-dnh.h"
#include "../smtalgs/rcsph.h"
#include "../smtalgs/rcdlsph.h"
#include "../smtalgs/dcsp.h"
#include "../smtalgs/dcdbsp.h"
#include "../smtalgs/dcdbsph.h"
#include "../smtalgs/dcsph.h"
#include "../network/group_info.h"

#include "../graphviz/graphviz.h"
#include "../graphstats/graphstats.h"
#include "../graphstats/recordtree.h"

using namespace std;
using namespace boost;

namespace TreeAlgorithms
{
/*-----------------------------------------------------------------------

			Namespace TreeAlgorithms: buildTree(), start function

------------------------------------------------------------------------- */
bool buildTree(const TreeStructure &inputT, TreeStructure& outT, TreeAlgo treeAlgo, int src, clock_t &alg_time, uint64_t &cpu_cycles)
{
#ifdef SYSTEM_SUPPORTS_PUSH_POP
    uint64_t a, b;
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

	alg_time = buildTree(inputT, outT, treeAlgo, src);
	
#ifdef SYSTEM_SUPPORTS_PUSH_POP
	__asm __volatile(".byte 0x0f,0x31" : "=A" (b));
	cpu_cycles = b - a;
#endif

	return (isValidTree(outT, inputT));
}
		
clock_t buildTree(const TreeStructure &inputT, TreeStructure& outT, TreeAlgo treeAlgo, int src)
{
	// -- start debug --
	//cerr << WRITE_FUNCTION << treeAlgo << " src " << src << endl; 
	ASSERTING(!inputT.V.empty());
	// -- end debug --
		
	clock_t start_time = ::clock();		// start algorithm timer
	
	// 	-- Execute Tree Algorithm --
	if(inputT.V.size() == 1)
	{
		outT = inputT;
	}
	else if(treeAlgo == SHORTEST_PATH_HEURISTIC) 
	{
		ShortestPathHeuristic sph(inputT, outT);	
		sph.Algorithm(src);	 
	}
	else if(treeAlgo == MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC)
	{
		MDShortestPathHeuristic mdsph(inputT, outT);
		mdsph.Algorithm(src);
	}
	else if(treeAlgo == MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	{
		MDDLShortestPathHeuristic mddlsph(inputT, outT);
		mddlsph.Algorithm(src);
	}
	else if(treeAlgo == BOUNDED_DIAMETER_OTTC_SHORTEST_PATH_HEURISTIC)
	{
		BDShortestPathHeuristic bdsph(inputT, outT);
		bdsph.Algorithm(src, GlobalSimArgs::getDiameterBound());
	}
	else if(treeAlgo == BOUNDED_DIAMETER_DEGREE_LIMITED_OTTC_SHORTEST_PATH_HEURISTIC)
	{
		BDDLShortestPathHeuristic bddlsph(inputT, outT);
		bddlsph.Algorithm(src, GlobalSimArgs::getDiameterBound());
	}
	else if(treeAlgo == BOUNDED_DIAMETER_RGH_SHORTEST_PATH_HEURISTIC || 
			treeAlgo == BOUNDED_DIAMETER_RH_SHORTEST_PATH_HEURISTIC)
	{
		RandomizedSPH rsph(inputT, outT);
		rsph.Algorithm(src, GlobalSimArgs::getDiameterBound());
	}
	else if(treeAlgo == BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_SHORTEST_PATH_HEURISTIC) // ||	treeAlgo == BOUNDED_DIAMETER_DEGREE_LIMITED_RH_SHORTEST_PATH_HEURISTIC)
	{
		DLRandomizedSPH dlrsph(inputT, outT);
		dlrsph.Algorithm(src, GlobalSimArgs::getDiameterBound());
	}
	else if(treeAlgo == AVERAGE_DISTANCE_HEURISTIC) 
	{
		//GraphN dg;
		//GraphAlgorithms::densifyGraph(inputT, dg);
		//inputT.g = dg;
		//start_time = ::clock();
		
		AverageDistanceHeuristic adh(inputT, outT);
		adh.Algorithm(src);
		
		//start_time = ::clock() - start_time;
		//GraphN sg;
		//GraphAlgorithms::sparsifyGraph(outT.g, sg);
		//outT.g = sg;
		//TreeAlgorithms::updateTreeIdDenseToSparse(outT);
		//return start_time;
	}
	else if(treeAlgo == RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC)
	{
		RCShortestPathHeuristic rcsph(inputT, outT);
		rcsph.Algorithm(src, GlobalSimArgs::getDiameterBound()/2 );
	}
	else if(treeAlgo == RADIUS_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	{
		RCDLShortestPathHeuristic rcsph(inputT, outT);
		rcsph.Algorithm(src, GlobalSimArgs::getDiameterBound()/2 );
	}
	else if(treeAlgo == DELAY_CONSTRAINED_SHORTEST_PATH ||  	// steiner version
			treeAlgo == DELAY_CONSTRAINED_SHORTEST_PATH_TREE)   // member version
	{
		DelayConstrainedShortestPath dcsp(inputT, outT);
		dcsp.Algorithm(src, GlobalSimArgs::getDelayConstraint()); 
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH ||  	// steiner version
			treeAlgo == DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_TREE)   // member version
	{
		DCDBShortestPath dcdbsp(inputT, outT);
		dcdbsp.Algorithm(src, GlobalSimArgs::getDelayConstraint()); 
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC)
	{
		DCShortestPathHeuristic dcsph(inputT, outT);
		dcsph.Algorithm(src, GlobalSimArgs::getDelayConstraint()); 
	}
	else if(treeAlgo == DELAY_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	{
		DCDBSPH dcdbsph(inputT, outT);
		dcdbsph.Algorithm(src, GlobalSimArgs::getDelayConstraint()); 
	}
	else if(treeAlgo == DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC)
	{
		DLShortestPathHeuristic dlsph(inputT, outT);
		dlsph.Algorithm(src);
	}
	else if(treeAlgo == DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_BAUER)
	{
		DLBauerShortestPathHeuristic dlsph_bauer(inputT, outT);
		dlsph_bauer.Algorithm(src);
	}
	else if(treeAlgo == BOUNDED_DIAMETER_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == BOUNDED_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == BOUNDED_DIAMETER_RGH_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == BOUNDED_DIAMETER_DEGREE_LIMITED_RGH_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == MINIMUM_DIAMETER_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == MINIMUM_DIAMETER_DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == DEGREE_LIMITED_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == DEGREE_LIMITED_SHORTEST_PATH_DISTANCE_NETWORK_HEURISTIC ||
			treeAlgo == DISTANCE_NETWORK_HEURISTIC)
	{
		DiameterDNH ddnh(inputT, outT);
		ddnh.Algorithm(src);
	}
	else if(treeAlgo == COMPACT_TREE)
	{
		PrimMDDBST MDDB(outT);
		MDDB.Algorithm(inputT, src, GlobalSimArgs::getDegreeConstraint());		
	}
	else if(treeAlgo == BOUNDED_COMPACT_TREE)
	{
		PrimBDRBST BDRB(outT);
		BDRB.Algorithm(inputT, src, GlobalSimArgs::getDegreeConstraint(), 4);
	}
	else if(treeAlgo == PRUNED_COMPACT_TREE)
	{
		PrimMDDBST MDDB(outT);
		MDDB.Algorithm(inputT, src, GlobalSimArgs::getDegreeConstraint());
		pruneNonTerminalLeaves(outT);
	} 
	else if(treeAlgo == MINIMUM_SPANNING_TREE) 	
	{
		getPrimMST(inputT, outT, src);
	}
	else if(treeAlgo == DEGREE_LIMITED_MINIMUM_SPANNING_TREE) 	
	{
		getDBPrimMST(inputT, outT, src);
	}
	else if(treeAlgo == PRUNED_MINIMUM_SPANNING_TREE) 	
	{
		getPrimMST(inputT, outT, src);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == ONE_TIME_TREE_CONSTRUCTION_BDMST)
	{
		OTTC(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == MINIMUM_DIAMETER_ONE_TIME_TREE_CONSTRUCTION)
	{
		mdOTTC(inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION)
	{
		mddlOTTC(inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == MINIMUM_DIAMETER_DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_STEINER)
	{
		mddlOTTC(inputT, outT, src, inputT.V);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == RANDOMIZED_GREEDY_HEURISTIC_BDMST)
	{
		RGH(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST)
	{
		dlOTTC(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == DEGREE_LIMITED_ONE_TIME_TREE_CONSTRUCTION_BDMST_STEINER)
	{
		dlOTTC(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST)
	{
		dlRGH(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
	}
	else if(treeAlgo == DEGREE_LIMITED_RANDOMIZED_GREEDY_HEURISTIC_BDMST_STEINER)
	{
		dlRGH(GlobalSimArgs::getDiameterBound(), inputT, outT, src, inputT.V);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == PRUNED_DEGREE_LIMITED_MINIMUM_SPANNING_TREE) 	
	{
		getDBPrimMST(inputT, outT, src);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == SHORTEST_PATH_TREE)
	{
		getDijkstraSPT(inputT, outT, src);
	}
	else if(treeAlgo == SHORTEST_PATH_TREE_STEINER)
	{
		getDijkstraSPT(inputT, outT, src);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == DEGREE_LIMITED_SHORTEST_PATH_TREE)
	{
		getDBDijkstraSPT(inputT, outT, src);
	}
	else if(treeAlgo == DEGREE_LIMITED_SHORTEST_PATH_TREE_STEINER)
	{
		getDBDijkstraSPT(inputT, outT, src);
		pruneNonTerminalLeaves(outT);
	}
	else if(treeAlgo == SERVER_SHORTEST_PATH_TREE || treeAlgo == PROXY_SHORTEST_PATH_TREE)
	{
		getServerSPT(inputT, outT, src);
	}
	else ASSERTING(0);
	
	if(!outT.S.empty()) pruneNonTerminalLeaves(outT);

	return (::clock() - start_time); 	// execution time
}

void recordStats(iostream& ostr, StatsContainer *SC)
{
	Statistics::recordTree(ostr, *SC);
}

void flushStats(iostream& ostr)
{
	if(!GlobalSimArgs::getSimStatFile())
	{
		ostr.flush();
		return;
	}		
	ofstream outfile(GlobalSimArgs::getSimStatFile(), ios::out | ios::app);
	outfile << ostr.rdbuf();
	outfile.close();

	//stattofile.write(ostr.rdbuf(), sizeof(ostr.rdbuf()));
	//stattofile.flush();
}

bool isValidTree(const TreeStructure &outT, const TreeStructure &inputT)
{
	bool valid = true;
	if((outT.V - outT.S) != (inputT.V - inputT.S)) 
	{
		valid = false;
		cerr << " Missing group member in tree : " << (inputT.V - inputT.S) - (outT.V - outT.S) << endl;
	}
	if(outT.E.size() != (outT.V.size() - 1)) valid = false;

	return valid;
}

bool isValidMesh(const TreeStructure &outT, const TreeStructure &inputT)
{
	if((outT.V - outT.S) != (inputT.V - inputT.S)) 
	{
		cerr << " Missing group member in tree : " << (inputT.V - inputT.S) - (outT.V - outT.S) << endl;
		return false;
	}
	if(outT.E.size() < (outT.V.size() - 1)) return false;
	if(outT.V.size() > 1)
	{
		if(!checkIsPartitioned(outT.g)) return false;
	}

	return true;
}

int meshifyGraph(GraphN &g, VertexSet &V)
{
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(g, *vit) > 0) V.insert(*vit);
	}

	int vertex_degree = V.size() - 1;
	int num_edges_added = 0;
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		int src = *vit;
		VertexSet outV;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(src, g); oit != oit_end; ++oit)
		{	
			vertex_descriptorN targ = target(*oit, g);
			ASSERTING(targ != src);
			outV.insert(targ);		

			if(g[*oit].weight <= 0.0001) g[*oit].weight = 1.0f;
		}
		
		VertexSet remV = V - outV - src;

		for(VertexSet::iterator it = remV.begin(), it_end = remV.end(); it != it_end; ++it)
		{	
			int targ = *it;

			if(isEdge(g, src, targ)) continue;
			if(src == targ) continue;

			/*if(!isVertex(g, src)) 
			{
				addVertex(g, src);
				g[src].id = src;
			}
			if(!isVertex(g, targ)) 
			{
				addVertex(g, targ);
				g[targ] = VertexProp(targ, CLIENT);
			}*/
			pair<edge_descriptorN, bool> e = add_edge(src, targ, g);
			ASSERTING(e.second);
			
			num_edges_added++;

			EdgeProp ep;
			ep.id = pair<int, int>(src, targ);
			ep.delay = 1.0f; 
			ep.weight = 1.0f; 
			ep.bw = 1000.0; 
			ep.cost = 1.0;
			ep.length = 1.0;
			ep.hops = 1;	
			
			g[e.first] = ep;
		}

		ASSERTING(getOutDegree(src, g) == vertex_degree);
	}
	
	return num_edges_added;
}

void addVertex(GraphN &g, vertex_descriptorN v)
{
	int num = num_vertices(g);
	if(num <= v)
	{
		ASSERTING(num <= v);
		vertex_descriptorN ret;	
		for(int i = num; i <= v; i++)
			ret = add_vertex(g);	
		ASSERTING(ret == v);
	}
}

/*-----------------------------------------------------------------------
					Basic Tree Algorithms
------------------------------------------------------------------------- */

void getPrimMST(const TreeStructure &inputT, TreeStructure &outT, int source)
{
	ASSERTING(num_vertices(inputT.g) > 2);
	ASSERTING(num_edges(inputT.g) > 1);
	
    DistanceVector 	zdistance(num_vertices(inputT.g));
	ParentVector 	zparent(num_vertices(inputT.g));

	//prim_minimum_spanning_tree(g, source, &zparent[0], &zdistance[0], get(&EdgeProp::weight, g), get(vertex_index, g), default_dijkstra_visitor());
	myPrimMST(inputT.g, source, zdistance, zparent); //, inputT.V);
	
	outT.insertVertex(source, inputT.g);
	createTreeFromParent(inputT.g, outT, zparent);
	
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(outT);
}

void getDBPrimMST(const TreeStructure &inputT, TreeStructure &outT, int source)
{
	ASSERTING(num_vertices(inputT.g) > 2);
	ASSERTING(num_edges(inputT.g) > 1);
	
    DistanceVector 	zdistance(num_vertices(inputT.g));
	ParentVector 	zparent(num_vertices(inputT.g));

	//prim_minimum_spanning_tree(g, source, &zparent[0], &zdistance[0], get(&EdgeProp::weight, g), get(vertex_index, g), default_dijkstra_visitor());
	myDBPrimMST(inputT.g, source, zdistance, zparent, inputT.V);
	
	outT.insertVertex(source, inputT.g);
	createTreeFromParent(inputT.g, outT, zparent);
	
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(outT);
}


void getDijkstraSPT(const TreeStructure &inputT, TreeStructure &outT, int source)
{
    ASSERTING(num_vertices(inputT.g) > 1);
    ASSERTING(num_edges(inputT.g) > 0);

	DistanceVector 	zdistance(num_vertices(inputT.g));
	ParentVector 	zparent(num_vertices(inputT.g));

	outT.insertVertex(source, inputT.g);
	
	if(num_edges(inputT.g) == 1)
	{		
		out_edge_iteratorN oit, oit_end;
        boost::tuples::tie(oit, oit_end) = out_edges(source, inputT.g);
		ASSERTING(oit != oit_end);
		
		int targ = target(*oit, inputT.g);
		
		outT.insertVertex(targ, inputT.g);
		outT.insertEdge(source, targ, inputT.g);
	}
	else // run dijkstra
	{ 
		//dijkstra_shortest_paths(g, source, weight_map(get(&EdgeProp::weight, g)).distance_map(&zdistance[0]).predecessor_map(&zparent[0]));
		myDijkstraSPT_TieBr(inputT.g, source, zdistance, zparent, inputT.V);
		createTreeFromParent(inputT.g, outT, zparent);

		//cerr << "Src: " << source << " Tree : " << outT << endl;
		//dumpGraph(outT.g);
		//ASSERTING(getOutDegree(source, outT.g) == outT.Ep.size());
	}
}

void getDBDijkstraSPT(const TreeStructure &inputT, TreeStructure &outT, int source)
{
    ASSERTING(num_vertices(inputT.g) > 1);
    ASSERTING(num_edges(inputT.g) > 0);

	DistanceVector 	zdistance(num_vertices(inputT.g));
	ParentVector 	zparent(num_vertices(inputT.g));

	outT.insertVertex(source, inputT.g);
	
	if(num_edges(inputT.g) == 1)
	{		
		out_edge_iteratorN oit, oit_end;
        boost::tuples::tie(oit, oit_end) = out_edges(source, inputT.g);
		ASSERTING(oit != oit_end);
				
		int targ = target(*oit, inputT.g);
		
		outT.insertVertex(targ, inputT.g);
		outT.insertEdge(source, targ, inputT.g);
	}
	else // run dijkstra
	{ 
		//dijkstra_shortest_paths(g, source, weight_map(get(&EdgeProp::weight, g)).distance_map(&zdistance[0]).predecessor_map(&zparent[0]));
		myDBDijkstraSPT(inputT.g, source, zdistance, zparent, inputT.V);
		createTreeFromParent(inputT.g, outT, zparent);
	}
}

void getServerSPT(const TreeStructure &inputT, TreeStructure &outT, int source)
{
    ASSERTING(num_vertices(inputT.g) > 1);
    ASSERTING(num_edges(inputT.g) > 0);

	//cerr << WRITE_FUNCTION << " source in tree : " << source << endl;

	outT.insertVertex(source, inputT.g);

	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(source, inputT.g); oit != oit_end; ++oit)
	{
		int targ = target(*oit, inputT.g);
		assert(targ != source);
		outT.insertVertex(targ, inputT.g);
		outT.insertEdge(source, targ, inputT.g);
	}
}


void getProxySPT(const TreeStructure &inputT, TreeStructure &outT, int source)
{
    ASSERTING(num_vertices(inputT.g) > 1);
    ASSERTING(num_edges(inputT.g) > 0);

	outT.insertVertex(source, inputT.g);

	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(source, inputT.g); oit != oit_end; ++oit)
	{
		int targ = target(*oit, inputT.g);
		assert(targ != source);
		outT.insertVertex(targ, inputT.g);
		outT.insertEdge(source, targ, inputT.g);
	}
}

void getServerSPT(const GraphN &g, TreeStructure &outT, int source)
{
	outT.insertVertex(source, g);

	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(source, g); oit != oit_end; ++oit)
	{
		int targ = target(*oit, g);
		assert(targ != source);

		outT.insertVertex(targ, g);
		outT.insertEdge(source, targ, g);
	}
}

/*-----------------------------------------------------------------------
			makes index == id,	only used by ADH
------------------------------------------------------------------------- */
void updateTreeIdDenseToSparse(TreeStructure& T)
{
	VertexSet newV, newS;
	EdgeList newE;

	vsVertexMap vsmap = get(&VertexProp::vertexState, T.g);
	
	EdgePair::iterator peit = T.Ep.begin(), peit_end = T.Ep.end();
	for( ; peit != peit_end; ++peit)
	{
		pair<edge_descriptorN, bool> e = edge(peit->first, peit->second, T.g);
		ASSERTING(e.second);
		newE.push_back(e.first);

		int src = source(e.first, T.g), targ = target(e.first, T.g);
		newV.insert(src);
		newV.insert(targ);
		if(vsmap[src] == STEINER_POINT)	newS.insert(src);
		if(vsmap[targ] == STEINER_POINT) newS.insert(targ);

		//cerr << e.first << " " ;		
		//cerr << *peit << " src " << src << " targ " << targ << endl;
	}
		
	// update set/list
	T.V = newV;
	T.S = newS;
	T.E = newE;	
	
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(T);
}

/*-----------------------------------------------------------------------
  						Prune function
------------------------------------------------------------------------- */
/*void pruneNonTerminalLeaves(TreeStructure &T)
{
	bool foundLeaf = true;
	while(foundLeaf)
	{
		foundLeaf = false;
		VertexSet pruneSet;
		for(VertexSet::iterator vit = T.S.begin(), vit_end = T.S.end(); vit != vit_end; ++vit)
		{
			ASSERTING(get(&VertexProp::vertexState, T.g)[*vit] == STEINER_POINT);
			if(getOutDegree(*vit, T.g) <= 1) 
			{
				pruneSet.insert(*vit);
				foundLeaf = true;
			}
		}

		for(VertexSet::iterator vit = pruneSet.begin(), vit_end = pruneSet.end(); vit != vit_end; ++vit)
		{
			//cerr << "Pruning " << *vit << endl;
			T.removeVertex(*vit);
		}
	}
	//cerr << WRITE_FUNCTION << " Members " << T.V - T.S << endl;
	//cerr << " non-terminals " << T.S << endl;
	//dumpTreeStructure(T);
	//char c = getchar();
}
*/

void pruneNonTerminalLeaves(TreeStructure &T)
{
	//cerr << WRITE_PRETTY_FUNCTION << "steiner set " << T.S << endl;
	VertexSet remV;
	
	VertexSet::iterator vit = T.S.begin(), vit_end = T.S.end();
	for( ; vit != vit_end; ++vit)
	{
		int sp = *vit;
		//cerr << sp << ": " << endl;

		// -- debug --
		ASSERTING(T.g[sp].vertexState == STEINER_POINT);
		// -- debug end --
		if(getOutDegree(T.g, sp) == 0) 
		{
			//cerr << "Outdegree 0 " << endl;
			remV.insert(sp);
		}
		else if(getOutDegree(T.g, sp) == 1) 
		{
			//cerr << "Removing sp: " << sp << endl;
			//cerr << "Removing edge(s): " ; 
			out_edge_iteratorN oit, oit_end;
			while(getOutDegree(T.g, sp) == 1 && T.g[sp].vertexState == STEINER_POINT) 
			{
                boost::tuples::tie(oit, oit_end) = out_edges(sp, T.g);
				remV.insert(sp);
				sp = target(*oit, T.g);
				//cerr << *oit << " " ;
				T.removeEdge(*oit);
			}
			//cerr << endl;
		}
	}

	//if(!remV.empty())
	//	cerr << WRITE_PRETTY_FUNCTION << " removing vertices : " << remV << " from tree : " << T << endl;
	
	// new steiner set
	vit = remV.begin();
	vit_end = remV.end();
	for( ; vit != vit_end; ++vit)
	{
		ASSERTING(T.g[*vit].vertexState == STEINER_POINT);
		T.removeVertex(*vit);
	}
}

/*-----------------------------------------------------------------------
		createTreeFromParent: creates a tree from parent vector
------------------------------------------------------------------------- */
void createTreeFromParent(const GraphN &g, TreeStructure& treeStructure, const ParentVector &zparent)
{
	ASSERTING(zparent.size() > 0);

	for(size_t i = 0; i != zparent.size(); ++i)
	{
		pair<edge_descriptorN, bool> e = edge(i, zparent[i], g);
		if(e.second)
		{
			//cerr << "[createTree] Parent [" << i << "] = " << zparent[i] << endl;
			int src = source(e.first, g), targ = target(e.first, g);
			treeStructure.insertVertex(i, g);
			treeStructure.insertEdge(src, targ, g);
		}
	}
}

/*-----------------------------------------------------------------------
	createGraphFromTree: creates a boost graph from treestructure
------------------------------------------------------------------------- */
void createGraphFromTree(GraphN& gOut, const TreeStructure &T)
{
	EdgeList::const_iterator eit = T.E.begin(), eit_end = T.E.end();
	for( ; eit != eit_end; ++eit)
	{
		int src = source(*eit, T.g), targ = target(*eit, T.g);

		pair<edge_descriptorN, bool> ep = edge(src, targ, T.g);
		ASSERTING(ep.second);
		
		pair<edge_descriptorN, bool> e = add_edge(src, targ, gOut);
		ASSERTING(e.second);
		
		gOut[e.first] = EdgeProp(T.g[ep.first]);
		gOut[src] 	  = VertexProp(T.g[src]);
		gOut[targ] 	  = VertexProp(T.g[targ]);
	}
}
/*-----------------------------------------------------------------------
					relax functions
------------------------------------------------------------------------- */
bool isRelaxWorking(const VertexSet &degreeBroken, const VertexSet &diameterBroken)
{
	if(GlobalSimArgs::getRelaxDiameter() <= 0 && degreeBroken.empty()) return false;
	if(GlobalSimArgs::getRelaxDegree() <= 0 && diameterBroken.empty()) return false;
	
	if(degreeBroken.empty() && diameterBroken.empty()) 
	{
		cerr << WRITE_FUNCTION << " relaxing did not make more nodes available " << endl;
		return false;
	}

	return true;
}

bool relaxDegreeAndDiameter(const VertexSet &degreeBroken, DistanceVector &degree_bound, const VertexSet &diameterBroken, DistanceVector &diameter_bound)
{
	bool ret1 = relaxDegree(degreeBroken, degree_bound);
	bool ret2 = relaxDiameter(diameterBroken, diameter_bound);
	if(false == ret1 && false == ret2) return false;

	//if(!isRelaxWorking(degreeBroken, diameterBroken)) return false;

	return true;
}

bool relaxDegreeAndRadius(const VertexSet &degreeBroken, DistanceVector &degree_bound, const VertexSet &radiusBroken, DistanceVector &radius_bound)
{
	bool ret1 = relaxDegree(degreeBroken, degree_bound);
	bool ret2 = relaxRadius(radiusBroken, radius_bound);
	if(false == ret1 && false == ret2) return false;

	return true;
}

bool relaxDegreeAndRadius(const VertexSet &degreeBroken, DistanceVector &degree_bound, double &bound)
{
	bool ret1 = relaxDegree(degreeBroken, degree_bound);
	bool ret2 = relaxRadius(bound);
	if(false == ret1 && false == ret2) return false;

	return true;
}
bool relaxDegree(const VertexSet &setBroken, DistanceVector &bounds)
{
	if(!GlobalSimArgs::getRelaxDegree()) return false;

	relaxSet(setBroken, bounds, GlobalSimArgs::getRelaxDegree());
	return true;
}


bool relaxDegree(const VertexSet &setBroken, ParentVector &bounds)
{
	if(!GlobalSimArgs::getRelaxDegree()) return false;

	relaxSet(setBroken, bounds, GlobalSimArgs::getRelaxDegree());
	return true;
}

bool relaxDelay(const VertexSet &setBroken, DistanceVector &bounds)
{
	if(!GlobalSimArgs::getRelaxDelay()) return false;

	relaxSet(setBroken, bounds, GlobalSimArgs::getRelaxDelay());
	return true;
}

bool relaxRadius(const VertexSet &setBroken, DistanceVector &bounds)
{
	if(!GlobalSimArgs::getRelaxDiameter()) return false;

	relaxSet(setBroken, bounds, GlobalSimArgs::getRelaxDiameter()/2);
	return true;
}

bool relaxRadius(double &bound)
{
	if(!GlobalSimArgs::getRelaxDiameter()) return false;

	bound = bound + GlobalSimArgs::getRelaxDiameter()/2;

	return true;
}

bool relaxDiameter(const VertexSet &setBroken, DistanceVector &bounds)
{
	if(!GlobalSimArgs::getRelaxDiameter()) return false;

	relaxSet(setBroken, bounds, GlobalSimArgs::getRelaxDiameter());
	return true;
}

bool relaxDiameter(double &bound)
{
	if(!GlobalSimArgs::getRelaxDiameter()) return false;

	bound = bound + GlobalSimArgs::getRelaxDiameter();

	return true;
}

void relaxSet(const VertexSet &setBroken, DistanceVector &bounds, double inc)
{
	for(VertexSet::iterator vit = setBroken.begin(), vit_end = setBroken.end(); vit != vit_end; ++vit)
	{
		bounds[*vit] = bounds[*vit] + inc;
		//cerr << "(" << *vit << " : " << bounds[*vit] << ") " ;
	}
}

void relaxSet(const VertexSet &setBroken, ParentVector &bounds, int inc)
{
	for(VertexSet::iterator vit = setBroken.begin(), vit_end = setBroken.end(); vit != vit_end; ++vit)
	{
		bounds[*vit] = bounds[*vit] + inc;
		//cerr << "(" << *vit << " : " << degree_bound[*vit] << ") " ;
	}
}
/*-----------------------------------------------------------------------
					basic/simple get functions
------------------------------------------------------------------------- */

int getAvOutDegree(const GraphN &g, const TreeStructure &T, int u)
{
	return (getDegreeConstraint(g, u) - getOutDegree(T.g, u)); 
}

int getAvOutDegree(const GraphN &g, const TreeStructure &T, const VertexSet &V)
{
	int ODAvNeighbors = 0;
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
		ODAvNeighbors += (getDegreeConstraint(g, *vit) - getOutDegree(T.g, *vit));

	return ODAvNeighbors; 
}

int getVertLargestAvOD(const GraphN &g, const TreeStructure &T, const VertexSet &V)
{
	int MaxOD = 0;
	int vert = -1;
	for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		int ODAvNeighbors = (getDegreeConstraint(g, *vit) - getOutDegree(T.g, *vit));
		if(MaxOD < ODAvNeighbors) vert = *vit;
	}

	return vert;
}

int getMinODNeedConnectV(int numV)
{
	return ((numV - 1) * 2);
}

double getTotalCost(const TreeStructure &T)
{
	double totalCost = 0;
	EdgeList::const_iterator eit = T.E.begin(), eit_end = T.E.end();
	for( ; eit != eit_end; ++eit)
	{
		int src = source(*eit, T.g), targ = target(*eit, T.g);
		pair<edge_descriptorN, bool> ep = edge(src, targ, T.g);
		assert(ep.second);
		totalCost = totalCost + T.g[ep.first].weight;
	}
	return totalCost;
}

/*-----------------------------------------------------------------------
		finds the worst case pair-wise distance for src in tree_g 
------------------------------------------------------------------------- */

double getTreeDiameter(const TreeStructure &T, int src) 
{
	if(GlobalSimArgs::insertMeshAlgo() != NO_DYNAMIC_MESH_ALGO || GlobalSimArgs::removeMeshAlgo() != NO_DYNAMIC_MESH_ALGO || GlobalSimArgs::meshAlgo() != NO_GRAPH_ALGO)
	{
		return GraphAlgorithms::eccentricity_distance(T, src);
	}
	else
	{
		PathList tree_path;
		return getTreeDiameter(T.g, T.V, tree_path); 
	}
}

double getTreeDiameter(const TreeStructure &T) 
{
	PathList tree_path;
	return getTreeDiameter(T.g, T.V, tree_path);
}

double getTreeDiameter(const TreeStructure &T, PathList &tree_path)
{
	return getTreeDiameter(T.g, T.V, tree_path); 
}

double getTreeDiameter(const GraphN &g, const VertexSet &V, PathList &tree_path)
{
	PathList path;
	double worstcase_dist = 0;
	
	int src = *(V.begin());
	ASSERTING(!V.empty());

	worstCaseDist(src, src, src, g, worstcase_dist, path);
	path.push_front(src);
	
	ASSERTING(!path.empty());
	
	// root r at node farthest away from src. r is a part of the diameter
	int r = *(path.rbegin());
	
	if(V.size() <= 2) 
	{
		tree_path = path;
		return worstcase_dist;
	}

	double tree_diameter = 0;
	worstCaseDist(r, r, r, g, tree_diameter, tree_path);
	tree_path.push_front(r);
	
	return tree_diameter;
}

/*-----------------------------------------------------------------------
		finds the worst case pair-wise distance for src in tree_g 
------------------------------------------------------------------------- */
void worstCaseDist(int src, int prev, int curr, const GraphN &tree_g, double &worstcase_dist, PathList &path)
{
    double 	temp1_wc_dist = 0, temp2_wc_dist = 0; 	// weight/distance
	PathList temp1_path, temp2_path;
	ASSERTING(isVertex(tree_g, curr));
		
	while(true)
	{
		for(pair<out_edge_iteratorN, out_edge_iteratorN> poeit = out_edges(curr, tree_g); poeit.first != poeit.second; ++poeit.first)
		{
			int targ = target(*(poeit.first), tree_g);
			if(targ == prev) continue;	 // don't go back 
								
			temp1_wc_dist = tree_g[*(poeit.first)].weight;
			temp1_path.push_back(targ);
			worstCaseDist(src, curr, targ, tree_g, temp1_wc_dist, temp1_path);

			if(temp2_wc_dist < temp1_wc_dist) 
			{
				temp2_wc_dist = temp1_wc_dist;
				temp2_path = temp1_path;
			}
	
			temp1_wc_dist = 0;
			temp1_path.clear();
		}
		if(curr != src) 
		{
			worstcase_dist += temp2_wc_dist;
			path.insert(path.end(), temp2_path.begin(), temp2_path.end());
		}
		else if(curr == src && temp2_wc_dist > worstcase_dist)	
		{
			worstcase_dist = temp2_wc_dist;
			path = temp2_path;
		}
		
		break;
	}
}


/*-----------------------------------------------------------------------
			finds the eccentricity for each node in V 
------------------------------------------------------------------------- */

void worstCaseDist(const GraphN &g, const VertexSet &V, TreeDiameter &TD, int src)
{
	TD.distance = DistanceVector(num_vertices(g));
	for(VertexSet::iterator it = V.begin(), it_end = V.end(); it != it_end; ++it)
	{
		double worstcase_dist = 0;
		if(getOutDegree(g, *it) > 0)
			worstCaseDist(*it, *it, *it, g, worstcase_dist);
		TD.distance[*it] = worstcase_dist;
	}			
}

/*-----------------------------------------------------------------------
			finds the worst pair wise distance from src
------------------------------------------------------------------------- */
void worstCaseDist(int src, int prev, int curr, const TreeStructure &tree, double &worstcase_dist)
{
	if(GlobalSimArgs::insertMeshAlgo() != NO_DYNAMIC_MESH_ALGO || GlobalSimArgs::removeMeshAlgo() != NO_DYNAMIC_MESH_ALGO || GlobalSimArgs::meshAlgo() != NO_GRAPH_ALGO)
	{
		worstcase_dist = GraphAlgorithms::eccentricity_distance(tree, src);
		//cerr << WRITE_FUNCTION << " eccentricity " << worstcase_dist << endl;
	}
	else
	{
		worstCaseDist(src, prev, curr, tree.g, worstcase_dist);
	}
}

void worstCaseDist(int src, int prev, int curr, const GraphN &tree_g, double &worstcase_dist)
{
	int targ = -1;
	bool done = false;
    double 	temp1_wc_dist = 0, temp2_wc_dist = 0; 	// weight/distance
	ASSERTING(isVertex(tree_g, curr));

	dEdgeMapConst wmap = get(&EdgeProp::weight, tree_g);
	
	while(!done)
	{
		int degree = getOutDegree(tree_g, curr);
		ASSERTING(degree > 0);
		
		if(degree >= 2)	// -- split -> pick one out and start search again --
		{
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(curr, tree_g); oit != oit_end; oit++)
			{
				targ = target(*oit, tree_g);
				//targ = getTarget(tree_g[*oit].id, curr);
				if(targ == prev) continue;	 // don't go back 
				
				//if(!isVertex(tree_g, targ))
				//{
					//dumpGraph(tree_g);
				//	cerr << " curr : " << curr << " targ : " << targ << " edge : " << *oit << " prop : " << tree_g[*oit] << " p: " << tree_g[targ] << endl;
				//}				
				ASSERTING(isVertex(tree_g, targ));
				
				temp1_wc_dist = wmap[*oit];
				worstCaseDist(src, curr, targ, tree_g, temp1_wc_dist);

				if(temp2_wc_dist < temp1_wc_dist) temp2_wc_dist = temp1_wc_dist;

				temp1_wc_dist = 0;
			}
		
			if(curr != src) worstcase_dist += temp2_wc_dist;
			else if(curr == src && temp2_wc_dist > worstcase_dist)	worstcase_dist = temp2_wc_dist;
			done = true;
		}
		else if(degree == 1) 	// -- leaf node --
		{
			out_edge_iteratorN oit, oit_end;
            boost::tuples::tie(oit, oit_end) = out_edges(curr, tree_g);
			ASSERTING(oit != oit_end);
			targ = target(*oit, tree_g);
		
			if(prev == targ) done = true;
			else worstcase_dist = worstcase_dist + wmap[*oit]; 
		}
		ASSERTING(isVertex(tree_g, curr));
		ASSERTING(isVertex(tree_g, targ));

		prev = curr; curr = targ;
	}
}

int getTarget(const EdgeId &id, int src)
{
	if(id.first == src) return id.second;
	
	ASSERTING(id.second == src);
	return id.first;
}

int getSource(const EdgeId &id, int targ)
{
	if(id.first == targ) return id.second;
	
	ASSERTING(id.second == targ);
	return id.first;
}

void worstCaseCost(int src, int prev, int curr, const GraphN &tree_g, double &worstcase_dist)
{
	pair<out_edge_iteratorN, out_edge_iteratorN> poeit;
	int targ;
	bool done = false;
    double 	temp1_wc_dist = 0, temp2_wc_dist = 0; 	// weight/distance
	ASSERTING(isVertex(tree_g, curr));

	dEdgeMapConst wmap = get(&EdgeProp::cost, tree_g);
	
	while(!done)
	{
		int degree = getOutDegree(tree_g, curr);
		ASSERTING(degree > 0);

		if(degree >= 2)	// -- split -> pick one out and start search again --
		{
			for(poeit = out_edges(curr, tree_g); poeit.first != poeit.second; ++poeit.first)
			{
				targ = target(*(poeit.first), tree_g);
				if(targ == prev) continue;	 // don't go back 
								
				temp1_wc_dist = wmap[*(poeit.first)];
				worstCaseCost(src, curr, targ, tree_g, temp1_wc_dist);

				if(temp2_wc_dist < temp1_wc_dist) temp2_wc_dist = temp1_wc_dist;

				temp1_wc_dist = 0;
			}
		
			if(curr != src) worstcase_dist += temp2_wc_dist;
			else if(curr == src && temp2_wc_dist > worstcase_dist)	worstcase_dist = temp2_wc_dist;
			done = true;
		}
		else if(degree == 1) 	// -- leaf node --
		{
			poeit = out_edges(curr, tree_g);
			targ = target(*(poeit.first), tree_g);
		
			if(prev == targ) done = true;
			else worstcase_dist = worstcase_dist + wmap[*(poeit.first)]; 
		}
		prev = curr; curr = targ;
	}
}

void worstCaseCost(int src, int prev, int curr, const GraphN &tree_g, pair<double, double> &worstcase_dist)
{
	pair<out_edge_iteratorN, out_edge_iteratorN> poeit;
	int targ;
	bool done = false;
    pair<double, double> temp1_wc_dist(0,0), temp2_wc_dist(0,0); 	// weight/distance
	ASSERTING(isVertex(tree_g, curr));

	dEdgeMapConst cmap = get(&EdgeProp::cost, tree_g); 		// .first
	dEdgeMapConst wmap = get(&EdgeProp::weight, tree_g); 	// .second
		
	while(!done)
	{
		int degree = getOutDegree(tree_g, curr);
		ASSERTING(degree > 0);

		if(degree >= 2)	// -- split -> pick one out and start search again --
		{
			for(poeit = out_edges(curr, tree_g); poeit.first != poeit.second; ++poeit.first)
			{
				targ = target(*(poeit.first), tree_g);
				if(targ == prev) continue;	 // don't go back 
								
				temp1_wc_dist.first = cmap[*(poeit.first)];
				temp1_wc_dist.second = wmap[*(poeit.first)];
				worstCaseCost(src, curr, targ, tree_g, temp1_wc_dist);

				if(temp2_wc_dist.first < temp1_wc_dist.first) temp2_wc_dist.first = temp1_wc_dist.first;
				if(temp2_wc_dist.second < temp1_wc_dist.second) temp2_wc_dist.second = temp1_wc_dist.second;
	
				temp1_wc_dist.first = 0; temp1_wc_dist.second = 0;
			}
		
			if(curr != src) 
			{
				worstcase_dist.first += temp2_wc_dist.first;
				worstcase_dist.second += temp2_wc_dist.second;
			}
			else
			{
				if(curr == src && temp2_wc_dist.first > worstcase_dist.first)	worstcase_dist.first = temp2_wc_dist.first;
				if(curr == src && temp2_wc_dist.second > worstcase_dist.second) worstcase_dist.second = temp2_wc_dist.second;
			}
			done = true;
		}
		else if(degree == 1) 	// -- leaf node --
		{
			poeit = out_edges(curr, tree_g);
			targ = target(*(poeit.first), tree_g);
		
			if(prev == targ) done = true;
			else 
			{
				worstcase_dist.first = worstcase_dist.first + cmap[*(poeit.first)]; 
				worstcase_dist.second = worstcase_dist.second + wmap[*(poeit.first)]; 
			}
		}
		prev = curr; curr = targ;
	}
}

/*-----------------------------------------------------------------------
		computes the worst case pair wise distance for each vertex
		in the tree, it is the MDDBST algorithm and O(n^3)
	TODO:  - a function that updates worst case distances if, say, only
	a single vertex has been added, or a few edges ...
------------------------------------------------------------------------- */
void 
wcVertexDistances(const TreeStructure &inputT, TreeDiameter& TD, int src)
{
	iVertexMapConst idmap = get(&VertexProp::id, inputT.g);
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(inputT.g));
	
	VertexSet V;
	TD.distance = DistanceVector(num_vertices(inputT.g));
	TD.parent = ParentVector(num_vertices(inputT.g));
	
	// -- Initalize --
	for(VertexSet::iterator vit = inputT.V.begin(), vit_end = inputT.V.end(); vit != vit_end; ++vit)
	{
		pair<edge_descriptorN, bool> ep = edge(src, *vit, inputT.g);
		if(ep.second) 			TD.distance[*vit] = inputT.g[ep.first].weight;	
		else if(*vit == src)	TD.distance[*vit] = 0;
		else  					TD.distance[*vit] = (std::numeric_limits<double>::max)();
	
		TD.parent[*vit] = src; // src is parent of each
		
		if(idmap[*vit] > -1) 
		{
			V.insert(*vit);		
			if(*vit != src)
			{
				heap->insert(*vit, TD.distance[*vit]);
			}
		}
	}
	
	TreeStructure T;
	T.insertVertex(src, inputT.g);
	V.erase(src);

	int num_vertices = V.size();
	while(T.V.size() < (num_vertices + 1)) 
	{
		int u = heap->deleteMin();
		
		// -- debug --
		assert(TD.parent[u] != u);
		// -- debug end --
	
		T.insertVertex(u, inputT.g);
		T.insertVertex(TD.parent[u], inputT.g);
		T.insertEdge(u, TD.parent[u], inputT.g);
		V.erase(u);
		
		// update distance for v
		VertexSet::iterator vit_in = T.V.begin(), vit_in_end = T.V.end();
		for( ; vit_in != vit_in_end; ++vit_in)
		{
			if(*vit_in == u) continue;
			TD.distance[*vit_in] = std::max(TD.distance[*vit_in], vertexDistance(T.g, TD.distance, TD.parent, u, *vit_in, src)); 
		}

		vit_in = V.begin(), vit_in_end = V.end();
		for( ; vit_in != vit_in_end; ++vit_in)
		{
			TD.distance[*vit_in] = MAXIMUM_WEIGHT;
			heap->decreaseKey(*vit_in, TD.distance[*vit_in]);
								
			VertexSet::iterator vert_it = T.V.begin(), vert_it_end = T.V.end();
			for( ; vert_it != vert_it_end; ++vert_it)
			{	
				pair<edge_descriptorN, bool> ep = edge(*vit_in, *vert_it, inputT.g);
				if(!ep.second) continue;
				edge_descriptorN e = ep.first;				
				int targ = target(e, inputT.g);
				
				if(inputT.g[e].weight + TD.distance[targ] < TD.distance[*vit_in])
				{
					TD.distance[*vit_in] = inputT.g[e].weight + TD.distance[targ];
					TD.parent[*vit_in] = targ;
					heap->decreaseKey(*vit_in, TD.distance[*vit_in]);
				}
			}
		}
	}
	
	delete heap;
}

double vertexDistance(const GraphN &g, const DistanceVector &rdistance, const ParentVector &rparent, int u, int v, int zsource)
{
	int traverse_vert = u;
	double dist_uv = 0;

	GraphN uv_g(rdistance.size());
	while(traverse_vert != v && traverse_vert != zsource)
	{
		pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], g);
		assert(ep.second);
		pair<edge_descriptorN, bool> e = add_edge(traverse_vert, rparent[traverse_vert], uv_g);
		assert(e.second);
		//uv_g[e.first] = g[ep.first];
		
		dist_uv = dist_uv + g[ep.first].weight;
		traverse_vert = rparent[traverse_vert];	
	}

	if(traverse_vert == zsource && traverse_vert != v)
	{
		traverse_vert = v;
		while(traverse_vert != zsource ) 
		{
			assert(traverse_vert != u);
			
			pair<edge_descriptorN, bool> ep = edge(traverse_vert, rparent[traverse_vert], g);
			assert(ep.second);
			pair<edge_descriptorN, bool> e = edge(traverse_vert, rparent[traverse_vert], uv_g);
			if(e.second) 
			{
				dist_uv = dist_uv - g[ep.first].weight;
			}
			else
			{	
				dist_uv = dist_uv + g[ep.first].weight;
			}
			traverse_vert = rparent[traverse_vert];	
		}
	}
	return dist_uv;
}

void createPDVectors(const GraphN &g, ParentVector &parent, DistanceVector &distance, int src)
{
	ASSERTING(parent.size() >= num_vertices(g));
	parent[src] = src;
	distance[src] = 0;
	createParentVector(src, src, g, parent, distance);	

	//dumpGraph(g);
	//cerr << WRITE_FUNCTION << " Src " << src << " Distance Vector: " << endl;
	// dump distance vector
	//int i = 0;
	//for(DistanceVector::iterator it = distance.begin(), it_end = distance.end(); it != it_end; ++it, i++)
	//{
	//	if(*it > 0) cerr << i << ": " << *it << " "  << endl;
	//}
	//char c = getchar();
}

void createParentVector(int prev_v, int start_v, const GraphN &tree_g, ParentVector &parent, DistanceVector &distance)
{
	//cerr << WRITE_FUNCTION << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		int targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		if(targ == prev_v) continue;
		
		distance[targ] = distance[start_v] + tree_g[*oit].weight;
		parent[targ] = start_v;
	
		ASSERTING(distance[targ] > 0);	

		createParentVector(start_v, targ, tree_g, parent, distance);
	}
}

void wcDistancesOTTC(const GraphN &g, const VertexSet &treeNodes, TreeDiameter &TD, int src)
{
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	out_edge_iteratorN oit, oit_end;
	VertexSet::iterator vit, vit_end, vit_in, vit_in_end;
		
	TD.parent = ParentVector(num_vertices(g));
	TD.distance = DistanceVector(num_vertices(g));
	ParentVector& near = TD.parent;
	DistanceVector& ecc = TD.distance;
	
	using namespace boost::numeric::ublas;
	matrix<double> dist(num_vertices(g), num_vertices(g));
	pair<int, double> nearest_vert(-1,(std::numeric_limits<double>::max)());
	
	for(vit = treeNodes.begin(), vit_end = treeNodes.end(); vit != vit_end; ++vit)
	{	
		ecc[*vit] = 0;		
		if(*vit != src) 
		{
			pair<edge_descriptorN, bool> ep = edge(*vit, src, g);
			if(ep.second) 
			{
				near[*vit] = src;
				if(nearest_vert.second > wmap[ep.first])
				{
					nearest_vert.first = *vit;
					nearest_vert.second = wmap[ep.first];
				}
			}
			else near[*vit] = -1;
		}
		else 
		{
			near[*vit] = 0;
		}
		for(vit_in = treeNodes.begin(), vit_in_end = treeNodes.end(); vit_in != vit_in_end; ++vit_in)
			dist(*vit,*vit_in) = 0;
	}

	// -- begin --
	TreeStructure T;
	T.insertVertex(src, g[src]);
	ecc[src] = 0;
	dist(src,src) = 0;
	VertexSet notInTree = treeNodes - src;
	
	while(1) 	
	{
		int z = nearest_vert.first;
		
		T.insertVertex(z, g[z]);
		T.insertEdge(z, near[z], g);
		notInTree = notInTree - z;
		if(notInTree.empty()) break;

		pair<edge_descriptorN, bool> edge_z_near = edge(z, near[z], g);
		ASSERTING(edge_z_near.second);
		
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			if(dist(near[z],*vit) > 0) dist(z,*vit) = dist(near[z],*vit) + wmap[edge_z_near.first];
		}
		
		dist(z,z) = 0;
		ecc[z] = ecc[near[z]] + wmap[edge_z_near.first];

		dist(near[z],z) = wmap[edge_z_near.first];
		if(ecc[near[z]] <= 0) ecc[near[z]] = wmap[edge_z_near.first];
			
		for(vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		{
			dist(*vit,z) = dist(*vit, near[z]) + wmap[edge_z_near.first];
			ecc[*vit] = std::max(ecc[*vit], dist(*vit,z));	
		}
		
		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			double curr_ecc = (std::numeric_limits<double>::max)(); 
			for(vit_in = T.V.begin(), vit_in_end = T.V.end(); vit_in != vit_in_end; ++vit_in)
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(!ep.second) continue;
					
				double new_ecc = ecc[*vit_in] + wmap[ep.first]; 
					
				if(new_ecc < curr_ecc)
				{ 
					near[*vit] = *vit_in;
					curr_ecc = new_ecc;
				}	
			}
		}
		
		nearest_vert = pair<int,double>(-1,(std::numeric_limits<double>::max)());

		for(vit = notInTree.begin(), vit_end = notInTree.end(); vit != vit_end; ++vit)
		{
			if(near[*vit] > -1) 
			{
				pair<edge_descriptorN, bool> ep = edge(*vit, near[*vit], g);
				ASSERTING(ep.second);
					
				double new_ecc = ecc[near[*vit]] + wmap[ep.first];
				if(nearest_vert.second > new_ecc) 
				{
					nearest_vert.first = *vit;
					nearest_vert.second = new_ecc; 
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------

				Check Tree validity and dump functions

------------------------------------------------------------------------- */
void checkTree(const TreeStructure &t)
{

#ifndef NDEBUG

	if(t.isValid() == false) 
	{
		cerr << WRITE_FUNCTION << " tree not valid : " << t << endl;
		return;
	}
		
	if(t.V.size() <= 1)
	{
		ASSERTING(t.E.size() == 0);
		ASSERTING(t.Ep.size() == 0);
		ASSERTING(t.E.size() == num_edges(t.g));
		if(t.V.size() == 1)
			ASSERTING(t.g[*(t.V.begin())].vertexState == GROUP_MEMBER);
		return;
	}
	
	GraphVisualization GV;
	
	if(t.E.size() != (t.V.size() - 1))
	{
		VertexSet edgeV, diffV;
		edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = edges(t.g); eit !=eit_end; ++eit)
		{
			edge_descriptorN e = *eit;
			int src = source(e, t.g), targ = target(e, t.g);
			edgeV.insert(src);
			edgeV.insert(targ);
			if(!t.V.contains(src)) diffV.insert(src);
			if(!t.V.contains(targ)) diffV.insert(targ);

			bool foundP = false;
			EdgePair::const_iterator epit = t.Ep.begin(), epit_end = t.Ep.end();
			for( ; epit != epit_end; ++epit)
			{
				if(*epit == pair<int, int>(src,targ)) 
				{
					foundP = true;
					break;
				}
			}
			
			bool foundL = false;
			EdgeList::const_iterator elit = t.E.begin(), elit_end = t.E.end();
			for( ; elit != elit_end; ++elit)
			{
				int u = source(*elit, t.g), v = target(*elit, t.g);
				if(pair<int, int>(u,v) == pair<int, int>(src,targ)) 
				{
					foundL = true;
					break;
				}
			}
			if(foundL == false)
				cerr << "Couldn't find List " << e << endl;
			if(foundP == false)
				cerr << "Couldn't find Pair " << e << endl;
		}

		cerr << "Edge vertices " << edgeV << endl;
		cerr << "Tree vertices " << t.V << endl;
		cerr << "Diff " << edgeV - t.V << endl;
		cerr << "Diff " << t.V - edgeV << endl;			
		cerr << "Diff " << diffV << endl;
		cerr << "Sizes g.edges " << num_edges(t.g) << " t.E.size() " << t.E.size() << " t.V.size() " << t.V.size() << endl;
	
		GV.VizTree("error_tree_viz.net", t);
	}
	
	edge_iteratorN edit, edit_end;
    for(boost::tuples::tie(edit, edit_end) = edges(t.g); edit != edit_end; ++edit)
	{
		edge_descriptorN e = *edit;
		int src = source(e, t.g), targ = target(e, t.g);
		ASSERTING(t.V.contains(src));
		ASSERTING(t.V.contains(targ));

		ASSERTING(src == t.g[e].id.first || src == t.g[e].id.second);
		ASSERTING(targ == t.g[e].id.first || targ == t.g[e].id.second);
	}
	
	if(t.S.size())	ASSERTING((t.V - t.S) != t.V);
	ASSERTING(t.V.size() > 1);
	ASSERTING(t.E.size() > 0);
	ASSERTING(t.E.size() == t.Ep.size());
	ASSERTING(t.E.size() == num_edges(t.g));
	ASSERTING(t.E.size() == (t.V.size() - 1));

	GraphAlgorithms::checkGraph(t.g);
	
	VertexSet::iterator vit = t.V.begin(), vit_end = t.V.begin();
	for( ; vit != vit_end; ++vit)
	{
		if(	(t.g[*vit].vertexState == NO_STATE) ||
			(t.g[*vit].vertexState == VERTEX_STATE_ERROR) ||
			(t.g[*vit].vertexFunction == NO_FUNCTION) ||
			(t.g[*vit].vertexFunction == VERTEX_FUNCTION_ERROR))
				GV.VizTree("error_tree_viz.net", t);
		
		if(t.g[*vit].vertexState == STEINER_POINT || t.g[*vit].vertexState == STEINER_POINT_AVAILABLE)
		{
			if(getOutDegree(*vit, t.g) <= 1) cerr << "Steiner point " << *vit << " out-degree " << getOutDegree(*vit, t.g) << " Tree : " << t << endl;
			ASSERTING(getOutDegree(*vit, t.g) > 1);
		}
		else ASSERTING(getOutDegree(*vit, t.g) > 0); // if(t.g[*vit].vertexState == GROUP_MEMBER)
		//ASSERTING(getOutDegree(*vit, t.g) <= getDegreeConstraint(t.g, *vit));
		
		ASSERTING(t.g[*vit].id > -1);
		ASSERTING(t.g[*vit].vertexState != NO_STATE);
		ASSERTING(t.g[*vit].vertexState != VERTEX_STATE_ERROR);
		ASSERTING(t.g[*vit].vertexFunction != NO_FUNCTION);
		ASSERTING(t.g[*vit].vertexFunction != VERTEX_FUNCTION_ERROR);
	}

	for(vit = t.S.begin(), vit_end = t.S.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, t.g) <= 1) 
		{
			t.printVertexState(cerr);
			prettyPrint(t);
			cerr << "Steiner point " << *vit << " out-degree " << getOutDegree(*vit, t.g) << " Tree : " << t << endl;
		}
		if(!t.V.contains(*vit)) cerr << " steiner points " << t.S << " vertices " << t.V << endl;
		
		ASSERTING(t.V.contains(*vit));
		ASSERTING(t.g[*vit].vertexState == STEINER_POINT || t.g[*vit].vertexState == STEINER_POINT_AVAILABLE);

		ASSERTING(getOutDegree(*vit, t.g) > 1);
		//ASSERTING(getOutDegree(*vit, t.g) <= getDegreeConstraint(t.g, *vit));
		ASSERTING(t.g[*vit].id > -1);
	}
	
	EdgePair::const_iterator eit = t.Ep.begin(), eit_end = t.Ep.end();
	EdgeList::const_iterator elit = t.E.begin(), elit_end = t.E.end();
	for( ; eit != eit_end && elit != elit_end; ++elit, ++eit)
	{
		int src = eit->first;
		int targ = eit->second;
			
		std::pair<edge_descriptorN, bool> ep = edge(src, targ, t.g);
		ASSERTING(ep.second);
				
		src = source(*elit, t.g);
	   	targ = target(*elit, t.g);
		std::pair<edge_descriptorN, bool> el = edge(src, targ, t.g);
		ASSERTING(el.second);
		
		ASSERTING(getOutDegree(src, t.g) > 0);
		ASSERTING(getOutDegree(targ, t.g) > 0);
		
		//cerr << "EdgeList : " << *elit << " " << e.first << " EdgePair " << *eit << " " << el.first << endl;
		
		if(t.g[el.first].id.first == t.g[el.first].id.second)
			GV.VizTree("error_tree_viz.net", t);

		if( t.g[el.first].id.first == t.g[el.first].id.second ||
			t.g[el.first].cost <= 0 ||
			t.g[el.first].weight <= 0 ||
			t.g[el.first].delay <= 0  ||
			!(t.g[el.first].id == *eit) ||
			el.first != ep.first)
		{
			cerr << *eit << " == " << ep.first << " == " << t.g[ep.first].id << " == " << EdgeId(src,targ) << " == " << t.g[el.first].id << " ? " << endl;
			cerr << " Prop: Ep " << t.g[ep.first] << " E: " << t.g[el.first] ;
			print_graph(t.g, get(vertex_index, t.g));
			dumpGraph(t.g);
		}

		ASSERTING(el.first == ep.first);
		ASSERTING(t.g[el.first].id == *eit);
		ASSERTING(t.g[el.first].id.first != t.g[el.first].id.second);
		ASSERTING(t.g[el.first].cost > 0);
		ASSERTING(t.g[el.first].weight > 0);
		ASSERTING(t.g[el.first].delay > 0);
		ASSERTING(t.g[el.first].weight <= GlobalSimArgs::maxEdgeDelay_);
	}
/*	
	EdgeList::iterator eit = t.E.begin(), eit_end = t.E.end();
	for( ; eit != eit_end; ++eit)
	{
		
		int src = source(*eit, t.g), targ = target(*eit, t.g);
		std::pair<edge_descriptorN, bool> e = edge(src, targ, t.g);
		ASSERTING(e.second);
				
		if(t.g[e.first].id.first == t.g[e.first].id.second)
			GV.VizTree("error_tree_viz.net", t);

		if( t.g[e.first].id.first == t.g[e.first].id.second ||
			t.g[e.first].cost <= 0 ||
			t.g[e.first].weight <= 0 ||
			t.g[e.first].delay <= 0 ||
			e.first != *eit)
		{
			cerr << *eit << "Cost: " << t.g[*eit].cost << " Weight : " << t.g[*eit].weight << endl;	
			cerr << e.first << "Cost: " << t.g[e.first].cost << " Weight : " << t.g[e.first].weight << endl;
			print_graph(t.g, get(vertex_index, t.g));
			dumpGraph(t.g);
		}

		ASSERTING(e.first == *eit);
		ASSERTING(t.g[e.first].id.first != t.g[e.first].id.second);
		ASSERTING(t.g[e.first].cost > 0);
		ASSERTING(t.g[e.first].weight > 0);
		ASSERTING(t.g[e.first].delay > 0);
	}
*/


	/*
	SimVertexMap::iterator svit = t.simV.begin(), svit_end = t.simV.end();
	for( ; svit != svit_end; ++svit)
	{
		if(	(svit->second.getVertexState() == NO_STATE) ||
			(svit->second.getVertexState() == VERTEX_STATE_ERROR) ||
			(svit->second.getVertexFunction() == NO_FUNCTION) ||
			(svit->second.getVertexFunction() == VERTEX_FUNCTION_ERROR))
				GV.VizTree("error_tree_viz.net", t);
				
		ASSERTING(svit->second.getVertexState() != NO_STATE);
		ASSERTING(svit->second.getVertexState() != VERTEX_STATE_ERROR);
		ASSERTING(svit->second.getVertexFunction() != NO_FUNCTION);
		ASSERTING(svit->second.getVertexFunction() != VERTEX_FUNCTION_ERROR);		
	}
	
	SimEdgeList::iterator seit = t.simE.begin(), seit_end = t.simE.end();
	for( ; seit != seit_end; ++seit)
	{
		if(seit->getWeight() < 0 || seit->getSource() == seit->getDestination())
			GV.VizTree("error_tree_viz.net", t);
		
		ASSERTING(seit->getWeight() > 0);
		ASSERTING(seit->getWeight() <= GlobalSimArgs::maxEdgeDelay_);
		ASSERTING(seit->getSource() != seit->getDestination());
	}
	*/
	//checkHasLoop(t);
#endif // NDEBUG
}

void checkDijkstra(const ParentVector &p, const DistanceVector &distances)
{
#ifndef NDEBUG	
	bool found_parent = false;
	//std::cerr << "[SH::dumpDijkstra] Parent/Distance Map : "<< endl;
	for (std::size_t i = 0; i != p.size(); ++i)
    {
    	if (p[i] != i)
    	{
    		found_parent = true;
    		if(distances[i] <= 0 || distances[i] > MAXIMUM_WEIGHT)
    			dumpDijkstra(p, distances);
    		ASSERTING(distances[i] > 0 || distances[i] < MAXIMUM_WEIGHT);
    	}
	}
	if(found_parent == false)
	{
		dumpDijkstra(p, distances);
	}
	ASSERTING(found_parent);
#endif // NDEBUG
}


/*-----------------------------------------------------------------------
			finds out whether the tree has a loop
				NB! does not work!
------------------------------------------------------------------------- */
void checkHasLoop(const TreeStructure &T)
{
#ifndef NDEBUG	
	GraphN g;
	createGraphFromTree(g, T);
	VertexSet visited;
		
	// start walk 
	vertex_iteratorN vit, vit_end;
    for(boost::tuples::tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, g) > 0)
		{
			walkTree(g, *vit, *vit, *vit, visited);
			break;
		}
	}
	if(visited.contains(*vit))
	{
		boost::print_graph(g, get(vertex_index, g));
		dumpGraph(g);
	}
	
	ASSERTING(!visited.contains(*vit));
#endif // NDEBUG
}

void walkTree(const GraphN &g, int start, int u, int v, VertexSet &visited)
{
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
	{
		if(visited.contains(start))	break;

		int w = target(*oit, g);
		ASSERTING(v != w);
		
		if( u != w && start == w)
		{
			visited.insert(start);
		}
		else if(!(visited.contains(w)))
		{
			visited.insert(w);
			walkTree(g, start, v, w, visited);	
		}
	}		
}

/*-----------------------------------------------------------------------
						dump functions
------------------------------------------------------------------------- */
void prettyPrint(const TreeStructure &T)
{
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		cerr << *vit << ": " ;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, T.g); oit != oit_end; ++oit)
			cerr << target(*oit, T.g) << ", " ;
		cerr << endl;
	}

	cerr << " leaves : " << endl;
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, T.g) == 1)
		{
			cerr << *vit << ": " ;
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, T.g); oit != oit_end; ++oit)
				cerr << target(*oit, T.g) << ", " ;
			cerr << endl;
		}
	}
	cerr << " centers (od > 1): " << endl;
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, T.g) > 1)
		{
			cerr << *vit << ": " ;
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, T.g); oit != oit_end; ++oit)
				cerr << target(*oit, T.g) << ", " ;
			cerr << endl;
		}
	}
}

void dumpTreeStructure(const TreeStructure &T)
{
    iVertexMapConst idmap 	= get(&VertexProp::id, T.g);
	IndexMap   index	= get(vertex_index, T.g);
	
	cerr << "#V = " << T.V.size() << " #E = " << T.E.size() << " #Ep = " << T.Ep.size() << endl;
	
	VertexSet_it vit;
	cerr << "V (index):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << *vit ;
		
	cerr << " E (index):";
	EdgeList_cit eit;
	for(eit = T.E.begin(); eit != T.E.end(); ++eit)
	{
		cerr << " " << *eit ;
	}
	cerr << endl;

	cerr << "V (id):";
	for(vit = T.V.begin(); vit != T.V.end(); ++vit)
		cerr << " " << idmap[*vit] ;
	
	cerr << " E (id): ";
	EdgePair::const_iterator epit;
	for(epit = T.Ep.begin(); epit != T.Ep.end(); ++epit)
		cerr << " " << *epit ;
	cerr << endl;
}

void dumpDijkstra(const ParentVector &p, const DistanceVector &distances)
{
	std::cerr << "[dumpDijkstra] Parent/Distance Map : "<< endl;
	for (std::size_t i = 0; i != p.size(); ++i)
    {
    	if(p[i] != i && distances[i] > 0 && distances[i] < MAXIMUM_WEIGHT)
    	  std::cerr << "parent[" << i << "] = " << p[i] << " Dist: " << distances[i] << std::endl;
    	else if( p[i] == i)
    	  std::cerr << "parent[" << i << "] = no parent" << " Dist: " << distances[i] <<  std::endl;
    }
}

void dumpGraph(const GraphN &g)
{
    iVertexMapConst 	idmap 	= get(&VertexProp::id, g);
	vsVertexMapConst 	vsmap 	= get(&VertexProp::vertexState, g);
	vfVertexMapConst 	vfmap 	= get(&VertexProp::vertexFunction, g);
    dEdgeMapConst 		wm 		= get(&EdgeProp::weight, g);
	eidEdgeMapConst 	eidmap 	= get(&EdgeProp::id, g);

    cerr << "vertices(index) = ";
    pair<vertex_iteratorN, vertex_iteratorN> vp;
    for(vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		if(idmap[*vp.first] > -1)
			cerr << *vp.first << ", "; 
    }
	cerr << endl;
	
    cerr << "edges(index) = ";
	graph_traits<GraphN>::edge_iterator ei, ei_end;
    for(boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
    	cerr << "(" << *ei << " w: " << wm[*ei] << ") "; 
    }
    cerr << endl;
 
    cerr << "vertices(id) = ";
    for(vp = vertices(g); vp.first != vp.second; ++vp.first)		
    {
		if(idmap[*vp.first] > -1)
			cerr << "(" << idmap[*vp.first] << " " << vsmap[*vp.first] << " " << vfmap[*vp.first] << ") ";
    }
    cerr << endl;
    	
    cerr << "edges(id) = ";
    for(boost::tuples::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
		std::cerr << "(" << eidmap[*ei] << " w: " << wm[*ei] << ") "; 
	}
    cerr << endl;
}

/*
double getPairWiseCost(const TreeStructure &T)
{
	TreeDiameter TD;
	int src = *(T.V.begin());
	wcVertexDistances(T, TD, src);

	double maxPairWise = 0;
	VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end();
	for( ; vit != vit_end; ++vit)
	{
		if(maxPairWise < TD.distance[*vit])
			maxPairWise = TD.distance[*vit];
	}
	return maxPairWise;
}

int getCenterNodePW(const VertexSet &V, const GraphN &g)
{
	TreeDiameter TD;
	int src = *(V.begin());
	wcVertexDistances(g, TD, src);

	double maxPairWise = 0;
	int centerVertexPW = -1;
	VertexSet::iterator vit = V.begin(), vit_end = V.end();
	for( ; vit != vit_end; ++vit)
	{
		if(maxPairWise < TD.distance[*vit])
		{
			maxPairWise = TD.distance[*vit];
			centerVertexPW = *vit;
		}
	}
	return centerVertexPW;
}
*/


/*-----------------------------------------------------------------------
 	createBoostGraphIndex: converts g_in to a graph where index == id
			NB! Currently it is only used by ADH
------------------------------------------------------------------------- */
/*void createBoostGraphIndex(GraphN& g_out, const GraphN &g_in, TreeStructure &T) 
{
	cerr << WRITE_FUNCTION << endl;
	dumpTreeStructure(T);
	
	EdgeList& simEdges = T.E;
	VertexSet simVertices = T.V;
	EdgeList dupEdges;
	
	ASSERTING(simEdges.size() && simVertices.size()); // ASSERTING size > 0
	ASSERTING(simEdges.size() == (simVertices.size() - 1));

	GraphAlgorithms::checkGraph(g_in);
	//checkTree(T);

	EdgeList::iterator eit, eit_end;
	for(eit = simEdges.begin(), eit_end = simEdges.end(); eit != eit_end; ++eit)
	{
		int src = source(*eit, g_in), targ = target(*eit, g_in);
		pair<edge_descriptorN, bool> e_in = edge(src, targ, g_in);
	
		ASSERTING(e_in.second);
		ASSERTING(simVertices.contains(src));
		if(!simVertices.contains(targ))
		{
			cerr << endl << simVertices << endl;
		}
		ASSERTING(simVertices.contains(targ));		

		//cerr << *eit << " (" << src << "," << targ << ") " << e_in.first << endl;
				
		pair<edge_descriptorN, bool> e_out = edge(src, targ, g_out);
		if(e_out.second) // duplicate in simEdges
		{
			cerr << WRITE_FUNCTION << " Found Duplicate " << e_out.first << endl;
			dupEdges.push_back(e_in.first);
			//char c = getchar();
		}
		else
		{		
			e_out = add_edge(src, targ, g_out);
			ASSERTING(e_out.second);
			g_out[e_out.first] = EdgeProp(g_in[e_in.first], src, targ);
			
			ASSERTING(g_out[e_out.first].weight > 0);
			ASSERTING(g_out[e_out.first].cost > 0);
		}
	}

	VertexSet::iterator vit, vit_end;
	for(vit = simVertices.begin(), vit_end = simVertices.end(); vit != vit_end; ++vit)
	{
		g_out[*vit] = g_in[*vit]; 
		if(g_out[*vit].vertexState != GROUP_MEMBER) g_out[*vit].vertexState = STEINER_POINT;
		
		ASSERTING(g_out[*vit].vertexFunction != NO_FUNCTION);
		ASSERTING(g_in[*vit].vertexFunction != NO_FUNCTION);
	}
	
	iVertexMapConst idmap = get(&VertexProp::id, g_in);
	
	EdgeList::iterator deit = dupEdges.begin(), deit_end = dupEdges.end();
	for( ; deit != deit_end; ++deit)
	{
		for(eit = simEdges.begin(), eit_end = simEdges.end(); eit != eit_end; ++eit)
		{ 
			//cerr << *eit << *deit << endl;
			if(*eit == *deit) 
			{
				cerr << WRITE_FUNCTION << " EdgeList Erasing " << *eit << endl;
				simEdges.erase(eit);
				break;
			}
		}
		
		int src = source(*deit, g_in), targ = target(*deit, g_in);
		
		EdgePair::iterator peit = T.Ep.begin(), peit_end = T.Ep.end();
		for( ; peit != peit_end; ++peit)
		{
			if((peit->first == idmap[src] && peit->second == idmap[targ]) || (peit->second == idmap[src] && peit->first == idmap[targ]))
			{
				cerr << WRITE_FUNCTION << " EdgdPair Erasing " << *eit << endl;
				T.Ep.erase(peit);
				break;
			}
		}		
	}
	GraphAlgorithms::checkGraph(g_out);
}
*/
/*
void createBoostGraphId(GraphN& g_out, const GraphN &g_in, TreeStructure &T) 
{
	EdgePair& simEdges = T.Ep;
	VertexSet simVertices = T.V;
	EdgeList dupEdges;
	
	eidEdgeMapConst  eidmap = get(&EdgeProp::id, g_in);
	ASSERTING(simEdges.size() && simVertices.size()); // ASSERTING size > 0

	EdgeList::iterator elit = T.E.begin(), elit_end = T.E.end();
	EdgePair::iterator eit, eit_end;
	for(eit = simEdges.begin(), eit_end = simEdges.end(); elit != elit_end && eit != eit_end; ++eit, ++elit)
	{
		int src = source(*elit, g_in);
		int targ = target(*elit, g_in);
		pair<edge_descriptorN, bool> el_in = edge(src, targ, g_in);
		ASSERTING(el_in.second);
		pair<edge_descriptorN, bool> el2_in = edge(targ, src, g_in);
		ASSERTING(el2_in.second);
		pair<edge_descriptorN, bool> e_in = edge(eit->first, eit->second, g_in);
		ASSERTING(e_in.second);
		int src_in = source(e_in.first, g_in), targ_in = target(e_in.first, g_in);

		if(el_in.first != e_in.first || el_in.first != el2_in.first)
		{
			cerr << "El " << el_in.first << " src " << src << " targ " << targ << " Ep " << e_in.first << " src_in " << src_in << " targ_in " << targ_in << endl;
			cerr << "El prop id " << g_in[el_in.first].id << " El2 prop id " << g_in[el2_in.first].id << " Ep prop id " << g_in[e_in.first].id << endl;
			cerr << "El prop id " << eidmap[el_in.first] << " Ep prop id " << eidmap[e_in.first] << endl;
			cerr << "El prop " << g_in[el_in.first] << " El2 prop " << g_in[el2_in.first] << " Ep prop " << g_in[e_in.first] << endl;
			//dumpTreeStructure(T);
		}
	
		ASSERTING(el_in.first == e_in.first && el_in.first == el2_in.first);
		
		if(!simVertices.contains(eit->first) || !simVertices.contains(eit->second))
		{
			cerr << endl << simVertices << endl;
			cerr << *eit << endl;
		}
		ASSERTING(simVertices.contains(eit->first) && simVertices.contains(eit->second));
		ASSERTING(simVertices.contains(src_in) && simVertices.contains(targ_in));
		
		pair<edge_descriptorN, bool> e_out = edge(eit->first, eit->second, g_out);
		if(e_out.second) // duplicate edges
		{
			//cerr << WRITE_FUNCTION << "Found Duplicate " << e_out.first << endl;
			dupEdges.push_back(e_in.first);
		}
		else // add edge
		{
			e_out = add_edge(eit->first, eit->second, g_out);
			ASSERTING(e_out.second);
			int src_out = source(e_out.first, g_out), targ_out = target(e_out.first, g_out);
			ASSERTING(simVertices.contains(src_out) && simVertices.contains(targ_out));
			ASSERTING(src_out == src_in && targ_out == targ_in);
			ASSERTING((src_out == eit->first && targ_out == eit->second) || (src_out == eit->second && targ_out == eit->first));
			
			g_out[e_out.first] = g_in[e_in.first];
			g_out[src_out] = g_in[src_in];
			g_out[targ_out] = g_in[targ_in];

			if(g_out[src_out].vertexState != GROUP_MEMBER)	g_out[src_out].vertexState = STEINER_POINT;
			if(g_out[targ_out].vertexState != GROUP_MEMBER)	g_out[targ_out].vertexState = STEINER_POINT;

			ASSERTING(g_out[src_out].vertexFunction != NO_FUNCTION);
			ASSERTING(g_in[src_in].vertexFunction != NO_FUNCTION);
			ASSERTING(g_out[targ_out].vertexFunction != NO_FUNCTION);
			ASSERTING(g_in[targ_in].vertexFunction != NO_FUNCTION);
			ASSERTING(g_out[e_out.first].weight == g_in[e_in.first].weight);
		}
	}
	
	iVertexMapConst idmap = get(&VertexProp::id, g_in);
		
	EdgeList::iterator deit = dupEdges.begin(), deit_end = dupEdges.end();
	for( ; deit != deit_end; ++deit)
	{
		int src = source(*deit, g_in), targ = target(*deit, g_in);
		
		for(eit = simEdges.begin(), eit_end = simEdges.end(); eit != eit_end; ++eit)
		{
			if((eit->first == idmap[src] && eit->second == idmap[targ]) || (eit->second == idmap[src] && eit->first == idmap[targ]))
			{
				//cerr << WRITE_FUNCTION << " EdgePair Erasing " << *eit << endl;
				simEdges.erase(eit);
				break;
			}
		}

		EdgeList::iterator peit = T.E.begin(), peit_end = T.E.end();
		for( ; peit != peit_end; ++peit)
		{
			//cerr << *peit << *deit << endl;
			if(*peit == *deit) 
			{
				//cerr << WRITE_FUNCTION << " EdgeList Erasing " << *peit << endl;
				T.E.erase(peit);
				break;
			}
		}		
	}
	GraphAlgorithms::checkGraph(g_out);
}
*/

/*
void createSimGraph(GraphN g, SimGraph& simGraph)
{
	vertex_iteratorN vit, vit_end;
	for(tie(vit, vit_end) = vertices(g); vit != vit_end; ++vit)
	{
		VertexProp vp = g[*vit];
		simGraph.insertVertex(vp);
	}
	
	edge_iteratorN eit, eit_end;
	for(tie(eit, eit_end) = edges(g); eit != eit_end; ++eit)
	{
		EdgeProp ep = g[*eit];
		int src = source(*eit, g), targ = target(*eit, g);
		simGraph.insertEdge(src, targ, ep);
	}
}

void createBoostGraph(GraphN& g, SimGraph simGraph)
{
	ASSERTING(simGraph.Edges().size() && simGraph.Vertices().size()); // ASSERTING size > 0
	
	SimEdgeList::iterator eit, eit_end;
	for(eit = simGraph.Edges().begin(), eit_end = simGraph.Edges().end(); eit != eit_end; ++eit)
	{
		edge_descriptorN e = add_edge(eit->getSource(), eit->getDestination(), g).first;
		//EdgeProp ep = g[e];
		g[e] = eit->getEdgeProp();
	}

	SimVertexMap::iterator vit, vit_end;
	for(vit = simGraph.Vertices().begin(), vit_end = simGraph.Vertices().end(); vit != vit_end; ++vit)
	{
		//VertexProp vp = g[vit->second.getVertexId()];
		g[vit->second.getVertexId()] = vit->second.getVertexProp();
	}
}

void createBoostGraph(GraphN& g, SimEdgeList simEdges, SimVertexMap simVertices)
{
	ASSERTING(simEdges.size() && simVertices.size()); // ASSERTING size > 0
	
	SimEdgeList::iterator eit, eit_end;
	for(eit = simEdges.begin(), eit_end = simEdges.end(); eit != eit_end; ++eit)
	{
		edge_descriptorN e = add_edge(eit->getSource(), eit->getDestination(), g).first;
		g[e] = eit->getEdgeProp();
	}

	SimVertexMap::iterator vit, vit_end;
	for(vit = simVertices.begin(), vit_end = simVertices.end(); vit != vit_end; ++vit)
	{
		g[vit->second.getVertexId()] = vit->second.getVertexProp();
	}
}

void createBoostGraphIndex(GraphN& g_out, TreeStructure T)
{
	GraphN g_in = T.g;
	EdgeList simEdges = T.E;
	VertexSet simVertices = T.V;

	ASSERTING(simEdges.size() && simVertices.size()); // ASSERTING size > 0
	
	EdgeList::iterator eit, eit_end;
	for(eit = simEdges.begin(), eit_end = simEdges.end(); eit != eit_end; ++eit)
	{
		int src = source(*eit, g_in), targ = target(*eit, g_in);
		edge_descriptorN e = add_edge(src, targ, g_out).first;
		g_out[e] = g_in[*eit];
	}

	VertexSet::iterator vit, vit_end;
	for(vit = simVertices.begin(), vit_end = simVertices.end(); vit != vit_end; ++vit)
	{
		g_out[*vit] = g_in[*vit]; 
	}
}

void createSimTree(TreeStructure treeStructure, SimGraph& simTree)
{
	ASSERTING(treeStructure.V.size() != 0 || treeStructure.E.size() != 0);
		
	VertexSet::iterator vit, vit_end;
	for(vit = treeStructure.V.begin(), vit_end = treeStructure.V.end(); vit != vit_end; ++vit)
	{
		VertexProp vp = treeStructure.g[*vit];
		simTree.insertVertex(vp);
	}
	
	EdgeList::iterator eit, eit_end;
	for(eit = treeStructure.E.begin(), eit_end = treeStructure.E.end(); eit != eit_end; ++eit)
	{
		EdgeProp ep = treeStructure.g[*eit];
		int src = source(*eit, treeStructure.g), targ = target(*eit, treeStructure.g);
		simTree.insertEdge(src, targ, ep);
	}
}
*/

}; // namespace TreeAlgorithms ends


