/***************************************************************************
                          adh.h  -  description
                             -------------------
    begin                : Thu Oct 6 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#ifndef GRAPHALGO_AVERAGE_DISTANCE_HEURISTIC_KHV
#define GRAPHALGO_AVERAGE_DISTANCE_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treealgs.h"
#include "../boostprop.h"

using namespace TreeAlgorithms;

class AverageDistanceHeuristic : public SteinerHeuristic
{
private:
	TreeStructure		&T_adh;
	const GraphN		&g;
	TreeStructure		inTree;
	VertexSet			steinerSet;

	std::map<int, std::map<int, double> > 						fw_matrix; 		// Floyd-Warshall matrix
	std::map<int, std::map<int, int > >							parent_matrix;	// Parent matrix from Floyd-Warshall
	std::map<int, std::multimap<double, TreeStructure*> >		dnt_matrix;		// d(n,t) matrix
	std::map<int, TreeStructure*>								vertTOtree_map; // faster tree access
	std::vector<TreeStructure*>									TreeBundle_adh;	// used when cleaning up
	
public:
	AverageDistanceHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_adh(out) , g(in.g), inTree(in) {}
	~AverageDistanceHeuristic() { }

	// Average Distance Heuristic main function
	void Algorithm(int zsource);

	// Average Distance Heuristic helper function
	void RunFloydWarshall();
	void FindMNode(int &fm_vert, int &fm_r);
	void AddClosestZVertices(int fm_vert, int fm_r);
	void AddEdges(int fm_vert, TreeStructure* pTadh_fm, TreeStructure* pTadh_nt);

	double RaywardSmithf(int &r, double fn, int v, multimap<double, TreeStructure*>::iterator vit, multimap<double,TreeStructure*>::iterator vit_end);
	// f(n) = min(1<=r<=k){ (sum of i=0 to r) d(n,t_i)/r:t_0,t_1,...,t_r in T}
	
	// dump functions
	void dumpParentMatrix();
	void dumpFWMatrix();
	void dumpDNTMatrix();
	void dumpVertTOtreeMap();
	void dumpAll();
	
	// clean up
	void cleanup();

	//floyd_warshall_all_pairs_shortest_paths(g, fw_matrix, get(&EdgeProp::weight, g), compare, combine, std::numeric_limits<double>::max(), (double)0); 
	//floyd_warshall_all_pairs_shortest_paths(g, fw_matrix, weight_map(get(&EdgeProp::weight, g))); 
	//std::less<double> compare;		// TODO: (unused) used when calling floyd-warshall as default
	//std::plus<double> combine;		// TODO: (unused) used when calling floyd-warshall as default
};


#endif /* GRAPHALGO_AVERAGE_DISTANCE_HEURISTIC_KHV */

