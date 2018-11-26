/***************************************************************************
                          mddlsph.h  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class MDDLShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure		&T_mddlsph;		// Stores the MDDLSPH tree 
	GraphN				g;
	
    vector<int> 					near_;
	DistanceVector 					ecc;
	numeric::ublas::matrix<double> 	dist; 
	
	vector<int>						degree_bound;
	VertexSet						degreeBroken;
	
public:
	MDDLShortestPathHeuristic(const TreeStructure &in, TreeStructure &out)
            : SteinerHeuristic(in), T_mddlsph(out), g(in.g), near_(num_vertices(in.g)), ecc(num_vertices(in.g)), dist(num_vertices(in.g), num_vertices(in.g)), degree_bound(num_vertices(in.g))	{}
	~MDDLShortestPathHeuristic() { }  
	
	// MDDLSPH main
	void Algorithm(vertex_descriptorN zsource);

private:
	void init(vertex_descriptorN src);

	void updateDist(vertex_descriptorN z);
	
	// MDDLSPH helper functions
	bool ExtractClosestZ(PathVector &newPath, int &z); 
	void AddTreeSPH(PathVector &newPath); 
	bool FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);	
};

#endif // GRAPHALGO_MINIMUM_DIAMETER_DEGREE_LIMITED_SHORTEST_PATH_HEURISTIC_KHV



