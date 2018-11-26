/***************************************************************************
                          rcdlsph.h  -  description
                             -------------------
    begin                : Tue Sep 26 2007
    copyright            : (C) 2006 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_RADIUS_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_RADIUS_CONSTRAINED_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class RCDLShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure			&T_rcdlsph;			// Stores the SPH tree 
	GraphN 					g;

	DistanceVector			degree_bound, radius_bound;
	VertexSet				degreeBroken, radiusBroken;

	void init(double radius);

public:
	RCDLShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_rcdlsph(out), g(in.g), degree_bound(num_vertices(in.g)), radius_bound(num_vertices(in.g)) { }
	~RCDLShortestPathHeuristic() { }  
	
	// SPH main
	void Algorithm(vertex_descriptorN zsource, double radius);

	// SPH helper functions
	bool ExtractClosestZ(PathVector &newPath, vertex_descriptorN src); 
	void AddTreeSPH(PathVector &newPath); 
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);	
};


#endif // GRAPHALGO_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV 




