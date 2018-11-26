/***************************************************************************
                          floyd_warshall_apspt.cc  -  description
                             -------------------
    begin                : Thu Jul 26 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef KHV_FLOYD_WARSHALL_APSP
#define KHV_FLOYD_WARSHALL_APSP

#include "../boostprop.h"
#include "../network/vertex_set.h"
#include <map>

using namespace std;
using namespace boost;

namespace TreeAlgorithms
{

typedef std::map<int, std::map<int, double> > DistanceMatrix;
typedef std::map<int, std::map<int, int> > 	ParentMatrix;
typedef vector< vector<double> > DistanceVectorMatrix;
typedef vector< vector<int> > ParentVectorMatrix;

void floyd_warshall_apsp(const GraphN &g, DistanceMatrix& d, ParentMatrix& p, const VertexSet &V);
void floyd_warshall_apsp(const GraphN &g, DistanceMatrix& d, ParentMatrix& p);
void floyd_warshall_apsp(const GraphN &g, DistanceVectorMatrix& d, ParentVectorMatrix& p, const VertexSet &V);

};

#endif 

