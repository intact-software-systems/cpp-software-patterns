/***************************************************************************
                          prim_ottc.h  -  description
                             -------------------
    begin                : Wed May 30 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_PRIM_OTTC_MINIMUM_SPANNING_TREE_KHV
#define GRAPHALGO_PRIM_OTTC_MINIMUM_SPANNING_TREE_KHV

#include "treealgs.h"
#include "fheap.h"
#include <vector>

using namespace boost;
using namespace std;

namespace TreeAlgorithms
{

bool OTTC(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes);
bool dlOTTC(double D, const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes);
bool mdOTTC(const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes);
bool mddlOTTC(const TreeStructure &Tin, TreeStructure &T, vertex_descriptorN src, const VertexSet &treeNodes);

// with distance/parent vectors
bool OTTC(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes);
bool dlOTTC(double D, const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes);
bool mdOTTC(const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes);
bool mddlOTTC(const GraphN &g, vertex_descriptorN src, DistanceVector &distance, ParentVector &parent, const VertexSet &treeNodes);

};


#endif // GRAPHALGO_PRIM_OTTC_MINIMUM_SPANNING_TREE_KHV

