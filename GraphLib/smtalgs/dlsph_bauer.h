/***************************************************************************
                          dlsph_bauer.h  -  description
                             -------------------
    begin                : Wed Nov 22 2006
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_DEGREE_LIMITED_BAUER_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_DEGREE_LIMITED_BAUER_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class DLBauerShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure			&T_sph;			// Stores the SPH tree 
	GraphN 					g;

public:
	DLBauerShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_sph(out), g(in.g) {}
	~DLBauerShortestPathHeuristic() { }  
	
	typedef pair<vector<int>, int>  	PathVector;
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource);

	// SPH helper functions
	bool ExtractClosestZ(PathVector &newPath); 
	bool AddTreeSPH(PathVector &newPath); 
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);	
	bool removeDCNodes();
};


#endif // GRAPHALGO_DEGREE_LIMITED_BAUER_SHORTEST_PATH_HEURISTIC_KHV



