/***************************************************************************
                          dcsph.h  -  description
                             -------------------
    begin                : Thu Mar 23 200
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"

using namespace boost;
using namespace TreeAlgorithms;

struct DCSPH {
	ShortestPathKeeper spkCost;
	ShortestPathKeeper spkDistance;
	int zid;
};

class DCShortestPathHeuristic : SteinerHeuristic
{
public:
	DCShortestPathHeuristic(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_dcsph_(out) {}
	~DCShortestPathHeuristic() { }  
	
	// typedefs definitions
	enum COLOR { WHITE = 0, GRAY, BLACK};
	typedef vector<bool>				BoolVector;
	typedef vector<COLOR>				ColorVector;
	//typedef std::vector<DCSPH>		DCSPHVector;
	typedef std::map<int, DCSPH> 		DCSPHMap;
	
	typedef std::pair<std::vector<int>, int>  PathVector;
	
	void Algorithm(vertex_descriptorN zsource, double delayLimit);
	
	// class functions
	void ExtractClosestZCost(PathVector &newPath, vertex_descriptorN src);
	void ExtractClosestZDist(PathVector &newPath);
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	void AddTree(PathVector &newPath, int &zAdded);
		
	// dijkstra least cost, shortest path
	void DijkstraD(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost);
	void DijkstraC(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost);
	void RunDijkstraCDForEveryZ();
	void findDCSPH(vertex_descriptorN id, DCSPH &dcsph);
	DCSPH& findDCSPH(vertex_descriptorN id);

private:
	TreeStructure	&T_dcsph_;			// Stores the SPH tree 
	DCSPHMap		dcspMap_;
	double 			delayLimit_;
};

#endif // GRAPHALGO_DELAY_CONSTRAINED_SHORTEST_PATH_HEURISTIC_KHV

