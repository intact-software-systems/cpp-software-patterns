/***************************************************************************
                          bdsph.h  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_BOUNDED_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_BOUNDED_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class BDShortestPathHeuristic : SteinerHeuristic
{
private:
	double							maxViolation;
	double 							DB; 	// diameter bound

	TreeStructure			&T_bdsph;			// Stores the BDSPH tree
	
    vector<int> 					near_;
	DistanceVector 					ecc;
	numeric::ublas::matrix<double> 	dist; 
	DistanceVector					diameter_bound;
	VertexSet						diameterBroken;
	
public:
	BDShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) 
            : SteinerHeuristic(in), maxViolation(0.0), DB(-1), T_bdsph(out), near_(num_vertices(in.g)), ecc(num_vertices(in.g)), dist(num_vertices(in.g), num_vertices(in.g)), diameter_bound(num_vertices(in.g)) {}
	~BDShortestPathHeuristic() { }  
	
	// BDSPH main
	void Algorithm(vertex_descriptorN zsource, double D);

private:
	void init(vertex_descriptorN src, const double &DB);
	void updateDist(int z);

	// BDSPH helper functions
	bool ExtractClosestZ(PathVector &newPath, int&); 
	void AddTreeSPH(PathVector &newPath); 
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, const DistanceVector &zdistance);
};

#endif // GRAPHALGO_BOUNDED_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV


