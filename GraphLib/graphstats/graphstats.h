/***************************************************************************
                          graphstats.h  -  description
                             -------------------
    begin                : Wed Nov 30 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef GRAPHALGO_GRAPH_STATS_KHV
#define GRAPHALGO_GRAPH_STATS_KHV

#include <vector>
#include "../boostprop.h"
#include "../treealgs/treestructure.h"
#include "../treealgs/floyd_warshall_apsp.h"

using namespace TreeAlgorithms;
using namespace std;

namespace Statistics
{
typedef std::list<int> PathList;

/* -------------------------------------------------------------------------------------
							struct StatValues
----------------------------------------------------------------------------------------*/
struct StatValues
{
public:
	StatValues(const double &wd, const double &wc, const int &hc) : worstcase_dist(wd), worstcase_cost(wc), hopcount(hc), path_list(NULL) {}
	StatValues(const double &wd, const double &wc, const int &hc, PathList *p) : worstcase_dist(wd), worstcase_cost(wc), hopcount(hc), path_list(p) {}
	StatValues(PathList *p) : worstcase_dist(0), worstcase_cost(0), hopcount(0), path_list(p) {}
	StatValues() : worstcase_dist(0), worstcase_cost(0), hopcount(0), path_list(NULL) {}
	~StatValues() {}
	
	double worstcase_dist;
	double worstcase_cost;
	int hopcount;		

	PathList *path_list;
};

/* -------------------------------------------------------------------------------------
							class GraphStatistics 
----------------------------------------------------------------------------------------*/
class GraphStatistics
{
public:
	
	GraphStatistics(TreeStructure &Tin, GraphN &global) 
			: T(Tin), 
			g(Tin.g), 
			globalG(global), 
			medMemberCostSize(0), 
			medMemberDistSize(0), 
			avmembernode_dist(0), 
			avmembernode_cost(0),
			medCoreMemberCostSize(0), 
			medCoreMemberDistSize(0), 
			core_avmembernode_dist(0), 
			core_avmembernode_cost(0),
			max_outdegree_(0),
			min_edge_dist_(0),
			min_edge_cost_(0),
			sum_edge_dist_(0),
			sum_edge_cost_(0)
			//distance_(num_vertices(global), num_vertices(global)),
			//distance_hops_(num_vertices(global), num_vertices(global)),
			//distance_parent_(num_vertices(global), num_vertices(global)),
			//hop_distance_(num_vertices(global), num_vertices(global)),
			//hops_(num_vertices(global), num_vertices(global)),
			//hop_parent_(num_vertices(global), num_vertices(global))
			{
				cout << "GraphStatistics(): WARNING! TODO! Initialize vector<DistanceVector> DistanceVectorMatrix!" << endl;
			}
	~GraphStatistics() { }
	
	// functions
	bool TreeStats(ostream &filename);		
	bool TreeStatsAll(ostream &filename);	
	void coreStatistics(ostream &filename);
	
	// find pair-wise latencies, diameter ..
	void SearchWorstCasePairAll(int src, int prev, int curr, bool core_node, bool member_node, StatValues &SV);

	// mesh functions
	void meshStatistics(ostream &filename);
	void meshStatisticsFloydWarshall(ostream &outfile);
	void printAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path);
	void statsAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path, StatValues &SV);

	// stats functions
	bool graphNodeDegreeStats(ostream &outfile);
	bool graphEdgeStats(ostream &outfile);

	// inlines
	inline void fillArray(const vector<double> &vec, double arr[])
	{
		int i = 0;
		for(vector<double>::const_iterator vit = vec.begin(), vit_end = vec.end(); vit != vit_end; ++vit, i++) arr[i] = *vit;		
	}

	inline void init(const TreeStructure &Tin, const GraphN &global)
	{
		T = Tin;
		g = Tin.g;
		globalG = global;		

		medMemberCostSize= 0; 
		medMemberDistSize= 0; 
		avmembernode_dist= 0; 
		avmembernode_cost= 0;
		medCoreMemberCostSize= 0; 
		medCoreMemberDistSize= 0; 
		core_avmembernode_dist= 0; 
		core_avmembernode_cost= 0;
		max_outdegree_= 0;
		min_edge_dist_= 0;
		min_edge_cost_= 0;
		sum_edge_dist_= 0;
		sum_edge_cost_ = 0;
		
		/*vecPWMemberCost.clear();
		vecPWMemberDist.clear();
		vecPWMemberHops.clear();

		vecCorePWMemberCost.clear();
		vecCorePWMemberDist.clear();
		vecCorePWMemberHops.clear();
		*/
		//distance_.clear();
		//hops_.clear();
		//parent_.clear();
		//hop_distance_.clear();
	}
	
private:
	TreeStructure &T;
	GraphN &g;
	GraphN &globalG;
	
private:
	int medMemberCostSize, medMemberDistSize;
	double avmembernode_dist, avmembernode_cost;
	std::vector<double> vecPWMemberCost, vecPWMemberDist, vecPWMemberHops;

	// core stats
	int medCoreMemberCostSize, medCoreMemberDistSize;
	std::vector<double> vecCorePWMemberCost, vecCorePWMemberDist, vecCorePWMemberHops;
	double core_avmembernode_dist, core_avmembernode_cost;

	PathList diameterPath_;
	int max_outdegree_;
	double min_edge_dist_, min_edge_cost_, sum_edge_dist_, sum_edge_cost_;

private:
	typedef vector<DistanceVector> DistanceVectorMatrix;
	typedef vector<ParentVector> ParentVectorMatrix;
	
	DistanceVectorMatrix 	distance_;
	DistanceVectorMatrix 	distance_hops_;
	ParentVectorMatrix 		distance_parent_;

	DistanceVectorMatrix 	hop_distance_;
	DistanceVectorMatrix 	hops_;
	ParentVectorMatrix 		hop_parent_;

	void maxDistance(int v, DistanceVector &distance, DistanceVector &hops, ParentVector &parent);
	double maxDistanceHops(int v, DistanceVector &distance);
	void maxDistanceHops(int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent);

	double diameter();
	void distanceMatrixHops(DistanceVectorMatrix &D);
	void distanceMatrixHops(DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent);
	void distanceMatrix(DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent);
};

}; // namespace Statistics

#endif // GRAPHALGO_GRAPH_STATS_KHV


