/***************************************************************************
                          sph.h  -  description
                             -------------------
    begin                : Thu Sep 14 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class ShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure			&T_sph;			// Stores the SPH tree 

public:
	ShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_sph(out) {}
	~ShortestPathHeuristic() { }  
	
	typedef pair<vector<int>, int>  	PathVector;
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource);

	// SPH helper functions
	void ExtractClosestZ(PathVector &newPath); 
	void AddTreeSPH(PathVector &newPath); 
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);

	// experimented using prim MST -- not too good :)
	//void ExtractClosestZ_prim(PathVector &newPath);
	//double FindPathDistance_prim(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, ParentVector zparent, DistanceVector zdistance);
};


#endif /* GRAPHALGO_SHORTEST_PATH_HEURISTIC_KHV */


