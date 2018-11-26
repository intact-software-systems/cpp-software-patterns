/***************************************************************************
                          prim_mst.h  -  description
                             -------------------
    begin                : Mon Mar 20 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_PRIM_MINIMUM_SPANNING_TREE_KHV
#define GRAPHALGO_PRIM_MINIMUM_SPANNING_TREE_KHV

#include "../boostprop.h"
#include "treealgs.h"
#include <vector>

using namespace boost;
using namespace std;

namespace TreeAlgorithms
{

void myPrimMST(const GraphN &g, vertex_descriptorN src, DistanceVector &zdistance, ParentVector &zparent);
void myPrimMST(const GraphN &g, vertex_descriptorN src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes);
bool myDBPrimMST(const GraphN &g, vertex_descriptorN src, DistanceVector &zdistance, ParentVector &zparent, const VertexSet &treeNodes);

}; // namespace TreeAlgorithms

#endif // GRAPHALGO_PRIM_MINIMUM_SPANNING_TREE_KHV

