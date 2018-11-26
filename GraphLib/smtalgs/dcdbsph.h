/***************************************************************************
                          dcdbsph.h  -  description
                             -------------------
    begin                : Tue Jan 16 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_HEURISTIC_KHV
#define GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_HEURISTIC_KHV

#include "steiner_class.h"
#include "../treealgs/treestructure.h"
#include "dcsph.h"

using namespace boost;
using namespace TreeAlgorithms;

// defined in dcsph.h
//struct DCSPH {
//	ShortestPathKeeper spkCost;
//	ShortestPathKeeper spkDistance;
//	int zid;
//};

class DCDBSPH : SteinerHeuristic
{
public:
	DCDBSPH(const TreeStructure &in, TreeStructure &out) : SteinerHeuristic(in), T_dcsph_(out), g(in.g) {}
	~DCDBSPH() { }  
	
	// typedefs definitions
	enum COLOR { WHITE = 0, GRAY, BLACK};
	typedef vector<bool>				BoolVector;
	typedef vector<COLOR>				ColorVector;
	//typedef std::vector<DCSPH>		DCSPHVector;
	typedef std::map<int, DCSPH> 		DCSPHMap;
	
	typedef std::pair<std::vector<int>, int>  PathVector;
	
	void Algorithm(vertex_descriptorN zsource, double delayLimit);
	
	// class functions
	bool ExtractClosestZCost(PathVector &newPath, vertex_descriptorN src);
	bool ExtractClosestZDist(PathVector &newPath);
	void FindPath(PathVector &newPath, vertex_descriptorN tree_vert, vertex_descriptorN z_vert, const ParentVector &zparent);
	bool AddTree(PathVector &newPath, int &zAdded);
		
	// dijkstra least cost, shortest path
	void DijkstraD(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost);
	void DijkstraC(vertex_descriptorN src, ParentVector &zparent, DistanceVector &zdistance, DistanceVector &zcost);
	void findDCSPH(vertex_descriptorN id, DCSPH &dcsph);
	
	void RunDijkstraCDForEveryZ(const GraphN &g);
	void RunDijkstraCForEveryZ(const GraphN &g);
	void RunDijkstraDForEveryZ(const GraphN &g);

	bool removeDCNodes();

private:
	TreeStructure	&T_dcsph_;			// Stores the SPH tree 
	DCSPHMap		dcspMap_;
	double 			delayLimit_;
	GraphN			g;
};

#endif // GRAPHALGO_DELAY_CONSTRAINED_DEGREE_BOUNDED_SHORTEST_PATH_HEURISTIC_KHV 


