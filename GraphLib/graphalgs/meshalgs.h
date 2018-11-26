/***************************************************************************
                          meshalgs.h  -  description
                             -------------------
    begin                : Mon Mar 31 2008
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_MESHALGS_KHV
#define GRAPHALGO_MESHALGS_KHV

#include <vector>
#include "../network/vertex_set.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/floyd_warshall_apsp.h"
#include "../treealgs/treealgs.h"

using namespace TreeAlgorithms;
using namespace std;

namespace GraphAlgorithms
{
void maxDistance(const GraphN &g, const VertexSet &V, int src, DistanceVector &zdistance, DistanceVector &hops, ParentVector &zparent, double &eccentricity, int &node_in_eccentricity_path);
void maxDistance(const TreeStructure &T, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent, double &eccentricity, int &node_in_eccentricity_path);
void maxDistance(const TreeStructure &T, int v, DistanceVector &distance, DistanceVector &hops, ParentVector &parent);
double maxDistanceHops(const TreeStructure &T, int v, DistanceVector &distance);
void maxDistanceHops(const TreeStructure &T, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent);

double eccentricity_distance(const TreeStructure &T, int src);
double eccentricity_distance(const GraphN &g, const VertexSet &V, int src);
double eccentricity_distance(const GraphN &g, const VertexSet &V, int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent);
double eccentricity_hops(const TreeStructure &T, int src);

double diameter_hops(const TreeStructure &T);
void diameter_distance(const TreeStructure &T, PathList &diameterPath, double &diameter);
double diameter_distance(const TreeStructure &T);

void distanceMatrixHops(const TreeStructure &T, DistanceVectorMatrix &D);
void distanceMatrix(const TreeStructure &T, DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent);

void printAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path);

};

#endif // ..

