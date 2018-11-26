/***************************************************************************
                          prim_mddbst.h  -  description
                             -------------------
    begin                : Mon Jun 26 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_PRIM_MDDBST_KHV
#define GRAPHALGO_PRIM_MDDBST_KHV

//#include "../smtalgs/steiner_class.h"
#include "../boostprop.h"
#include "./treealgs.h"

using namespace TreeAlgorithms;

class PrimMDDBST
{
public:
	PrimMDDBST(TreeStructure &Tout) : T_mddbst_(Tout), failed_(false) {}
	~PrimMDDBST() {}
	
	void Algorithm(const TreeStructure &, vertex_descriptorN, int);
	bool prim_mddbst(const TreeStructure &inputT, vertex_descriptorN src);
	double vertexDistance(const DistanceVector &rdistance, const ParentVector &rparent, vertex_descriptorN u, vertex_descriptorN v, vertex_descriptorN zsource);
		
private:
	TreeStructure 		&T_mddbst_;	
	int 				degreeLimit_;
	bool				failed_;
	vector<int>			degree_bound;
};

#endif // GRAPHALGO_PRIM_MDDBST_KHV



