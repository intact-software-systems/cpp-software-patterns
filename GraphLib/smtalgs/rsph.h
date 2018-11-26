/***************************************************************************
                          rsph.h  -  description
                             -------------------
    begin                : Thu Jul 26 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_RANDOMIZED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_RANDOMIZED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class RandomizedSPH : SteinerHeuristic
{
private:
	TreeStructure			&T_sph;			// Stores the SPH tree 

public:
	RandomizedSPH(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_sph(out), D(0), depth(num_vertices(in.g)), algo(GlobalSimArgs::getSimTreeAlgo()) {}
	~RandomizedSPH() { }  
	
	typedef pair<vector<int>, int>  	PathVector;
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource, const double &dDB);

	// SPH helper functions
	bool ExtractClosestZ(PathVector &newPath); 
	bool AddTreeSPH(PathVector &newPath); 
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, TreeAlgo algo);
	bool FindPathRGH(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	bool FindPathRH(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);

private:
	double D;
	DistanceVector depth;
	VertexSet vertsWithinDB;
	TreeAlgo algo;
};


#endif /* GRAPHALGO_SHORTEST_PATH_HEURISTIC_KHV */



