/***************************************************************************
                          reconfig_algs.h  -  description
                             -------------------
    begin                : Tue Sep 5 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_RECONFIG_ALGS_KHV
#define GRAPHALGO_RECONFIG_ALGS_KHV

#include <fstream>
#include "../treealgs/treealgs.h"
#include "../treealgs/tree_dynamics.h"
#include "../boostprop.h"


using namespace std;
using namespace boost;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
						class ReconfigAlgs
------------------------------------------------------------------------- */
class ReconfigAlgs : TreeDynamics
{
public:
	ReconfigAlgs(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints &fifoSteiner)
		   	: TreeDynamics(g, groupInfo, fifoSteiner) {}
	~ReconfigAlgs() {}

	
public:
	void reconfigTree();
	
	void reconfigMSP();
	void pruneRTR(TreeStructure &T);
	void pruneNTLeaves(TreeStructure &T, VertexSet &S);
};

#endif 

