/***************************************************************************
                          remove_dynamics.h  -  description
                             -------------------
    begin                : Thu Aug 24 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_REMOVE_DYNAMICS_KHV
#define GRAPHALGO_REMOVE_DYNAMICS_KHV

#include "../boostprop.h"
#include "treealgs.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"
#include "tree_dynamics.h"

using namespace std;
using namespace boost; 

/*-----------------------------------------------------------------------
					class RemoveDynamics()
------------------------------------------------------------------------- */
class RemoveDynamics : TreeDynamics
{
public:
	RemoveDynamics(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, vertex_descriptorN actionVert)
		   	: TreeDynamics(g, groupInfo, fifoSteiner, actionVert) {}
	~RemoveDynamics() {}

	// main function
	void removeVertex(const DynamicTreeAlgo &);
 	clock_t removeVertex_in(const DynamicTreeAlgo &);

	// basic remove functions
	void rvLeaf();
	void rvODTwo();
	void rvUnusedSP();
	
	// remove functions
	void rvKeepAsSP();
	void rvKeepWCN();
	void rvTryReplaceNaive();
	void rvTryReplace();
	void rvTRMDDLN();

	// minimum cost versions
	void rvMCEdge();
	void rvSearchMCEdge();

	// minimum diameter versions
	void rvMDEdge();
	void rvSearchMDEdge();

	// subgraph
	void rvSubGraph(TreeStructure &groupT, TreeStructure &subT, VertexSet& steinerSet);
	
	// unused
	void rvMDBNP();
	void rvBNP();
	void rvReplaceWithSP();
	
	void treeAlgo(DynamicTreeAlgo algo) 		{ treeAlgo_ = algo; }
	DynamicTreeAlgo treeAlgo()			const   { return treeAlgo_; }

private:

	// -- unused --
	struct subVertInfo { 
	public:
		subVertInfo(edge_descriptorN e, vertex_descriptorN sub) : new_edge(e), subVert(sub) {}
		subVertInfo(edge_descriptorN e, vertex_descriptorN sub, vertex_descriptorN act) : new_edge(e), subVert(sub), actVert(act) {}

		edge_descriptorN new_edge; 
		vertex_descriptorN subVert;
		vertex_descriptorN actVert;
	};
	// -- unused end --
};
 	
 
#endif 



