/***************************************************************************
                          dijkstra_sp.h  -  description
                             -------------------
    begin                : Mon Mar 20 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DIJKSTRA_SHORTEST_PATH_KHV
#define GRAPHALGO_DIJKSTRA_SHORTEST_PATH_KHV

#include "../boostprop.h"
//#include "treealgs.h"
#include <vector>
#include "../network/vertex_set.h"

using namespace boost;
using namespace std;

namespace TreeAlgorithms
{

// simple typedefs
typedef vector<double>			DistanceVector;
typedef vector<int>				ParentVector;

void myDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent);
void myDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes);
void myDijkstraSPT_TieBr(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes);

bool myDBDijkstraSPT(const GraphN &g, int src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes);


	
}; // namespace TreeAlgorithms

#endif // GRAPHALGO_DIJKSTRA_SHORTEST_PATH_KHV



