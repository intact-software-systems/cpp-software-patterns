/***************************************************************************
                          mdsph.h  -  description
                             -------------------
    begin                : Mon Jun 11 2007
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/treealgs.h"

using namespace boost;
using namespace std;
using namespace TreeAlgorithms;

class MDShortestPathHeuristic : SteinerHeuristic
{
private:
	TreeStructure						&T_mdsph;		// Stores the MDSPH tree 
	
    vector<int> 						near_;
	DistanceVector 						ecc;
	numeric::ublas::matrix<double> 		dist; 
	
public:
	MDShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : 
        SteinerHeuristic(in), T_mdsph(out), near_(num_vertices(in.g)), ecc(num_vertices(in.g)), dist(num_vertices(in.g), num_vertices(in.g)) {}
	~MDShortestPathHeuristic() { }  
	
	// MDSPH main
	void Algorithm(vertex_descriptorN zsource);

private:
	void init(vertex_descriptorN src);
	void updateDist(vertex_descriptorN z);
	
	void ExtractClosestZ(PathVector &newPath, int&); 
	void AddTreeSPH(PathVector &newPath); 
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
};

#endif // GRAPHALGO_MINIMUM_DIAMETER_SHORTEST_PATH_HEURISTIC_KHV


