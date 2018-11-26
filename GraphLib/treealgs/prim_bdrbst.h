/***************************************************************************
                          prim_bdrbst.h  -  description
                             -------------------
    begin                : Fri Jun 30 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_PRIM_BDRBST_KHV
#define GRAPHALGO_PRIM_BDRBST_KHV

#include "../boostprop.h"
#include "./treealgs.h"
#include "../treealgs/fheap.h"

using namespace TreeAlgorithms;

class PrimBDRBST
{
public:
	PrimBDRBST(TreeStructure &out) : T_bdrbst_(out), balanceFactor_(1), degreeLimit_(0) {}
	~PrimBDRBST() {}
	
	void Algorithm(const TreeStructure &, vertex_descriptorN, int, int);
	bool prim_bdrbst(const TreeStructure &, vertex_descriptorN src);
	double vertexDistance(const DistanceVector &rdistance, const ParentVector &rparent, vertex_descriptorN u, vertex_descriptorN v, vertex_descriptorN zsource);
	int findBalanceVertex(const GraphN &g, Heap* heap, const ParentVector &rparent, const DistanceVector &rdistance);
		
private:
	TreeStructure 		&T_bdrbst_;
	int balanceFactor_;
	int degreeLimit_;
	vector<int> degree_bound;
};

#endif // GRAPHALGO_PRIM_BDRBST_KHV




