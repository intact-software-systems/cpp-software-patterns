/***************************************************************************
                          bddlsph.h  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_BOUNDED_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_BOUNDED_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV

#include<stdlib.h>
#include<vector>

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
//using namespace std;
using namespace TreeAlgorithms;

class BDDLShortestPathHeuristic : public SteinerHeuristic
{
private:
	double							maxViolation;
	double							DB;

	TreeStructure		&T_bddlsph;		// Stores the BDDLSPH tree
	GraphN				g;

    ParentVector	 				near_;	// std::vector<int>
    DistanceVector 					ecc;	// std::vector<double>
	numeric::ublas::matrix<double> 	dist; 
	DistanceVector					diameter_bound;
	DistanceVector					degree_bound;
	VertexSet 						diameterBroken, degreeBroken;

public:
	BDDLShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) 
            : SteinerHeuristic(in), maxViolation(0.0), DB(-1), T_bddlsph(out), g(in.g), near_(num_vertices(in.g)), ecc(num_vertices(in.g)), dist(num_vertices(in.g), num_vertices(in.g)), diameter_bound(num_vertices(g)), degree_bound(num_vertices(g)) {}
    ~BDDLShortestPathHeuristic() { }
	
	// BDDLSPH main
	void Algorithm(vertex_descriptorN zsource, double D);

private:
	void init(vertex_descriptorN src, const double &DB);
	void updateDist(int z);
	
	// BDDLSPH helper functions
	bool ExtractClosestZ(PathVector &newPath, int&); 
	void AddTreeSPH(PathVector &newPath); 
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent, const DistanceVector &zdistance);	
	bool CheckPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
};

#endif // GRAPHALGO_BOUNDED_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV




