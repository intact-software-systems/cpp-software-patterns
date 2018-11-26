/***************************************************************************
                          tree_dynamics.h  -  description
                             -------------------
    begin                : Wed Apr 26 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_TREE_DYNAMICS_KHV
#define GRAPHALGO_TREE_DYNAMICS_KHV

#include "../boostprop.h"
#include "treestructure.h"
#include "../network/group_info.h"
#include "../graphalgs/steiner_set.h"
#include "../simdefs.h"

using namespace std;
using namespace boost; 

/*-----------------------------------------------------------------------
					class TreeDynamics()
------------------------------------------------------------------------- */
class TreeDynamics
{
public:
	TreeDynamics(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner, vertex_descriptorN actionVert)
		 : globalG_(g) 
		 , groupInfo_(groupInfo)
		 , fifoSteiner_(fifoSteiner)
		 , groupT_(groupInfo.treeStructure_)
		 , actionVert_(actionVert) 
		 , treeAlgo_(NO_DYNAMIC_TREE_ALGO) {}
	TreeDynamics(const GraphN &g, GraphLib::GroupInformation& groupInfo, SteinerPoints& fifoSteiner)
		 : globalG_(g) 
		 , groupInfo_(groupInfo)
		 , fifoSteiner_(fifoSteiner)
		 , groupT_(groupInfo.treeStructure_)
		 , treeAlgo_(NO_DYNAMIC_TREE_ALGO) {}
		~TreeDynamics() {}

	// typedefs	
	typedef multimap<int, vertex_descriptorN, greater<int> > OutDegreeMap;

	// main functions
	//virtual void insertVertex();
	//virtual void removeVertex();
	
	// subgraph
	clock_t subTree(TreeStructure &groupT, TreeStructure &subT, VertexSet& steinerSet);
	
	// help functions	
	//void subGraphRingSP(SimVertexMap& subVertices, VertexSet& steinerSet, vertex_descriptorN actVert, vertex_descriptorN src, vertex_descriptorN prev, EdgeList& remEdges, int k);
	void subGraphRingSP(VertexSet& subVertices, VertexSet& steinerSet, vertex_descriptorN actVert, vertex_descriptorN src, vertex_descriptorN prev, EdgeList& remEdges, int k);
	void getNeighborsOptimizeSP(const TreeStructure &groupT, vertex_descriptorN src, vertex_descriptorN prev, VertexSet &adjacentSet, VertexSet &adjacentSetSP, EdgeList &remEdges);
	void getNeighbors(const TreeStructure &, vertex_descriptorN src, VertexSet &adjacentSet);
	void getNeighbors(const TreeStructure &, vertex_descriptorN src, VertexSet &adjacentSet, OutDegreeMap &adjacentMap);

	void connectComponentsDisjoint(TreeStructure &groupT, const VertexSet &One, const VertexSet &Two);
	void connectComponentsEqual(TreeStructure &groupT, const VertexSet &One, const VertexSet &Two);
	
	// degree bounded
	void connectMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV);
	void connectMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV);
	void connectSearchMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV);
	void connectSearchMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV);
		
	// delay constrained and degree bounded
	void connectDCMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance);
	void connectDCMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance);
	void connectDCSearchMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance);
	void connectDCSearchMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance);

	// bounded diameter and degree bounded
	void connectBDMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound);
	void connectBDMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound);
	void connectBDSearchMCE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound);
	void connectBDSearchMDE(TreeStructure &groupT, OutDegreeMap &, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound);

	// unconstrained
	void findMCEdge(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w);
	//void findMDEdge(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w);
	void findMDEdge(vertex_descriptorN src, const double &src_eccentricity, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &current_diameter);

	// delay constrained
	void findMCEdgeC(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &);
	void findMDEdgeC(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &);

	// diameter bounded
	void findMCEdgeBD(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance, const double &diameterBound);
	void findMDEdgeBD(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance, const double &diameterBound);

	// unused
	void findBNP(const GraphN &tree_g, vertex_descriptorN actVert, vertex_descriptorN start_v, vertex_descriptorN prev_v, double &bcNeighborHood, int &numNeighbors, edge_descriptorN &new_e);
	void findMDBNP(const GraphN &tree_g, vertex_descriptorN actVert, vertex_descriptorN start_v, vertex_descriptorN prev_v, double &bcNeighborHood, int &numNeighbors, edge_descriptorN &new_e);
	void calcNeighborHood(vertex_descriptorN actionVert, vertex_descriptorN src, vertex_descriptorN prev, double& w, int& n, int k);

	// debug
	void checkRemove(vertex_descriptorN rem_vert);
	void checkInsert(vertex_descriptorN ins_vert);
	
	void treeAlgo(DynamicTreeAlgo algo) 		{ treeAlgo_ = algo; }
	DynamicTreeAlgo treeAlgo()			const   { return treeAlgo_; }

protected:
	const GraphN		&globalG_;
	GraphLib::GroupInformation 	&groupInfo_;
	SteinerPoints 		&fifoSteiner_;
	TreeStructure		&groupT_;
	vertex_descriptorN	actionVert_;

	DynamicTreeAlgo		treeAlgo_;
};
 	
 
#endif 


