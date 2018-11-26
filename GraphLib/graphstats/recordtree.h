/***************************************************************************
                          recordtree.h  -  description
                             -------------------
    begin                : Mon Sep 11 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_RECORDTREE_KHV
#define GRAPHALGO_RECORDTREE_KHV

#include <sstream>
#include "../boostprop.h"
#include "../treealgs/treealgs.h"
#include "../simdefs.h"

using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

class StatsContainer {
public:
	StatsContainer(const TreeStructure &Tcu, const TreeStructure &Tpr, TreeAlgo tree, DynamicTreeAlgo dynamic, ReconfigAlgo reconfig, int g, clock_t exec) 
			: Tc(Tcu)
			, Tp(Tpr)
			, treeAlgo(tree)
			, meshAlgo(NO_GRAPH_ALGO)
			, dynamicTreeAlgo(dynamic)
			, dynamicMeshAlgo(NO_DYNAMIC_MESH_ALGO)
			, reconfigAlgo(reconfig)
			, gId(g)
			, exec_time(exec)	
			, cpu_cycles(0)
			{}
    StatsContainer(const TreeStructure &Tcu, const TreeStructure &Tpr, TreeAlgo tree, DynamicTreeAlgo dynamic, ReconfigAlgo reconfig, int g, clock_t exec, uint64_t cpu)
			: Tc(Tcu)
			, Tp(Tpr)
			, treeAlgo(tree)
			, meshAlgo(NO_GRAPH_ALGO)
			, dynamicTreeAlgo(dynamic)
			, dynamicMeshAlgo(NO_DYNAMIC_MESH_ALGO)
			, reconfigAlgo(reconfig)
			, gId(g)
			, exec_time(exec)
			, cpu_cycles(cpu)
			{}
	
    StatsContainer(const TreeStructure &Tcu, const TreeStructure &Tpr, GraphAlgo mesh, DynamicMeshAlgo dynamic, ReconfigAlgo reconfig, int g, clock_t exec, uint64_t cpu)
			: Tc(Tcu)
			, Tp(Tpr)
			, treeAlgo(NO_TREE_ALGO)
			, meshAlgo(mesh)
			, dynamicTreeAlgo(NO_DYNAMIC_TREE_ALGO)
			, dynamicMeshAlgo(dynamic)
			, reconfigAlgo(reconfig)
			, gId(g)
			, exec_time(exec)
			, cpu_cycles(cpu)
			{}

    /*StatsContainer(const TreeStructure &Tcu, const TreeStructure &Tpr, TreeAlgo tree, DynamicTreeAlgo dynamic, ReconfigAlgo reconfig, int g, clock_t exec, uint64_t cpu, clock_t overhead)
			: Tc(Tcu)
			, Tp(Tpr)
			, treeAlgo(tree)
			, meshAlgo(NO_GRAPH_ALGO)
			, dynamicTreeAlgo(dynamic)
			, dynamicMeshAlgo(NO_DYNAMIC_MESH_ALGO)
			, reconfigAlgo(reconfig)
			, gId(g)
			, exec_time(exec)
			, cpu_cycles(cpu)
			{}
    StatsContainer(const TreeStructure &Tcu, const TreeStructure &Tpr, GraphAlgo mesh, DynamicTreeAlgo dynamic, ReconfigAlgo reconfig, int g, clock_t exec, uint64_t cpu, clock_t overhead)
			: Tc(Tcu)
			, Tp(Tpr)
			, treeAlgo(NO_TREE_ALGO)
			, meshAlgo(mesh)
			, dynamicTreeAlgo(dynamic)
			, dynamicMeshAlgo(NO_DYNAMIC_MESH_ALGO)
			, reconfigAlgo(reconfig)
			, gId(g)
			, exec_time(exec)
			, cpu_cycles(cpu)
            {}*/


	~StatsContainer() {}
	
	const TreeStructure &Tc;
	const TreeStructure &Tp;

	TreeAlgo 		treeAlgo;
	GraphAlgo		meshAlgo;

	DynamicTreeAlgo dynamicTreeAlgo;
	DynamicMeshAlgo dynamicMeshAlgo;
	ReconfigAlgo	reconfigAlgo;

	int gId;
	clock_t exec_time;
    uint64_t cpu_cycles;

	static clock_t overhead_time;
};


namespace Statistics
{
#define CLOCKS_PR_MS (CLOCKS_PER_SEC/1000)

bool doFlush();
void recordTree(std::ostream& ostr, const StatsContainer &SC);
EdgePair findDifference(const EdgePair &a, const EdgePair &b);
double findMedian(const std::multiset<double> &dset);
double findMedian(const std::vector<double> &dset);
double findMedian(const std::vector<double> &arr, const int &size);
double findMedian(double arr[], const int &size);

}

#endif // GRAPHALGO_RECORDTREE_KHV

