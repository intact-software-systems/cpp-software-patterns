/***************************************************************************
                          mesh_construction.h  -  description
                             -------------------
    begin                : Thu Mar 09 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_MESH_CONSTRUCTION_KHV
#define GRAPHALGO_MESH_CONSTRUCTION_KHV

#include "../simdefs.h"
#include "../treealgs/treestructure.h"
#include <map>
#include <list>

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
				namespace GraphAlgorithms
----------------------------------------------------------------------- */
namespace GraphAlgorithms
{

class TAMeshConfig
{
public:
	//TAMeshConfig() : algo(NO_TREE_ALGO), k(0) {}
	TAMeshConfig(TreeAlgorithms::TreeAlgo a, int i) : algo(a), k(i) {}
	~TAMeshConfig() {}
	
	TreeAlgorithms::TreeAlgo algo;
	int k;
};

typedef vector<TAMeshConfig> TAMeshConfigVector;

// mesh start functions
clock_t buildMesh(const TreeStructure &inputT, TreeStructure &outT, int k, int src, GraphAlgo algo);
bool buildMesh(const TreeStructure &inputT, TreeStructure& outT, GraphAlgo algo, int k, int src, clock_t &alg_time, uint64_t &cpu_cycles);

// k trees algorithms
void kShortestPathTree(const TreeStructure &inputT, TreeStructure &outT, int k);
void kDLShortestPathTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src);
void kMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &outT, int k);
void kDLMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src);
void DLBestLinks(const TreeStructure &inputT, TreeStructure &outT);
void kMDDLOTTCTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src);
void kMDOTTCTree(const TreeStructure &inputT, TreeStructure &outT, int k);
void kDLRGHTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src);

// k parallel trees
void kParallelDLRGHTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src);
void kParallelMDDLOTTCTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src);
void kParallelDLShortestPathTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src);
void kParallelDLMinimumSpanningTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src);

// k combined tree algorithms
void kCombinedTreeAlgorithms(const TreeStructure &input, TreeStructure &newT, int k, int src, GraphAlgo algo);

// enhanced tree algorithms
void enhancedTreeAlgorithms(const TreeStructure &inputT, TreeStructure &outT, int k, int src, GraphAlgo algo);
void reduceDiameterOfTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src, GraphAlgo algo);
void reduceEccentricityOfNodes(const TreeStructure &inputT, TreeStructure &outT, int k, int src, GraphAlgo algo);
void addLongLinksToTree(const TreeStructure &inputT, TreeStructure &outT, int k, int src, GraphAlgo algo);
void addCoreLinksToTree(const TreeStructure &inputT, TreeStructure &newT, int k, int src, GraphAlgo algo);

// edge pruning algorithms
void edgePruningAlgs(const TreeStructure &inputT, TreeStructure &outT, int k);
void pruneGraph(const TreeStructure &, TreeStructure &, int k);
void kBL(const TreeStructure &, TreeStructure& , int k);
void aCL_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure& newT, int k);
void aCLO_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk);
void dlaCLO_noSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk);

// for SMT algorithms
void kBestLinksGraph(const TreeStructure &, const VertexSet &coreSet, TreeStructure& , int k);
void kBestLinksOptimizedGraph(const TreeStructure &, const VertexSet &coreSet, TreeStructure &, int k, int sk);
void kBestLinksOptimizedDLGraph(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk);
// for non-SMT algorithms
void kBestLinksGraphNoSP(const TreeStructure &, const VertexSet &coreSet, TreeStructure& , int k);
void kBestLinksOptimizedGraphNoSP(const TreeStructure &, const VertexSet &coreSet, TreeStructure &, int k, int sk);
void kBestLinksOptimizedDLGraphNoSP(const TreeStructure& inputT, const VertexSet &coreSet, TreeStructure &newT, int k, int sk);

};


#endif // GRAPHALGO_MESH_CONSTRUCTION_KHV


