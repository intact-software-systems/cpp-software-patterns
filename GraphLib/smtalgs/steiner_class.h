/***************************************************************************
                          steiner_class.h  -  description
                             -------------------
    begin                : Wed Sep 14 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_STEINER_CLASS_KHV
#define GRAPHALGO_STEINER_CLASS_KHV

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <queue>
#include <map>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <functional>
#include <time.h>
//#include <boost/limits.hpp>
//#include <boost/graph/named_function_params.hpp>
//#include <boost/graph/breadth_first_search.hpp>
//#include <boost/pending/relaxed_heap.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "../boostprop.h"
#include "../treealgs/treestructure.h"
#include "../network/vertex_set.h"
//#include "../../graphstats/recordtree.h"

using namespace TreeAlgorithms;
using namespace std;

/* -----------------------------------------------------
				helper structs
----------------------------------------------------- */

struct MyWrapper {

	MyWrapper() {}
	MyWrapper(int id_in, int prio_in, vertex_descriptorN v_in) : id(id_in), priority(prio_in), v(v_in) {}
	~MyWrapper() {}
		
	int id;
	int priority;		
	vertex_descriptorN v;
};

struct ShortestPathKeeper {

	ShortestPathKeeper() {}	
	ShortestPathKeeper(const DistanceVector &d, const ParentVector &p, int id) : zdistance(d), zparent(p), zid(id) {}
	ShortestPathKeeper(const DistanceVector &d, const ParentVector &p, const vector<double> &c, int id) : zdistance(d), zparent(p), zcost(c), zid(id) {}
	~ShortestPathKeeper() {}
		
	DistanceVector zdistance;
	ParentVector zparent;
	std::vector<double> zcost;
	int zid;
};

/* -----------------------------------------------------

	SteinerHeuristic()
		Base class for SMT algorithms 

----------------------------------------------------- */
class SteinerHeuristic
{
protected:
	int 					num_zvertices;
	const TreeStructure		&inputT;
	const GraphN 			&g;
	
	VertexSet							ZVertSet;	// a set with every z-vertex
	vector<MyWrapper>					ZVert;		// a vector with every z-vertex
	map<int, ShortestPathKeeper>		SPKeeper;	// stores Shortest Paths/MSTs from every z-node in the graph

public:
	SteinerHeuristic(const TreeStructure &in) : num_zvertices(0), inputT(in), g(in.g), ZVert(), SPKeeper(), overhead_exec_time_(0) {}
	~SteinerHeuristic() 
	{
		//StatsContainer::overhead_time = overheadExecTime();
		//cerr << WRITE_PRETTY_FUNCTION << " -OT " << StatsContainer::overhead_time  << endl;
	}
	
	// typedfes
	typedef list<vertex_descriptorN> 				DistanceList;
	typedef pair<DistanceList, vertex_descriptorN>  PathVector;
	
	
	// general Steiner Heuristic functions
	//virtual TreeStructure Algorithm(const GraphN &, vertex_descriptorN){ assert(0); return TreeStructure();}
	virtual void Initialize(vertex_descriptorN zsource);	
	virtual void CallPrimMST(const GraphN &g, vertex_descriptorN znode, ParentVector &zparent, DistanceVector &zdistance);
	virtual void RunPrimForEveryZ();
	virtual void RunDijkstraForEveryZ();
	virtual void RunDijkstraForEveryZ(const GraphN &g);
	virtual void RunDijkstraForEveryNode();
	virtual void findSPMaps(vertex_descriptorN src, ShortestPathKeeper &spk);
	virtual ShortestPathKeeper& findSPMaps(vertex_descriptorN src);
	
	// dump functions
	void dumpDijkstra(const ParentVector &p, const DistanceVector &d);
	void dumpGraph(const GraphN &g);
	void dumpSteinerTree(const TreeStructure &T);
	void dumpSteinerTree_full(const TreeStructure &T);
	
	// Test Functions -- unused -- 
	//virtual void PriorityQueueUnsigned(vertex_descriptorN zsource);	
	//virtual void RelaxedHeap(vertex_descriptorN zsource);

	inline clock_t overheadExecTime() { return overhead_exec_time_; }
	
protected:
	clock_t overhead_exec_time_;
};

#endif // GRAPHALGO_STEINER_CLASS_KHV


