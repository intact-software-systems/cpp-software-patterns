/***************************************************************************
                          dsph.h  -  description
                             -------------------
    begin                : Wed Nov 22 2006
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class DLShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure			&T_sph;			// Stores the SPH tree 
	GraphN 					g;

	DistanceVector			degree_bound;
	VertexSet				degreeBroken;

	void init();

public:
	DLShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_sph(out), g(in.g), degree_bound(num_vertices(in.g)) { }
	~DLShortestPathHeuristic() { }  
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource);

	// SPH helper functions
	bool ExtractClosestZ(PathVector &newPath); 
	void AddTreeSPH(PathVector &newPath); 
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);	
};


#endif // GRAPHALGO_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV 



