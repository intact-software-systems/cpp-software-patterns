/***************************************************************************
                          rcsph.h  -  description
                             -------------------
    begin                : Tue Sep 26 2007
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"

using namespace boost;
using namespace TreeAlgorithms;

class RCShortestPathHeuristic : SteinerHeuristic
{
public:
	RCShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_rcsph_(out), radius_bound(num_vertices(in.g)) {}
	~RCShortestPathHeuristic() { }  
	
	void Algorithm(vertex_descriptorN zsource, double radiusBound);
	
	// class functions
	bool ExtractClosestZ(PathVector &newPath, vertex_descriptorN src);
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	void AddTree(PathVector &newPath);

private:
	void init(double radius);	

private:
	TreeStructure	&T_rcsph_;			// Stores the SPH tree
	DistanceVector radius_bound;
	VertexSet radiusBroken;
};

#endif // GRAPHALGO_RADIUS_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV


