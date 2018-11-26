/***************************************************************************
                          dcsp.h  -  description
                             -------------------
    begin                : Thu Mar 22 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_KHV
#define GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_KHV

#include "steiner_class.h"
#include "../boostprop.h"
#include <vector>

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

/*-----------------------------------------------------------------------
					class DCDBShortestPath
------------------------------------------------------------------------- */
class DCDBShortestPath : SteinerHeuristic
{
public:
	DCDBShortestPath(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_dcsp_(out), zintreeCnt_(0) {}
	~DCDBShortestPath() {}

	// typedefs definitions
	enum COLOR { WHITE = 0, GRAY, BLACK};
	typedef vector<double> 				DistanceVector;
	typedef vector<vertex_descriptorN> 	ParentVector;
	typedef vector<bool>				BoolVector;
	typedef vector<COLOR>				ColorVector;
	typedef multimap<double, vertex_descriptorN> WeightMap;
	typedef map<vertex_descriptorN, WeightMap> ParentWeightMap;
	
	void Algorithm(vertex_descriptorN, double);
	void init();

	// class functions
	void DCDBDijkstraSP(vertex_descriptorN src, double delayLimit);
	void DBDijkstraZ(vertex_descriptorN src);	
	
	bool createTree(vertex_descriptorN src, TreeStructure& treeStructure, bool zintree, const ParentVector &zparent);
	void combineTrees(const TreeStructure &treeDCSP, const TreeStructure &treeZ);
	void combineTrees(const TreeStructure &treeDCSP, const VertexSet &setZ);

	
	// -- new --
	void identifyLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ, vector<EdgeId> &loopEdges);
	void getLoopEdges(const ParentVector &zparent, const TreeStructure &tree, vertex_descriptorN intersection, vertex_descriptorN src, vector<EdgeId> &loopEdges);
	
	void removeLoopEdges(const TreeStructure &treeDCSP, const TreeStructure &treeZ, const vector<EdgeId> &loopEdges);
	
	// -- old --
	//void removeLoops(vertex_descriptorN src, double delayLimit, const TreeStructure &treeDCSP, const TreeStructure &treeZ);
	//void removeFromTree(const ParentVector &zparent, vertex_descriptorN intersection, vertex_descriptorN src, VertexSet &reV);
	
private:
	TreeStructure 		&T_dcsp_;	
	ColorVector			colorDCSP_, colorZ_;
	
	// DCDBSP
	DistanceVector		zdistanceDCSP_, zcostDCSP_;
	ParentVector 		zparentDCSP_;
	vector<int> 		outdegreeDCSP_; 
	vector<VertexSet> 	undiscoveredDCSP_; 
	ParentWeightMap		outedgesDCSP_;

	// DBDijkstraZ	
	DistanceVector		zdistanceZ_;
	ParentVector 		zparentZ_;
	vector<int> 		outdegreeZ_; 
	vector<int>    		degboundsZ_;
	vector<VertexSet> 	undiscoveredZ_;
	ParentWeightMap		outedgesZ_;
	
	BoolVector			zintree_;
	int					zintreeCnt_;
};

#endif //GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_KHV 



