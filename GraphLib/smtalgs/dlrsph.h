/***************************************************************************
                          dlrsph.h  -  description
                             -------------------
    begin                : Thu Jul 26 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DEGREE_LIMITED_RANDOMIZED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_DEGREE_LIMITED_RANDOMIZED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class DLRandomizedSPH : SteinerHeuristic
{
private:
	TreeStructure			&T_sph;			// Stores the SPH tree 

public:
	DLRandomizedSPH(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_sph(out), D(0), depth(num_vertices(in.g)), degree_bound(num_vertices(in.g)) {}
	~DLRandomizedSPH() { }  
	
	typedef pair<vector<int>, int>  	PathVector;
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource, const double &DB);

	// SPH helper functions
	bool ExtractClosestZ(PathVector &newPath, ShortestPathKeeper &spk, VertexSet &); 
	bool AddTreeSPH(PathVector &newPath, ShortestPathKeeper &spk); 
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	bool CheckPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, VertexSet &degreeBroken);

private:
	double D;
	DistanceVector depth, degree_bound;
	VertexSet vertsWithinDB;
};


#endif 




