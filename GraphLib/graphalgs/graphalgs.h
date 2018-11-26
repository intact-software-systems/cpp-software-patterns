/***************************************************************************
                          graph_algs.h  -  description
                             -------------------
    begin                : Tue Feb 28 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_GRAPHALGS_KHV
#define GRAPHALGO_GRAPHALGS_KHV

#include <vector>
#include "../boostprop.h"
#include "../simdefs.h"
#include "../network/vertex_set.h"
#include "../treealgs/treestructure.h"

using namespace TreeAlgorithms;
using namespace std;

namespace GraphAlgorithms
{

int generateRandomNumber(double max);
int generateSizeOfSteinerSet(int members);
int generateSizeOfSteinerSet(int members, int steiner);
int generateSizeNonSteiner(int members);

int generateMinEdgeCountNoPartition(int num_vertices);
double generateDelayLimit(int members);

void densifyGraph(const GraphN &g_in, GraphN& dg);
void densifyGraph(const TreeStructure &T, GraphN& dg);
void sparsifyGraph(const GraphN &g_in, GraphN& sg);

void connectPartitionedGraph(const TreeStructure &inputT, TreeStructure &newT);
bool findPartition(const GraphN &g, VertexSet &partV);

// check/debug/fail safe functions
void checkGraph(const GraphN &g);
bool checkIsPartitioned(const GraphN &g);	// check if mesh is connected
void walkGraph(const GraphN &g, vertex_descriptorN u, std::vector<bool> &visited);
void checkMaxDistance(const GraphN &g);
void checkGraphEqualSP(const GraphN &g);
int cntSPViolations(const GraphN &g);
int cntInvalidEdgeWeights(const GraphN &g, int invalid_weight);
int cntInvalidEdgeWeights(const GraphN &g, const VertexSet &invalidV, int invalid_weight);
int cntInvalidVertices(const GraphN &g, VertexSet &invalidVertices, int invalid_weight);

}; // namespace GraphAlgorithms

#endif // GRAPHALGO_GRAPHALGS_KHV

