/***************************************************************************
                          coreh.h  -  description
                             -------------------
    begin                : Thu Oct 6 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_CORE_HEURISTIC_KHV
#define GRAPHALGO_CORE_HEURISTIC_KHV

#include "graphalgs.h"
#include "../network/vertex_set.h"
#include "../treealgs/treestructure.h"
#include "../simdefs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

namespace GraphAlgorithms
{

std::ostream& operator<<(std::ostream& ostr, const std::pair<double, double>& p);

// -- start function for WCN selection --
void findWCNodes(const GraphN &g, VertexSet &vertexSet, int k, CoreSelectionAlgo algo);
void findWCNodes(const TreeStructure &T, VertexSet &vertexSet, int k, CoreSelectionAlgo algo);
int findWCNodes(const TreeStructure &T, CoreSelectionAlgo algo);
int findWCMemberNode(const TreeStructure &T, CoreSelectionAlgo algo);

// -- find WCNodes Tailed --
class Rectangle 
{
public:
	Rectangle(pair<double, double> c1, pair<double, double> c2, VertexSet set, multimap<double, int> arrC, bool arr) : 
					C1(c1), C2(c2), vSet(set), arrCoord(arrC), arrX(arr) {}
	Rectangle() : vSet(), arrCoord(), arrX(true) {}
	~Rectangle() {}

	pair<double, double> C1, C2;
	VertexSet vSet;
	multimap<double, int> arrCoord;
	bool arrX;
};	

void findWCNodesTailedSpread(const GraphN &g, VertexSet& vertexSet, int k);
void findWCNodesTailedSpread(const TreeStructure &T, VertexSet& vertexSet, int k);
void findClosestVertXY(const Rectangle &rect, VertexSet &split, int &newWCN);
void findClosestVertXY(const VertexSet &V, const Rectangle &rect, VertexSet &split, int &newWCN);
void makeSortedCoord(const GraphN &g, Rectangle &rect);
void removeRectangle(multimap<int, Rectangle> &mapRect, const Rectangle &max);

// -- average/median WCN selection -- 
void kWorstLocatedNodesFromGraph(const TreeStructure &T, VertexSet& vertexSet, int k);
void kBestLocatedNodesFromGraph_Average(const GraphN &g, VertexSet& v, int);
void kBestLocatedNodesFromGraph_Average(const TreeStructure &T, VertexSet& v, int);
void kBestLocatedMemberNodesFromGraph_Average(const TreeStructure &T, VertexSet& v, int);
void kBestLocatedNodesFromGraph_Median(const GraphN &g, VertexSet& v, int);
void kBestLocatedNodesFromGraph_Median(const TreeStructure &, VertexSet& v, int);
void kBestLocatedMemberNodesFromGraph_Median(const TreeStructure &, VertexSet& v, int);
void kBestLocatedNodesFromGraphNotInSet_Median(const TreeStructure &, VertexSet& vertexSet, const VertexSet& inSet, int k);

// -- uniform WCN selection --
void findWCNodesUniformSpread(const GraphN &g, VertexSet& vertexSet, int k);
void recurseGraph(const GraphN &g, VertexSet &vertexSet, int k, int recursed, double X, double Y);
int findClosestVertXY(const GraphN &g, double X, double Y);

// -- choose WCN based on bandwidth --
void findWCNodesBandwidth(const GraphN &g, VertexSet& vertexSet, int k);
void findWCNodesBandwidth(const TreeStructure &T, VertexSet& vertexSet, int k);
void findWCMemberNodesBandwidth(const TreeStructure &T, VertexSet& vertexSet, int k);

// -- various functions --
int findBestLocatedMemberNode(const TreeStructure &);	
int	findBestLocatedMemberNodeDegreeBound(const TreeStructure &);
int	findBestLocatedNodeDegreeBound(const TreeStructure &);

// find k best exept
void findBestLocatedMemberNode(const TreeStructure &T, const VertexSet &exeptV, VertexSet &newV, int k);
void findBestLocatedMemberNodeConnectedToV(const TreeStructure &T, const VertexSet &connectedToV, VertexSet &newV, int k);
void findBestLocatedMemberNodeConnectedToV(const GraphN &g, const VertexSet &V, const VertexSet &connectedToV, VertexSet &newV, int k);

int findCenterNode(const VertexSet &V, const GraphN &g);
int	findMDNode(const VertexSet &V, const GraphN &g);
int	findMDDLNode(const VertexSet &, const GraphN &g);

int findMDDLMemberNode(const VertexSet &, const GraphN &g);
int	findMDDLWCNode(const VertexSet &, const GraphN &g);
int findMDDLSteinerNode(const VertexSet &, const GraphN &g);

int	findDCMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);
int	findDCMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);

// multiple core algorithms
void kCenterNodes(const GraphN &g, const VertexSet &V, VertexSet &coreSet, int k);
void kDLCenterNodes(const GraphN &g, const TreeStructure &T, VertexSet &coreSet, int k);

// bounded radius
int	findBDMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);
int	findBDMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);

// bounded radius
int	findBRMDDLNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);	
int	findBRMDDLWCNode(const VertexSet &V, const GraphN &g, const GraphN &global, const DistanceVector &distance, int dest);

}; // GraphAlgorithms

#endif /* GRAPHALGO_CORE_HEURISTIC_KHV */



