/***************************************************************************
                          treealgs.h  -  description
                             -------------------
    begin                : Tue Jan 31 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_TREE_ALGORITHMS_KHV
#define GRAPHALGO_TREE_ALGORITHMS_KHV

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <queue>
#include <map>
#include <assert.h>
#include "./treestructure.h"
#include "../boostprop.h"
#include "../simdefs.h"
#include "../network/vertex_set.h"

using namespace std;
using namespace boost;

class StatsContainer;

namespace TreeAlgorithms
{
/*-----------------------------------------------------------------------
	TreeAlgorithms - Basic Tree algorithims - SPT, MST and more?
------------------------------------------------------------------------- */
enum COLOR { WHITE = 0, GRAY, BLACK};

void getPrimMST(const TreeStructure &inputT, TreeStructure &outT, int);
void getDBPrimMST(const TreeStructure &inputT, TreeStructure &outT, int);

void getDijkstraSPT(const TreeStructure &inputT, TreeStructure &outT, int);
void getDBDijkstraSPT(const TreeStructure &inputT, TreeStructure &outT, int);

void getServerSPT(const TreeStructure &inputT, TreeStructure &outT, int source);
void getServerSPT(const GraphN &g, TreeStructure &outT, int source);

/*-----------------------------------------------------------------------
					TreeAlgorithms - help functions
------------------------------------------------------------------------- */
clock_t buildTree(const TreeStructure &inputT, TreeStructure& outT, TreeAlgo treeAlgo, int src);
bool 	buildTree(const TreeStructure &inputT, TreeStructure& outT, TreeAlgo treeAlgo, int src, clock_t &alg_time, uint64_t &time);

bool isValidTree(const TreeStructure &newT, const TreeStructure &inputT);
bool isValidMesh(const TreeStructure &outT, const TreeStructure &inputT);
void recordStats(std::iostream& ostr, StatsContainer* SC);
void flushStats(std::iostream& ostr);

int meshifyGraph(GraphN &g, VertexSet &V);
//int getOutDegree(const GraphN &g, int u); // in treestructure.h/cc
//int getOutDegree(int u, const GraphN &g);
int getAvOutDegree(const GraphN &g, const TreeStructure &T, int u);
int getAvOutDegree(const GraphN &g, const TreeStructure &T, const VertexSet &V);
int getVertLargestAvOD(const GraphN &g, const TreeStructure &T, const VertexSet &V);

int getTarget(const EdgeId &id, int src);
int getSource(const EdgeId &id, int targ);

int getMinODNeedConnectV(int numV);
//bool isSteinerAlgo(const TreeAlgo &algo); // in simdefs.h/cc
//bool isDBAlgo(const TreeAlgo &algo);
//bool isDBAlgo(const DynamicTreeAlgo &algo);
//bool isVertex(const GraphN &g, int u);  // in treestructure.h/cc
//bool isEdge(const GraphN &g, int u, int v);
void addVertex(GraphN &g, int v);

bool isRelaxWorking(const VertexSet &degreeBroken, const VertexSet &diameterBroken);
bool relaxDegreeAndDiameter(const VertexSet &degreeBroken, DistanceVector &degree_bounds, const VertexSet &diameterBroken, DistanceVector &diameter_bounds);
bool relaxDegreeAndRadius(const VertexSet &degreeBroken, DistanceVector &degree_bounds, const VertexSet &radiusBroken, DistanceVector &radius_bounds);
bool relaxDegreeAndRadius(const VertexSet &degreeBroken, DistanceVector &degree_bounds, double &bound); 
bool relaxDegree(const VertexSet &setBroken, ParentVector &bounds);
bool relaxDegree(const VertexSet &setBroken, DistanceVector &bounds);
bool relaxDelay(const VertexSet &setBroken, DistanceVector &bounds);
bool relaxRadius(const VertexSet &setBroken, DistanceVector &bound);
bool relaxRadius(double &bound);
bool relaxDiameter(const VertexSet &setBroken, DistanceVector &bounds);
bool relaxDiameter(double &bound);
void relaxSet(const VertexSet &setBroken, DistanceVector &bounds, double inc);
void relaxSet(const VertexSet &setBroken, ParentVector &bounds, int inc);

typedef std::list<int> PathList;

double getTotalCost(const TreeStructure &T);
double getTreeDiameter(const TreeStructure &T);
double getTreeDiameter(const TreeStructure &T, PathList &tree_path);
double getTreeDiameter(const GraphN &g, const VertexSet &V, PathList &tree_path);
double getTreeDiameter(const TreeStructure &T, int src);

void worstCaseDist(const GraphN &g, const VertexSet &V, TreeDiameter &TD, int src);
void worstCaseDist(int src, int prev, int curr, const TreeStructure &tree, double &worstcase_dist);
void worstCaseDist(int src, int prev, int curr, const GraphN &tree_g, double &worstcase_dist, PathList &path);
void worstCaseDist(int src, int prev, int start, const GraphN &tree_g, double &worstcase_dist);
void worstCaseCost(int src, int prev, int start, const GraphN &tree_g, double &worstcase_dist);
void worstCaseCost(int src, int prev, int start, const GraphN &tree_g, pair<double, double> &worstcase_dist);

void wcVertexDistances(const TreeStructure &T, TreeDiameter &treeDiameter, int src);
double vertexDistance(const GraphN &g, const DistanceVector &rdistance, const ParentVector &rparent, int u, int v, int zsource);
void wcDistancesOTTC(const GraphN &g, const VertexSet &treeNodes, TreeDiameter &TD, int src);

void createPDVectors(const GraphN &g, ParentVector &parent, DistanceVector &distance, int src);
void createParentVector(int prev_v, int start_v, const GraphN &tree_g, ParentVector &parent, DistanceVector &distance);

void createTreeFromParent(const GraphN &g, TreeStructure& treeStructure, const ParentVector &zparent);
void createGraphFromTree(GraphN& gOut, const TreeStructure &treeStructure);
void updateTreeIdDenseToSparse(TreeStructure &T);

void pruneNonTerminalLeaves(TreeStructure &T);

/*-----------------------------------------------------------------------
				TreeAlgorithms - dump/debug etc.
------------------------------------------------------------------------- */

// check/debug functions
void checkTree(const TreeStructure &t);
void checkDijkstra(const ParentVector &p, const DistanceVector &distances);
void checkHasLoop(const TreeStructure &T);
void walkTree(const GraphN &g, int start, int u, int v, VertexSet &visited);

// dump/print functions for TreeStructure
void dumpTreeStructure(const TreeStructure &T);
void dumpDijkstra(const ParentVector &p, const DistanceVector &distances);
void dumpGraph(const GraphN &g);
void prettyPrint(const TreeStructure &);

//void createBoostGraphIndex(GraphN& g_out, const GraphN &g_in, TreeStructure &T);
//void createBoostGraphId(GraphN& g_out, const GraphN &g_in, EdgePair simEdges, VertexSet simVertices);
//void createBoostGraphIndex(GraphN& g_out, const GraphN &g_in, EdgeList simEdges, VertexSet simVertices);
//void dumpTreeStructure_full(TreeStructure T);
//void createSimTree(TreeStructure treeStructure, SimGraph& simTree);
//void createSimGraph(const GraphN &g, SimGraph& simGraph);
//void createBoostGraph(GraphN& g, SimGraph simGraph);
//void createBoostGraph(GraphN& g, SimEdgeList simEdges, SimVertexMap simVertices);
//void createBoostGraph(GraphN& g_out, TreeStructure T);

}; // namespace TreeAlgorithms

#endif // GRAPHALGO_TREE_ALGORITHMS_KHV


