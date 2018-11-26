/***************************************************************************
                          graphstats.cc  -  description
                             -------------------
    begin                : Wed Nov 30 2005
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "graphstats.h"
//#include "../graphviz/graphviz.h"
#include "recordtree.h"
#include "../treealgs/floyd_warshall_apsp.h"
#include "../treealgs/fheap.h"

using namespace std;
using namespace TreeAlgorithms;

//extern "C" {
//	int (*compfn)(const void* left, const void* right);
//}

int MyDouble_compare_qsort(const void* a, const void* b)
{
	return ( ((*(double*) a) > (*(double*) b)) - ((*(double*) a) < (*(double*) b)) );
}

int MyDouble_compare(const double &a, const double &b)
{
    return a < b; //(a > b) - (a < b);
}

namespace Statistics
{

/* -------------------------------------------------------------------------------------
							TreeStats
----------------------------------------------------------------------------------------*/
bool
GraphStatistics::TreeStats(ostream &filename)
{
	return TreeStatsAll(filename);
}

bool GraphStatistics::graphNodeDegreeStats(ostream &outfile)
{
	// variables 
	int max_outdegree = 0, max_outdegree_gm = 0, sum_outdegree = 0;
	int sum_outdegree_woleafs = 0, sum_outdegree_woleafs_sp = 0, sum_outdegree_woleafs_gm = 0;	
	VertexSet leafSet, leafSetGM, leafSetSP;
	
	VertexSet errorV; // error handling 
	for(VertexSet::const_iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		int degree = out_degree(v, g);
		if(degree > 1)
		{
			sum_outdegree_woleafs = sum_outdegree_woleafs + degree; 			// summarize out degree excluding leaf-nodes
			if(g[v].vertexState == STEINER_POINT) sum_outdegree_woleafs_sp = sum_outdegree_woleafs_sp + degree;
			else if(g[v].vertexState == GROUP_MEMBER) sum_outdegree_woleafs_gm = sum_outdegree_woleafs_gm + degree;
			else cerr << WRITE_FUNCTION << " vertex: " << v << " has no valid state! : " << g[v] << endl;
		}
		else if(degree == 1) 
		{
			leafSet.insert(v);
			if(g[v].vertexState == STEINER_POINT) leafSetSP.insert(v);
			else if(g[v].vertexState == GROUP_MEMBER) leafSetGM.insert(v);
			else cerr << WRITE_FUNCTION << " vertex: " << v << " has no valid state! : " << g[v] << endl;
		}
		else // -- error handling --
		{
			//cerr << WRITE_FUNCTION << " vertex: " << v << " out degree == 0" << endl;
			errorV.insert(v);
		} // -- error handling end --
		
		sum_outdegree += degree; 		
		if(degree > max_outdegree) max_outdegree = degree;	
		if(g[v].vertexState == GROUP_MEMBER && degree > max_outdegree_gm) max_outdegree_gm = degree;
	}
	
	// -- error handling --
	if(errorV.size()) 
	{
		//cerr << ";" ; 
		cerr << WRITE_FUNCTION << " vertices " << errorV << " out degree == 0" << endl;
		return false;
	}
	// -- error handling end --
	
	//ASSERTING(leafSet.size() > 1);
	
	// variables
	int leafnode_cnt_sp = leafSetSP.size();
	int leafnode_cnt = leafSet.size();
	int leafnode_cnt_gm = leafSetGM.size(); 

	// store maximum out degree
	max_outdegree_ = max_outdegree;
	
	// stats
	double average_out_degree_wo_leafs = 0;
	double average_out_degree_wo_leafs_sp = 0;
	double average_out_degree_wo_leafs_gm = 0;
	if(T.Ep.size() > 1) 
	{
		average_out_degree_wo_leafs = (double) sum_outdegree_woleafs/(T.V.size() - leafnode_cnt);   
		average_out_degree_wo_leafs_sp = (double) sum_outdegree_woleafs_sp/std::max(1, (int) (T.S.size() - leafnode_cnt_sp)); 
		average_out_degree_wo_leafs_gm = (double) sum_outdegree_woleafs_gm/std::max(1, (int) ((T.V.size() - T.S.size()) - leafnode_cnt_gm));
	}

	outfile << " -L " 			<< leafnode_cnt;

	outfile << " -ODmax " 		<< max_outdegree;
	outfile << " -ODmaxgm "     << max_outdegree_gm;
	outfile << " -ODavewol "	<< average_out_degree_wo_leafs; 
	outfile << " -ODavewolsp "  << average_out_degree_wo_leafs_sp;
	outfile << " -ODavewolgm "  << average_out_degree_wo_leafs_gm;
	outfile << " -ODave "		<< (double)(sum_outdegree/(double)(T.V.size()));

	return true;
}

bool GraphStatistics::graphEdgeStats(ostream &outfile)
{
	// weight and cost maps
	dEdgeMap wmap = get(&EdgeProp::weight, g);
	dEdgeMap cmap = get(&EdgeProp::cost, g);
	
   	// variables 
	double tree_dist = 0, max_edge_dist = 0, min_edge_dist = (numeric_limits<double>::max)(), sum_edge_dist = 0;
	double tree_cost = 0, max_edge_cost = 0, min_edge_cost = (numeric_limits<double>::max)(), sum_edge_cost = 0;
    //double medEdgeCost[T.Ep.size()], medEdgeDist[T.Ep.size()];
    std::vector<double> medEdgeCost(T.Ep.size()), medEdgeDist(T.Ep.size());

	int i = 0;
	for(EdgePair::const_iterator seit = T.Ep.begin(), seit_end = T.Ep.end(); seit != seit_end; ++seit, i++)
	{
		pair<edge_descriptorN, bool> e = edge(seit->first, seit->second, g);
		assert(e.second);
	
		ASSERTING(wmap[e.first] > 0);

		// weight
		tree_dist = tree_dist + wmap[e.first];	
		if(wmap[e.first] > max_edge_dist) max_edge_dist = wmap[e.first];
		if(wmap[e.first] < min_edge_dist) min_edge_dist = wmap[e.first];
		sum_edge_dist += wmap[e.first];
		medEdgeDist[i] = wmap[e.first];
		
		// cost
		tree_cost = tree_cost + cmap[e.first];	
		if(cmap[e.first] > max_edge_cost) max_edge_cost = cmap[e.first];
		if(cmap[e.first] < min_edge_cost) min_edge_cost = cmap[e.first];
		sum_edge_cost += cmap[e.first];
		medEdgeCost[i] = cmap[e.first];
	}
	
	min_edge_dist_ = min_edge_dist;
	min_edge_cost_ = min_edge_cost;
	sum_edge_dist_ = sum_edge_dist;
	sum_edge_cost_ = sum_edge_cost;

	// find median edge distance
    //qsort(&medEdgeDist[0], T.Ep.size(), sizeof(double), MyDouble_compare);
    std::sort(medEdgeDist.begin(), medEdgeDist.end(), MyDouble_compare);
    double final_median_edgedist = findMedian(medEdgeDist);
	ASSERTING(final_median_edgedist > 0);

	// find median edge cost
    //qsort(&medEdgeCost[0], T.Ep.size(), sizeof(double), MyDouble_compare);
    std::sort(medEdgeCost.begin(), medEdgeCost.end(), MyDouble_compare);
    double final_median_edgecost = findMedian(medEdgeCost);
	ASSERTING(final_median_edgecost > 0);

	outfile << " -Cdis " 		<< tree_dist;
	outfile << " -Emaxdis "		<< max_edge_dist;
	outfile << " -Emindis " 	<< min_edge_dist;
	outfile << " -Eavedis " 	<< sum_edge_dist/T.Ep.size();
	outfile << " -Emeddis "		<< final_median_edgedist;


	outfile << " -Ccos " 		<< tree_cost;
	outfile << " -Emaxcos "		<< max_edge_cost;
	outfile << " -Emincos " 	<< min_edge_cost;
	outfile << " -Eavecos " 	<< sum_edge_cost/T.Ep.size();
	outfile << " -Emedcos "		<< final_median_edgecost;

	return true;
}

/* -------------------------------------------------------------------------------------
							TreeStatsAll
		TODO: for meadian results add 5, 15, median, 75, 95 percentile results.
----------------------------------------------------------------------------------------*/
bool
GraphStatistics::TreeStatsAll(ostream &outfile)
{
	// print stats to file
	outfile << " -V "			<< T.V.size();
	outfile << " -E "			<< T.Ep.size();
	outfile << " -Z "			<< T.V.size() - T.S.size();
	outfile << " -S "			<< T.S.size(); 

	graphEdgeStats(outfile);
	graphNodeDegreeStats(outfile);

	// member nodes
	int membernode_cnt = (T.V - T.S).size();
    //double medMemberCost[membernode_cnt], medMemberDist[membernode_cnt], medMemberHops[membernode_cnt];
    std::vector<double> medMemberCost(membernode_cnt), medMemberDist(membernode_cnt), medMemberHops(membernode_cnt);
    double worstcase_dist = 0, temp_medianmembernode_dist = 0;
	double worstcase_cost = 0, temp_medianmembernode_cost = 0; 
	double max_hopcount = 0, sum_hopcount = 0;	

	// core nodes
	int corenode_cnt = T.C.size();
    //double medCoreMemberCost[corenode_cnt], medCoreMemberDist[corenode_cnt], medCoreMemberHops[corenode_cnt];
    std::vector<double> medCoreMemberCost(corenode_cnt), medCoreMemberDist(corenode_cnt), medCoreMemberHops(corenode_cnt);
    double core_worstcase_dist = 0, temp_core_medianmembernode_dist = 0;
	double core_worstcase_cost = 0, temp_core_medianmembernode_cost = 0; 
	double core_max_hopcount = 0, core_sum_hopcount = 0;	

	// Relative Delay Penalty
	double relative_delay_penalty_max = 0, temp_shortest_path_dist = 0, temp_ecc_path_dist = 0;
	double core_relative_delay_penalty_max = 0, temp_core_shortest_path_dist = 0, temp_core_ecc_path_dist = 0;

	VertexSet memberSet = T.V; // - T.S;
	int member_count = 0, core_count = 0, i = 0;
	// worst case pair wise cost 
	for(VertexSet::const_iterator vit = memberSet.begin(), vit_end = memberSet.end(); vit != vit_end; ++vit, i++) 
	{
		int v = *vit;

		bool core_node = false;
		if(T.C.contains(v)) core_node = true;

		bool member_node = true;
		if(T.S.contains(v)) member_node = false;
		
		// steiner point?
		if(member_node == false && core_node == false) continue;
		
		diameterPath_.clear();
		StatValues SV(&diameterPath_);
	    SearchWorstCasePairAll(v, v, v, core_node, member_node, SV);

		if(core_node == true) // core set
		{
			// record core set stats
			// RDP
			int max_ecc_node = *(SV.path_list->rbegin());
			pair<edge_descriptorN, bool> ep = edge(v, max_ecc_node, globalG);
			ASSERTING(ep.second);
			temp_core_shortest_path_dist += globalG[ep.first].weight;
			temp_core_ecc_path_dist += SV.worstcase_dist;
			if(core_relative_delay_penalty_max < (SV.worstcase_dist/globalG[ep.first].weight)) 
				core_relative_delay_penalty_max = (SV.worstcase_dist/globalG[ep.first].weight);

			// cost
			if(SV.worstcase_cost > core_worstcase_cost) core_worstcase_cost = SV.worstcase_cost;
			double median_cost = (core_avmembernode_cost - temp_core_medianmembernode_cost)/(corenode_cnt - 1);
			if(median_cost > 0) medCoreMemberCost[medCoreMemberCostSize++] = median_cost;
			temp_core_medianmembernode_cost = core_avmembernode_cost;

			// distance
			if(SV.worstcase_dist > core_worstcase_dist) core_worstcase_dist = SV.worstcase_dist;
			double median_dist = (core_avmembernode_dist - temp_core_medianmembernode_dist)/(corenode_cnt - 1);
			if(median_dist > 0) medCoreMemberDist[medCoreMemberDistSize++] = median_dist;
			temp_core_medianmembernode_dist = core_avmembernode_dist;
				
			// hops
			if(core_max_hopcount < SV.hopcount)	core_max_hopcount = SV.hopcount;
			core_sum_hopcount += SV.hopcount;
			medCoreMemberHops[core_count++] = (double) SV.hopcount;
		}

		if(member_node == false) continue;
		
		// RDP
		int max_ecc_node = *(SV.path_list->rbegin());
		pair<edge_descriptorN, bool> ep = edge(v, max_ecc_node, globalG);
		ASSERTING(ep.second);
		temp_shortest_path_dist += globalG[ep.first].weight;
		temp_ecc_path_dist += SV.worstcase_dist;
		if(relative_delay_penalty_max < (SV.worstcase_dist/globalG[ep.first].weight)) 
			relative_delay_penalty_max = (SV.worstcase_dist/globalG[ep.first].weight);

		// cost
		if(SV.worstcase_cost > worstcase_cost) worstcase_cost = SV.worstcase_cost;
		double median_cost = (avmembernode_cost - temp_medianmembernode_cost)/(membernode_cnt - 1);
		if(median_cost > 0) medMemberCost[medMemberCostSize++] = median_cost;
		temp_medianmembernode_cost = avmembernode_cost;

		//cerr << v << ": avmembernode_dist " << avmembernode_dist << " median " << (avmembernode_dist - temp_medianmembernode_dist)/membernode_cnt;
		//cerr << "median_cost "	<< median_cost << endl;

		// distance
		if(SV.worstcase_dist > worstcase_dist) worstcase_dist = SV.worstcase_dist;
		double median_dist = (avmembernode_dist - temp_medianmembernode_dist)/(membernode_cnt - 1);
		if(median_dist > 0) medMemberDist[medMemberDistSize++] = median_dist;
		temp_medianmembernode_dist = avmembernode_dist;
			
		// hops
		if(max_hopcount < SV.hopcount)	max_hopcount = SV.hopcount;
		sum_hopcount += SV.hopcount;
		medMemberHops[member_count++] = (double) SV.hopcount;

		//cerr << "SV.hc " << SV.hopcount << " SV.worstcase_dist " << SV.worstcase_dist << " SV.worstcase_cost " << SV.worstcase_cost << endl;
	}	

	// ----- Calculate Distance Sets -------	
	// find median distance
	double final_median_dist = 0;
	if(T.Ep.size() == 1) 
		final_median_dist = min_edge_dist_; 		// only one edge
	else if(max_outdegree_ == 2) 			 	// one long line
		final_median_dist = worstcase_dist;
	else
	{
        //qsort((void*) medMemberDist, medMemberDistSize, sizeof(double), MyDouble_compare);
        std::sort(medMemberDist.begin(), medMemberDist.begin() + medMemberDistSize, MyDouble_compare);
		final_median_dist = findMedian(medMemberDist, medMemberDistSize);
	}
	ASSERTING(final_median_dist > 0);

	// find median pairwise distance
	double final_medianpw_dist = 0;
	if(T.Ep.size() == 1)
		final_medianpw_dist = min_edge_dist_; 	// only one edge
	else if(max_outdegree_ == 2) 				// one long line
		final_medianpw_dist = worstcase_dist;
	else
	{
        std::vector<double> medPWMemberDist(vecPWMemberDist);
        //fillArray(vecPWMemberDist, medPWMemberDist);
        //qsort((void*) medPWMemberDist, vecPWMemberDist.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberDist.begin(), medPWMemberDist.end(), MyDouble_compare);
		final_medianpw_dist = findMedian(medPWMemberDist, vecPWMemberDist.size());
	}
	ASSERTING(final_medianpw_dist > 0);
	
	// find median hops
    //qsort((void*) medMemberHops, membernode_cnt, sizeof(double), MyDouble_compare);
    std::sort(medMemberHops.begin(), medMemberHops.begin() + membernode_cnt, MyDouble_compare);
	double final_median_hops = findMedian(medMemberHops, membernode_cnt); 
	ASSERTING(final_median_hops > 0);

	// find median pairwise hop
	double final_medianpw_hops = 0;
	if(vecPWMemberHops.size() == 0)
	{
		ASSERTING(T.Ep.size() == 1 || max_outdegree_ == 2);
		final_medianpw_hops = final_median_hops;
	}
	else	
	{
        std::vector<double> medPWMemberHops(vecPWMemberHops);
        //fillArray(vecPWMemberHops, medPWMemberHops);
        //qsort((void*) medPWMemberHops, vecPWMemberHops.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberHops.begin(), medPWMemberHops.end(), MyDouble_compare);
        final_medianpw_hops = findMedian(medPWMemberHops); //, medPWMemberHops.size());
	}
	ASSERTING(final_medianpw_hops > 0);	
	
	double averagePWDistance = 0;
	if(T.Ep.size() == 1)
		averagePWDistance = (double)(min_edge_dist_); 
	else if(max_outdegree_ == 2)
		averagePWDistance = worstcase_dist;
	else
		averagePWDistance = (double)(avmembernode_dist/(medMemberDistSize*(membernode_cnt - 1))); 
	if(averagePWDistance == 0)
		averagePWDistance = sum_edge_dist_/T.Ep.size();
	ASSERTING(averagePWDistance > 0);	

	// ----- Calculate Cost Sets -------		
	// find median cost
	double final_median_cost = 0;
	if(T.Ep.size() == 1) 
		final_median_cost = min_edge_cost_; 	// only one edge
	else if(max_outdegree_ == 2) 			 	// one long line
		final_median_cost = worstcase_cost;
	else
	{
        //qsort((void*) medMemberCost, medMemberCostSize, sizeof(double), MyDouble_compare);
        std::sort(medMemberCost.begin(), medMemberCost.end() + medMemberCostSize, MyDouble_compare);
		final_median_cost = findMedian(medMemberCost, medMemberCostSize); 
	}
	ASSERTING(final_median_cost > 0);

	// find median pairwise cost
	double final_medianpw_cost = 0;
	if(T.Ep.size() == 1)
		final_medianpw_cost = min_edge_cost_; // only one edge
	else if(max_outdegree_ == 2) 				 // one long line
		final_medianpw_cost = worstcase_cost;
	else
	{
        std::vector<double> medPWMemberCost(vecPWMemberCost);
        //fillArray(vecPWMemberCost, medPWMemberCost);
        //qsort((void*) medPWMemberCost, vecPWMemberCost.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberCost.begin(), medPWMemberCost.end(), MyDouble_compare);
		final_medianpw_cost = findMedian(medPWMemberCost, vecPWMemberCost.size()); 
	}
	ASSERTING(final_medianpw_cost > 0);

	double averagePWCost = 0;
	if(T.Ep.size() == 1)
		averagePWCost = min_edge_cost_; 
	else if(max_outdegree_ == 2)
		averagePWCost = worstcase_cost;
	else
	   averagePWCost = (avmembernode_cost/(medMemberCostSize* (membernode_cnt - 1)));
	if(averagePWCost == 0)
		averagePWCost = sum_edge_cost_/T.Ep.size();
	ASSERTING(averagePWCost > 0);
	
	// Stats
	//outfile << " -V "			<< T.V.size();
	//outfile << " -E "			<< T.Ep.size();
	//outfile << " -Z "			<< T.V.size() - T.S.size();
	//outfile << " -S "			<< T.S.size(); 
	//outfile << " -L " 			<< leafnode_cnt;
	//outfile << " -Cdis " 		<< tree_dist;
	//outfile << " -Emaxdis "		<< max_edge_dist;
	//outfile << " -Emindis " 	<< min_edge_dist;
	//outfile << " -Eavedis " 	<< sum_edge_dist/T.Ep.size();
	//outfile << " -Emeddis "		<< final_median_edgedist;
	outfile << " -PWmaxdis "	<< worstcase_dist;
	outfile << " -PWmaxavedis "	<< averagePWDistance;
	outfile << " -PWmaxmeddis " << final_median_dist;
	outfile << " -PWmeddis " 	<< final_medianpw_dist;
	outfile << " -PWmaxhoc " 	<< max_hopcount;
	outfile << " -PWavehoc " 	<< (double)(sum_hopcount/(membernode_cnt));
	outfile << " -PWmaxmedhoc " << final_median_hops;
	outfile << " -PWmedhoc " 	<< final_medianpw_hops;
	//outfile << " -ODmax " 		<< max_outdegree_;
	//outfile << " -ODmaxgm "     << max_outdegree_gm;
	//outfile << " -ODavewol "	<< average_out_degree_wo_leafs; 
	//outfile << " -ODavewolsp "  << average_out_degree_wo_leafs_sp;
	//outfile << " -ODavewolgm "  << average_out_degree_wo_leafs_gm;
	//outfile << " -ODave "		<< (double)(sum_outdegree/(double)(T.V.size()));
	//outfile << " -Ccos " 		<< tree_cost;
	//outfile << " -Emaxcos "		<< max_edge_cost;
	//outfile << " -Emincos " 	<< min_edge_cost;
	//outfile << " -Eavecos " 	<< sum_edge_cost/T.Ep.size();
	//outfile << " -Emedcos "		<< final_median_edgecost;
	outfile << " -PWmaxcos "	<< worstcase_cost;
	outfile << " -PWmaxavecos "	<< averagePWCost;
	outfile << " -PWmaxmedcos " << final_median_cost;
	outfile << " -PWmedcos " 	<< final_medianpw_cost;
	outfile << " -RDPmax " 		<< relative_delay_penalty_max;
	outfile << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;
	
	if(T.C.empty()) return true;

	// ---------- add core statistics ------------------
	// ----- Calculate Distance Sets -------	
	// find median distance
	final_median_dist = 0;
	if(T.Ep.size() == 1) 
		final_median_dist = min_edge_dist_; 		// only one edge
	else if(max_outdegree_ == 2) 			 	// one long line
		final_median_dist = core_worstcase_dist;
	else
	{
        //qsort((void*) medCoreMemberDist, medCoreMemberDistSize, sizeof(double), MyDouble_compare);
        std::sort(medCoreMemberDist.begin(), medCoreMemberDist.end(), MyDouble_compare);
        final_median_dist = findMedian(medCoreMemberDist, medCoreMemberDistSize);
	}
	//ASSERTING(final_median_dist > 0);

	// find median pairwise distance
	final_medianpw_dist = 0;
	if(T.Ep.size() == 1)
		final_medianpw_dist = min_edge_dist_; 	// only one edge
	else if(max_outdegree_ == 2) 				// one long line
		final_medianpw_dist = core_worstcase_dist;
	else
	{
        std::vector<double> medPWMemberDist(vecCorePWMemberDist);
        //fillArray(vecCorePWMemberDist, medPWMemberDist);
        //qsort((void*) medPWMemberDist, vecCorePWMemberDist.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberDist.begin(), medPWMemberDist.end(), MyDouble_compare);
        final_medianpw_dist = findMedian(medPWMemberDist, vecCorePWMemberDist.size());
	}
	//ASSERTING(final_medianpw_dist > 0);
	
	// find median hops
    //qsort((void*) medCoreMemberHops, corenode_cnt, sizeof(double), MyDouble_compare);
    std::sort(medCoreMemberHops.begin(), medCoreMemberHops.begin() + corenode_cnt, MyDouble_compare);
	final_median_hops = findMedian(medCoreMemberHops, corenode_cnt); 
	//ASSERTING(final_median_hops > 0);

	// find median pairwise hop
	final_medianpw_hops = 0;
	if(vecCorePWMemberHops.size() == 0)
	{
		//ASSERTING(T.Ep.size() == 1 || max_outdegree_ == 2);
		final_medianpw_hops = final_median_hops;
	}
	else	
	{
        std::vector<double> medPWMemberHops(vecCorePWMemberHops);
        //fillArray(vecCorePWMemberHops, medPWMemberHops);
        //qsort((void*) medPWMemberHops, vecCorePWMemberHops.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberHops.begin(), medPWMemberHops.end(), MyDouble_compare);
        final_medianpw_hops = findMedian(medPWMemberHops, medPWMemberHops.size());
	}
	//ASSERTING(final_medianpw_hops > 0);	
	
	averagePWDistance = 0;
	if(corenode_cnt == 1)
		averagePWDistance = (double)(min_edge_dist_); 
	else if(max_outdegree_ == 2)
		averagePWDistance = core_worstcase_dist;
	else
		averagePWDistance = (double)(core_avmembernode_dist/(medCoreMemberDistSize*(corenode_cnt - 1))); 
	if(averagePWDistance == 0)
		averagePWDistance = sum_edge_dist_/T.Ep.size();
	//ASSERTING(averagePWDistance > 0);	

	// ----- Calculate Cost Sets -------		
	// find median cost
	final_median_cost = 0;
	if(T.Ep.size() == 1) 
		final_median_cost = min_edge_cost_; 	// only one edge
	else if(max_outdegree_ == 2) 			 	// one long line
		final_median_cost = core_worstcase_cost;
	else
	{
        //qsort((void*) medCoreMemberCost, medCoreMemberCostSize, sizeof(double), MyDouble_compare);
        std::sort(medCoreMemberCost.begin(), medCoreMemberCost.begin() + medCoreMemberCostSize, MyDouble_compare);
		final_median_cost = findMedian(medCoreMemberCost, medCoreMemberCostSize); 
	}
	//ASSERTING(final_median_cost > 0);

	// find median pairwise cost
	final_medianpw_cost = 0;
	if(T.Ep.size() == 1)
		final_medianpw_cost = min_edge_cost_; // only one edge
	else if(max_outdegree_ == 2) 				 // one long line
		final_medianpw_cost = core_worstcase_cost;
	else
	{
        std::vector<double> medPWMemberCost(vecCorePWMemberCost);
        //fillArray(vecCorePWMemberCost, medPWMemberCost);
        //qsort((void*) medPWMemberCost, vecCorePWMemberCost.size(), sizeof(double), MyDouble_compare);
        std::sort(medPWMemberCost.begin(), medPWMemberCost.end(), MyDouble_compare);
        final_medianpw_cost = findMedian(medPWMemberCost, medPWMemberCost.size());
	}
	//ASSERTING(final_medianpw_cost > 0);
	
	averagePWCost = 0;
	if(corenode_cnt == 1)
		averagePWCost = min_edge_cost_; 
	else if(max_outdegree_ == 2)
		averagePWCost = core_worstcase_cost;
	else
	   averagePWCost = (avmembernode_cost/(medCoreMemberCostSize* (corenode_cnt - 1)));
	if(averagePWCost == 0)
		averagePWCost = sum_edge_cost_/T.Ep.size();
	//ASSERTING(averagePWCost > 0);
	
	// -- core node stats --
	outfile << " -Core "			<< T.C.size() ;
	outfile << " -CPWmaxdis "		<< core_worstcase_dist;
	outfile << " -CPWmaxavedis "	<< averagePWDistance;
	outfile << " -CPWmaxmeddis " 	<< final_median_dist;
	outfile << " -CPWmeddis " 		<< final_medianpw_dist;
	outfile << " -CPWmaxhoc " 		<< core_max_hopcount;
	outfile << " -CPWavehoc " 		<< (double)(core_sum_hopcount/(corenode_cnt));
	outfile << " -CPWmaxmedhoc " 	<< final_median_hops;
	outfile << " -CPWmedhoc " 		<< final_medianpw_hops;
	outfile << " -CPWmaxcos "		<< core_worstcase_cost;
	outfile << " -CPWmaxavecos "	<< averagePWCost;
	outfile << " -CPWmaxmedcos " 	<< final_median_cost;
	outfile << " -CPWmedcos " 		<< final_medianpw_cost;
	outfile << " -CRDPmax " 		<< core_relative_delay_penalty_max;
	outfile << " -CRDPave "			<< temp_core_ecc_path_dist/temp_core_shortest_path_dist;
	
	coreStatistics(outfile);

	return true;
}

void
GraphStatistics::SearchWorstCasePairAll(int src, int prev, int curr, bool core_node, bool member_node, StatValues &SV)
{
	out_edge_iteratorN eit, eit_end;
	int targ = -1;
	double temp_wc_cost = 0, temp_wc_dist = 0;
    int temp_hop_count = 0;
	PathList temp2_path, temp1_path;

	while(true)
	{
		int degree = out_degree(curr, g);
		if(degree > 2)			// -- split -> pick one out and start search again --
		{
            for(boost::tuples::tie(eit, eit_end) = out_edges(curr, g); eit != eit_end; ++eit)
			{
				edge_descriptorN e = *eit;
				targ = target(e, g);
				if(targ == prev) continue;	 // don't go back 

				// -- recursive call --
				temp1_path.push_back(targ);
				StatValues tempSV(get(&EdgeProp::weight, g)[*eit], get(&EdgeProp::cost, g)[*eit], 1, &temp1_path);
				//StatValues tempSV(get(&EdgeProp::weight, g)[e], get(&EdgeProp::cost, g)[e], 1);
				SearchWorstCasePairAll(src, curr, targ, core_node, member_node, tempSV);

				if(temp_wc_dist < tempSV.worstcase_dist) 
				{
					temp_wc_dist = tempSV.worstcase_dist; 
					temp2_path = temp1_path;
				}

				if(member_node)
				{
					// cost
					avmembernode_cost = avmembernode_cost + tempSV.worstcase_cost; 
					if(temp_wc_cost < tempSV.worstcase_cost) temp_wc_cost = tempSV.worstcase_cost;
					vecPWMemberCost.push_back(tempSV.worstcase_cost + SV.worstcase_cost);
					
					// distance
					avmembernode_dist = avmembernode_dist + tempSV.worstcase_dist; 
					vecPWMemberDist.push_back(tempSV.worstcase_dist + SV.worstcase_dist);

					// hops
					if(temp_hop_count < tempSV.hopcount) temp_hop_count = tempSV.hopcount;
					vecPWMemberHops.push_back(tempSV.hopcount + SV.hopcount);
				}
				if(core_node)
				{
					core_avmembernode_cost = core_avmembernode_cost + tempSV.worstcase_cost; 
					vecCorePWMemberCost.push_back(tempSV.worstcase_cost + SV.worstcase_cost);

					core_avmembernode_dist = core_avmembernode_dist + tempSV.worstcase_dist; 
					vecCorePWMemberDist.push_back(tempSV.worstcase_dist + SV.worstcase_dist);
					
					vecCorePWMemberHops.push_back(tempSV.hopcount + SV.hopcount);
				}
				
				// path
				temp1_path.clear();
			}
			if(curr != src)
			{
				SV.worstcase_cost += temp_wc_cost;
				SV.worstcase_dist += temp_wc_dist;
				SV.hopcount += temp_hop_count;
				SV.path_list->insert(SV.path_list->end(), temp2_path.begin(), temp2_path.end());
			}
			else if(curr == src)
			{
				SV.worstcase_cost = temp_wc_cost;
				SV.worstcase_dist = temp_wc_dist;
				SV.hopcount = temp_hop_count;
				SV.path_list->clear();
				SV.path_list->insert(SV.path_list->end(), temp2_path.begin(), temp2_path.end());
			}
			break;
		}
		else if(degree == 2) 	//  -- one in and one out --
		{
            for(boost::tuples::tie(eit, eit_end) = out_edges(curr, g); eit != eit_end; ++eit)
			{
				targ = target(*eit, g);
				if(prev != targ) break;
			}
			SV.worstcase_dist += get(&EdgeProp::weight, g)[*eit];
			SV.worstcase_cost += get(&EdgeProp::cost, g)[*eit];
			SV.hopcount += 1;
			SV.path_list->push_back(targ);
		}
		else if(degree == 1) 	// -- leaf node --
		{
            boost::tuples::tie(eit, eit_end) = out_edges(curr, g);
			ASSERTING(eit != eit_end);

			targ = target(*eit, g);
			if(prev == targ) break;

			SV.worstcase_dist += get(&EdgeProp::weight, g)[*eit];
			SV.worstcase_cost += get(&EdgeProp::cost, g)[*eit];
			SV.hopcount += 1;
			SV.path_list->push_back(targ);
		}
		prev = curr;
	   	curr = targ;
	}	
}

void GraphStatistics::meshStatistics(ostream &outfile)
{
	// print stats to file
	outfile << " -V "			<< T.V.size();
	outfile << " -E "			<< T.Ep.size();
	outfile << " -Z "			<< T.V.size() - T.S.size();
	outfile << " -S "			<< T.S.size(); 

	graphEdgeStats(outfile);
	graphNodeDegreeStats(outfile);

	distanceMatrix(distance_, distance_hops_, distance_parent_);
	distanceMatrixHops(hop_distance_, hops_, hop_parent_);

	// member nodes
	VertexSet memberSet = T.V - T.S;	
	int membernode_cnt = memberSet.size();
	double diameter_distance = 0; 
	double max_hopcount = 0, sum_hopcount = 0, hop_diameter = 0, sum_hopcount_ecc = 0;	
	double hop_diameter_distance = 0, sum_hopcount_ecc_distance = 0;	

	// Relative Delay Penalty
	double relative_delay_penalty_max = 0, temp_shortest_path_dist = 0, temp_ecc_path_dist = 0;
	
	// pair-wise latency
	double pair_wise_latency = 0;
	for(VertexSet::iterator vit = memberSet.begin(), vit_end = memberSet.end(); vit != vit_end; ++vit)
	{
		int src = *vit;
		for(VertexSet::iterator vit_in = vit, vit_in_end = memberSet.end(); vit_in != vit_in_end; ++vit_in)
		{
			int targ = *vit_in;
			if(src == targ) continue;

			// -- hop diameter --
			// hop diameter
			int hopecc = (int)hops_[src][targ];
			if(hop_diameter < hopecc) hop_diameter = hopecc;
			sum_hopcount_ecc += hopecc;

			// hop diameter -> distance	
			double hop_distance = hop_distance_[src][targ];
			if(hop_diameter_distance < hop_distance) hop_diameter_distance = hop_distance;
			sum_hopcount_ecc_distance += hop_distance;
			
			// -- distance diameter --
			// distance diameter
			double eccentricity = distance_[src][targ];
			if(eccentricity > diameter_distance) diameter_distance = eccentricity;
			pair_wise_latency += eccentricity; 	// pair-wise latency

			// distance diameter -> hop
			int hopcount = (int)distance_hops_[src][targ];
			if(max_hopcount < hopcount)	max_hopcount = hopcount;
			sum_hopcount += hopcount;

			// RDP
			pair<edge_descriptorN, bool> ep = edge(src, targ, globalG);
			ASSERTING(ep.second);
			temp_shortest_path_dist += globalG[ep.first].weight;
			temp_ecc_path_dist += eccentricity;
			if(relative_delay_penalty_max < (eccentricity/globalG[ep.first].weight)) 
				relative_delay_penalty_max = (eccentricity/globalG[ep.first].weight);
		}
	}

	pair_wise_latency = pair_wise_latency/( membernode_cnt * ( membernode_cnt - 1));
	if(temp_shortest_path_dist == 0) temp_shortest_path_dist = min_edge_dist_;

	// Stats
	outfile << " -PWmaxdis "	<< diameter_distance;
	outfile << " -PWavedis "	<< pair_wise_latency;
	outfile << " -PWmaxhocdis " << max_hopcount;
	outfile << " -PWavehocdis " << sum_hopcount/( membernode_cnt * ( membernode_cnt - 1));
	outfile << " -PWmaxhoc " 	<< hop_diameter;
	outfile << " -PWavehoc " 	<< sum_hopcount_ecc/( membernode_cnt * ( membernode_cnt - 1));
	outfile << " -PWmaxdishoc " << hop_diameter_distance;
	outfile << " -PWavedishoc " << sum_hopcount_ecc_distance/( membernode_cnt * ( membernode_cnt - 1));
	outfile << " -RDPmax " 		<< relative_delay_penalty_max;
	outfile << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;

	// -- stats --
	/*cerr << " -PWmaxdis "	<< worstcase_dist;
	cerr << " -PWavedis "	<< pair_wise_latency;
	cerr << " -PWmaxhocdis " << max_hopcount;
	cerr << " -PWavehocdis " << sum_hopcount/( membernode_cnt * ( membernode_cnt - 1));
	cerr << " -PWmaxhoc " 	<< hop_diameter;
	cerr << " -PWavehoc " 	<< sum_hopcount_ecc/( membernode_cnt * ( membernode_cnt - 1));
	cerr << " -RDPmax " 		<< relative_delay_penalty_max;
	cerr << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;
	char c = getchar();*/
}

// dijkstra's shortest path tree
void GraphStatistics::maxDistance(int src, DistanceVector &zdistance, DistanceVector &hops, ParentVector &zparent)
{
	ASSERTING(T.V.contains(src));
	out_edge_iteratorN oit, oit_end;
	
	HeapD<FHeap> heapD;
	Heap *heap = heapD.newInstance(num_vertices(g));
	
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		zdistance[*vit] = (std::numeric_limits<double>::max)();
		hops[*vit] = (std::numeric_limits<double>::max)();
		zparent[*vit] = *vit;
		heap->insert(*vit, (std::numeric_limits<double>::max)());
	}

	hops[src] = 0;
	zdistance[src] = 0;
	zparent[src] = src;
	heap->decreaseKey(src, 0.0);

	while(heap->nItems() > 0)
	{
		int u = heap->deleteMin();
        for(boost::tuples::tie(oit, oit_end) = out_edges(u, g); oit != oit_end; ++oit)
		{
			int v = target(*oit, g);
			double path_weight = zdistance[u] + g[*oit].weight;

			if((path_weight < zdistance[v]))
			{
				zdistance[v] = path_weight;
				hops[v] = hops[u] + 1;
				zparent[v] = u;
				heap->decreaseKey(v, zdistance[v]);
			}
		}
	}	
	delete heap;
}


// breadth first search tree
double GraphStatistics::maxDistanceHops(int src, DistanceVector &distance)
{
	ASSERTING(T.V.contains(src));

	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		distance[*vit] = (std::numeric_limits<double>::max)();   

	distance[src] = 0;
	double diameter = 0;

	typedef list<int> IntList;
	IntList neighbors;
	neighbors.push_back(src);
	
	while(!neighbors.empty())
	{
		int v = neighbors.front();
		neighbors.pop_front();

		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
		{
			int u = target(*oit, g);
			ASSERTING(T.V.contains(u));
		
			if(distance[u] == (std::numeric_limits<double>::max())) 
			{
				distance[u] = distance[v] + 1; 
				neighbors.push_back(u);
				if(distance[u] > diameter) diameter = distance[u];
			}
		}
	}

	return diameter;
}

void GraphStatistics::maxDistanceHops(int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent)
{
	ASSERTING(T.V.contains(src));

	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
		hops[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
		parent[*vit] = *vit;
	}

	distance[src] = 0;
	hops[src] = 0;
	parent[src] = src;

	typedef list<int> IntList;
	IntList neighbors;
	neighbors.push_back(src);
	
	while(!neighbors.empty())
	{
		int v = neighbors.front();
		neighbors.pop_front();

		ASSERTING(T.V.contains(v));
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
		{
			int u = target(*oit, g);
			ASSERTING(T.V.contains(u));
		
			if(hops[u] == (std::numeric_limits<double>::max())) 
			{
				hops[u] = hops[v] + 1;
				distance[u] = distance[v] + g[*oit].weight;
				parent[u] = v;
				neighbors.push_back(u);
			}
		}
	}
}



double GraphStatistics::diameter()
{
	double max = 0;
	DistanceVector distance(num_vertices(T.g));
	DistanceVector hops(num_vertices(T.g));
	ParentVector parent(num_vertices(T.g));

	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		double d = maxDistanceHops(v, distance); //, hops, parent);
		max = d > max ? d : max;
	}

	return max;
}

//It is useful to have a readily available table (matrix) of the shortest distances between any two vertices. We can compute such a distance matrix as follows:
void GraphStatistics::distanceMatrix(DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent)
{
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		//cerr << WRITE_FUNCTION << " distances for " << v << endl;
		maxDistance(v, distance[v], hops[v], parent[v]);
	}
} 

void GraphStatistics::distanceMatrixHops(DistanceVectorMatrix &distance)
{
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		maxDistanceHops(v, distance[v]);
	}
}

void GraphStatistics::distanceMatrixHops(DistanceVectorMatrix &distance, DistanceVectorMatrix &hops, ParentVectorMatrix &parent)
{
	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		maxDistanceHops(v, distance[v], hops[v], parent[v]);
	}
}


void GraphStatistics::statsAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path, StatValues &SV)
{
	if(src == targ) 
	{
		//cerr << src << " ";
		path.push_back(src);
	}
	else if(parent[src][targ] == -1)
	{
		cerr << " no path from " << src << " to " << targ << " exists " << endl;
	}
	else
	{
		statsAllPairsSP(parent, src, parent[src][targ], path, SV);
		//cerr << targ << " ";
		path.push_back(targ);
	}
}

void GraphStatistics::printAllPairsSP(ParentMatrix &parent, int src, int targ, PathList &path)
{
	if(src == targ) 
	{
		cerr << src << " ";
		path.push_back(src);
	}
	else if(parent[src][targ] == src)
	{
		cerr << " no path from " << src << " to " << targ << " exists " << endl;
	}
	else
	{
		printAllPairsSP(parent, src, parent[src][targ], path);
		cerr << targ << " ";
		path.push_back(targ);
	}
}

// calculate maximum/average/median eccentricity
void GraphStatistics::coreStatistics(ostream &outfile)
{
	multimap<double, int> rankMapMedian, rankMapAverage;
	dEdgeMap wmap = get(&EdgeProp::weight, globalG);
	double sum_weight = 0;
		
	for(VertexSet::iterator vit = T.C.begin(), vit_end = T.C.end(); vit != vit_end; ++vit)
	{
		multiset<double> weightSet;
		out_edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, globalG); eit != eit_end; ++eit)
		{
			weightSet.insert(wmap[*eit]);
			sum_weight += wmap[*eit];		
		}
		
		// Median
		int median = weightSet.size()/2;
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if( i >= median) 
			{
				rankMapMedian.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}

		// Average
		rankMapAverage.insert(pair<double, int>((sum_weight/out_degree(*vit, globalG)), *vit));
		sum_weight = 0;
	}
	
	// Minimium eccentricity median/average 
	// 25 percentile
	// Average eccentricity median/average 50 percentile
	// 75 percentile
	// 90 percentile
	// Maximum eccentricity median/average
		
	double min_med_ecc = 0, twentyfive_med_ecc = 0, fifty_med_ecc = 0, seventyfive_med_ecc = 0, ninety_med_ecc = 0, max_med_ecc = 0;
	int size_map = rankMapMedian.size();
	int middle = size_map/2, twentyfive = (size_map * 25)/100, seventyfive = (size_map * 75)/100, ninety = (size_map * 90)/100;
	multimap<double, int>::iterator mit = rankMapMedian.begin(), mit_end = rankMapMedian.end();
	for(int i = 0; mit != mit_end; i++, ++mit)
	{
		if(i == 0)
		{
			min_med_ecc = mit->first;
		}
		if(i == twentyfive)
		{
			twentyfive_med_ecc = mit->first;
		}
		if(i == middle)
		{
			fifty_med_ecc = mit->first;
		}
		if(i == seventyfive)
		{
			seventyfive_med_ecc = mit->first;
		}
		if(i == ninety)
		{
			ninety_med_ecc = mit->first;
		}
		if(i == (size_map - 1))
		{
			max_med_ecc = mit->first;
		}		
	}
	
	double min_ave_ecc = 0, twentyfive_ave_ecc = 0, fifty_ave_ecc = 0, seventyfive_ave_ecc = 0, ninety_ave_ecc = 0, max_ave_ecc = 0;
	size_map = rankMapAverage.size();
	middle = size_map/2;
	twentyfive = (size_map * 25)/100;
	seventyfive = (size_map * 75)/100;
	ninety = (size_map * 90)/100;
	mit = rankMapAverage.begin();
	mit_end = rankMapAverage.end();
	for(int i = 0; mit != mit_end; i++, ++mit)
	{
		if(i == 0)
		{
			min_ave_ecc = mit->first;
		}
		if(i == twentyfive)
		{
			twentyfive_ave_ecc = mit->first;
		}
		if(i == middle)
		{
			fifty_ave_ecc = mit->first;
		}
		if(i == seventyfive)
		{
			seventyfive_ave_ecc = mit->first;
		}
		if(i == ninety)
		{
			ninety_ave_ecc = mit->first;
		}
		if(i == (size_map - 1))
		{
			max_ave_ecc = mit->first;
		}		
	}

	outfile << " -CMedEccMin "	<< min_med_ecc;
	outfile << " -CMedEcc25 "	<< twentyfive_med_ecc;
	outfile << " -CMedEcc50 "	<< fifty_med_ecc;
	outfile << " -CMedEcc75 "	<< seventyfive_med_ecc;
	outfile << " -CMedEcc90 "	<< ninety_med_ecc;
	outfile << " -CMedEccMax "	<< max_med_ecc;

	outfile << " -CAveEccMin "	<< min_ave_ecc;
	outfile << " -CAveEcc25 "	<< twentyfive_ave_ecc;
	outfile << " -CAveEcc50 "	<< fifty_ave_ecc;
	outfile << " -CAveEcc75 "	<< seventyfive_ave_ecc;
	outfile << " -CAveEcc90 "	<< ninety_ave_ecc;
	outfile << " -CAveEccMax "	<< max_ave_ecc;
}

void GraphStatistics::meshStatisticsFloydWarshall(ostream &outfile)
{
	// print stats to file
	outfile << " -V "			<< T.V.size();
	outfile << " -E "			<< T.Ep.size();
	outfile << " -Z "			<< T.V.size() - T.S.size();
	outfile << " -S "			<< T.S.size(); 

	graphEdgeStats(outfile);
	graphNodeDegreeStats(outfile);

	// mesh distance stats
	DistanceMatrix distance;
	ParentMatrix parent;
	//cerr << "A " ;
	TreeAlgorithms::floyd_warshall_apsp(T.g, distance, parent, T.V);
	//cerr << "B " ;

	// member nodes
	VertexSet memberSet = T.V - T.S;	
	int membernode_cnt = memberSet.size();
	double worstcase_dist = 0, temp_medianmembernode_dist = 0; 
	double max_hopcount = 0, sum_hopcount = 0;	

	// Relative Delay Penalty
	double relative_delay_penalty_max = 0, temp_shortest_path_dist = 0, temp_ecc_path_dist = 0;
	
	// pair-wise latency
	double pair_wise_latency = 0;
	for(VertexSet::iterator vit = memberSet.begin(), vit_end = memberSet.end(); vit != vit_end; ++vit)
	{
		int src = *vit;
		for(VertexSet::iterator vit_in = vit, vit_in_end = memberSet.end(); vit_in != vit_in_end; ++vit_in)
		{
			int targ = *vit_in;
			if(src == targ) continue;

			// hop count
			PathList path;
			StatValues SV;
			statsAllPairsSP(parent, src, targ, path, SV);
			int hopcount = path.size();
			if(max_hopcount < hopcount)	max_hopcount = hopcount;
			sum_hopcount += hopcount;
	
			double eccentricity = distance[src][targ];

			// eccentricities
			if(distance[src][targ] > worstcase_dist)
				worstcase_dist = distance[src][targ];

			// pair-wise latency
			pair_wise_latency += eccentricity;

			// RDP
			pair<edge_descriptorN, bool> ep = edge(src, targ, globalG);
			ASSERTING(ep.second);
			temp_shortest_path_dist += globalG[ep.first].weight;
			temp_ecc_path_dist += eccentricity;
			if(relative_delay_penalty_max < (eccentricity/globalG[ep.first].weight)) 
				relative_delay_penalty_max = (eccentricity/globalG[ep.first].weight);
		}
	}
	//cerr << "C " ;

	pair_wise_latency = pair_wise_latency/( membernode_cnt * ( membernode_cnt - 1));

	double final_median_dist = 0, final_medianpw_dist = 0;
	double final_median_hops = 0, final_medianpw_hops = 0;
	double worstcase_cost = 0, averagePWCost = 0;
	double final_median_cost = 0, final_medianpw_cost = 0;

	// Stats
	//outfile << " -V "			<< T.V.size();
	//outfile << " -E "			<< T.Ep.size();
	//outfile << " -Z "			<< T.V.size() - T.S.size();
	//outfile << " -S "			<< T.S.size(); 
	//outfile << " -L " 			<< leafnode_cnt;
	//outfile << " -Cdis " 		<< tree_dist;
	//outfile << " -Emaxdis "		<< max_edge_dist;
	//outfile << " -Emindis " 	<< min_edge_dist;
	//outfile << " -Eavedis " 	<< sum_edge_dist/T.Ep.size();
	//outfile << " -Emeddis "		<< final_median_edgedist;
	outfile << " -PWmaxdis "	<< worstcase_dist;
	outfile << " -PWmaxavedis "	<< pair_wise_latency;
	outfile << " -PWmaxmeddis " << final_median_dist;
	outfile << " -PWmeddis " 	<< final_medianpw_dist;
	outfile << " -PWmaxhoc " 	<< max_hopcount;
	outfile << " -PWavehoc " 	<< sum_hopcount/( membernode_cnt * ( membernode_cnt - 1));
	outfile << " -PWmaxmedhoc " << final_median_hops;
	outfile << " -PWmedhoc " 	<< final_medianpw_hops;
	//outfile << " -ODmax " 		<< max_outdegree_;
	//outfile << " -ODmaxgm "     << max_outdegree_gm;
	//outfile << " -ODavewol "	<< average_out_degree_wo_leafs; 
	//outfile << " -ODavewolsp "  << average_out_degree_wo_leafs_sp;
	//outfile << " -ODavewolgm "  << average_out_degree_wo_leafs_gm;
	//outfile << " -ODave "		<< (double)(sum_outdegree/(double)(T.V.size()));
	//outfile << " -Ccos " 		<< tree_cost;
	//outfile << " -Emaxcos "		<< max_edge_cost;
	//outfile << " -Emincos " 	<< min_edge_cost;
	//outfile << " -Eavecos " 	<< sum_edge_cost/T.Ep.size();
	//outfile << " -Emedcos "		<< final_median_edgecost;
	outfile << " -PWmaxcos "	<< worstcase_cost;
	outfile << " -PWmaxavecos "	<< averagePWCost;
	outfile << " -PWmaxmedcos " << final_median_cost;
	outfile << " -PWmedcos " 	<< final_medianpw_cost;
	outfile << " -RDPmax " 		<< relative_delay_penalty_max;
	outfile << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;

	// -- stats --
	/*cerr << " -PWmaxdis "	<< worstcase_dist;
	cerr << " -PWmaxavedis "<< pair_wise_latency;
	cerr << " -PWmaxhoc " 	<< max_hopcount;
	cerr << " -PWavehoc " 	<< sum_hopcount/( membernode_cnt * ( membernode_cnt - 1));
	cerr << " -RDPmax " 	<< relative_delay_penalty_max;
	cerr << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;
	cerr << endl;
	char c = getchar();*/
}


}; // namespace Statistics

/*
bool
GraphStatistics::TreeStatsAll(ostream &outfile)
{
	// weight and cost maps
	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	dEdgeMapConst cmap = get(&EdgeProp::cost, g);
	
   	// variables 
	double tree_dist = 0, max_edge_dist = 0, min_edge_dist = (numeric_limits<double>::max)(), sum_edge_dist = 0;
	double tree_cost = 0, max_edge_cost = 0, min_edge_cost = (numeric_limits<double>::max)(), sum_edge_cost = 0;
	double medEdgeCost[T.Ep.size()], medEdgeDist[T.Ep.size()];

	int i = 0;
	for(EdgePair::const_iterator seit = T.Ep.begin(), seit_end = T.Ep.end(); seit != seit_end; ++seit, i++)
	{
		pair<edge_descriptorN, bool> e = edge(seit->first, seit->second, g);
		assert(e.second);
	
		ASSERTING(wmap[e.first] > 0);

		// weight
		tree_dist = tree_dist + wmap[e.first];	
		if(wmap[e.first] > max_edge_dist) max_edge_dist = wmap[e.first];
		if(wmap[e.first] < min_edge_dist) min_edge_dist = wmap[e.first];
		sum_edge_dist += wmap[e.first];
		medEdgeDist[i] = wmap[e.first];
		
		// cost
		tree_cost = tree_cost + cmap[e.first];	
		if(cmap[e.first] > max_edge_cost) max_edge_cost = cmap[e.first];
		if(cmap[e.first] < min_edge_cost) min_edge_cost = cmap[e.first];
		sum_edge_cost += cmap[e.first];
		medEdgeCost[i] = cmap[e.first];
	}
	
	// variables 
	double max_outdegree = 0, max_outdegree_gm = 0, sum_outdegree = 0;
	double sum_outdegree_woleafs = 0, sum_outdegree_woleafs_sp = 0, sum_outdegree_woleafs_gm = 0;	
	VertexSet leafSet, leafSetGM, leafSetSP;
	
	VertexSet errorV; // error handling 
	for(VertexSet::const_iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		int v = *vit;
		int degree = out_degree(v, g);
		if(degree > 1)
		{
			sum_outdegree_woleafs = sum_outdegree_woleafs + degree; 			// summarize out degree excluding leaf-nodes
			if(g[v].vertexState == STEINER_POINT) sum_outdegree_woleafs_sp = sum_outdegree_woleafs_sp + degree;
			else if(g[v].vertexState == GROUP_MEMBER) sum_outdegree_woleafs_gm = sum_outdegree_woleafs_gm + degree;
			else cerr << WRITE_FUNCTION << " vertex: " << v << " has no valid state! : " << g[v] << endl;
		}
		else if(degree == 1) 
		{
			leafSet.insert(v);
			if(g[v].vertexState == STEINER_POINT) leafSetSP.insert(v);
			else if(g[v].vertexState == GROUP_MEMBER) leafSetGM.insert(v);
			else cerr << WRITE_FUNCTION << " vertex: " << v << " has no valid state! : " << g[v] << endl;
		}
		else // -- error handling --
		{
			//cerr << WRITE_FUNCTION << " vertex: " << v << " out degree == 0" << endl;
			errorV.insert(v);
		} // -- error handling end --
		
		sum_outdegree += degree; 		
		if(degree > max_outdegree) max_outdegree = degree;	
		if(g[v].vertexState == GROUP_MEMBER && degree > max_outdegree_gm) max_outdegree_gm = degree;
	}
	
	// -- error handling --
	if(errorV.size()) 
	{
		//cerr << ";" ; 
		cerr << WRITE_FUNCTION << " vertices " << errorV << " out degree == 0" << endl;
		return false;
	}
	// -- error handling end --
	
	ASSERTING(leafSet.size() > 1);
	
	// variables
	int leafnode_cnt_sp = leafSetSP.size();
	int leafnode_cnt = leafSet.size();
	int leafnode_cnt_gm = leafSetGM.size(); 

	// member nodes
	int membernode_cnt = (T.V - T.S).size();
	double medMemberCost[membernode_cnt], medMemberDist[membernode_cnt], medMemberHops[membernode_cnt];
	double worstcase_dist = 0, temp_medianmembernode_dist = 0; 
	double worstcase_cost = 0, temp_medianmembernode_cost = 0; 
	double max_hopcount = 0, sum_hopcount = 0;	

	// core nodes
	int corenode_cnt = T.C.size();
	double medCoreMemberCost[corenode_cnt], medCoreMemberDist[corenode_cnt], medCoreMemberHops[corenode_cnt];
	double core_worstcase_dist = 0, temp_core_medianmembernode_dist = 0; 
	double core_worstcase_cost = 0, temp_core_medianmembernode_cost = 0; 
	double core_max_hopcount = 0, core_sum_hopcount = 0;	

	// Relative Delay Penalty
	double relative_delay_penalty_max = 0, temp_shortest_path_dist = 0, temp_ecc_path_dist = 0;
	double core_relative_delay_penalty_max = 0, temp_core_shortest_path_dist = 0, temp_core_ecc_path_dist = 0;

	VertexSet memberSet = T.V; // - T.S;
	int member_count = 0, core_count = 0; i = 0;
	// worst case pair wise cost 
	for(VertexSet::const_iterator vit = memberSet.begin(), vit_end = memberSet.end(); vit != vit_end; ++vit, i++) 
	{
		int v = *vit;

		bool core_node = false;
		if(T.C.contains(v)) core_node = true;

		bool member_node = true;
		if(T.S.contains(v)) member_node = false;
		
		// steiner point?
		if(member_node == false && core_node == false) continue;
		
		diameterPath_.clear();
		StatValues SV(&diameterPath_);
	    SearchWorstCasePairAll(v, v, v, core_node, member_node, SV);

		if(core_node == true) // core set
		{
			// record core set stats
			// RDP
			int max_ecc_node = *(SV.path_list->rbegin());
			pair<edge_descriptorN, bool> ep = edge(v, max_ecc_node, globalG);
			ASSERTING(ep.second);
			temp_core_shortest_path_dist += globalG[ep.first].weight;
			temp_core_ecc_path_dist += SV.worstcase_dist;
			if(core_relative_delay_penalty_max < (SV.worstcase_dist/globalG[ep.first].weight)) 
				core_relative_delay_penalty_max = (SV.worstcase_dist/globalG[ep.first].weight);

			// cost
			if(SV.worstcase_cost > core_worstcase_cost) core_worstcase_cost = SV.worstcase_cost;
			double median_cost = (core_avmembernode_cost - temp_core_medianmembernode_cost)/(corenode_cnt - 1);
			if(median_cost > 0) medCoreMemberCost[medCoreMemberCostSize++] = median_cost;
			temp_core_medianmembernode_cost = core_avmembernode_cost;

			// distance
			if(SV.worstcase_dist > core_worstcase_dist) core_worstcase_dist = SV.worstcase_dist;
			double median_dist = (core_avmembernode_dist - temp_core_medianmembernode_dist)/(corenode_cnt - 1);
			if(median_dist > 0) medCoreMemberDist[medCoreMemberDistSize++] = median_dist;
			temp_core_medianmembernode_dist = core_avmembernode_dist;
				
			// hops
			if(core_max_hopcount < SV.hopcount)	core_max_hopcount = SV.hopcount;
			core_sum_hopcount += SV.hopcount;
			medCoreMemberHops[core_count++] = (double) SV.hopcount;
		}

		if(member_node == false) continue;
		
		// RDP
		int max_ecc_node = *(SV.path_list->rbegin());
		pair<edge_descriptorN, bool> ep = edge(v, max_ecc_node, globalG);
		ASSERTING(ep.second);
		temp_shortest_path_dist += globalG[ep.first].weight;
		temp_ecc_path_dist += SV.worstcase_dist;
		if(relative_delay_penalty_max < (SV.worstcase_dist/globalG[ep.first].weight)) 
			relative_delay_penalty_max = (SV.worstcase_dist/globalG[ep.first].weight);

		// cost
		if(SV.worstcase_cost > worstcase_cost) worstcase_cost = SV.worstcase_cost;
		double median_cost = (avmembernode_cost - temp_medianmembernode_cost)/(membernode_cnt - 1);
		if(median_cost > 0) medMemberCost[medMemberCostSize++] = median_cost;
		temp_medianmembernode_cost = avmembernode_cost;

		//cerr << v << ": avmembernode_dist " << avmembernode_dist << " median " << (avmembernode_dist - temp_medianmembernode_dist)/membernode_cnt;
		//cerr << "median_cost "	<< median_cost << endl;

		// distance
		if(SV.worstcase_dist > worstcase_dist) worstcase_dist = SV.worstcase_dist;
		double median_dist = (avmembernode_dist - temp_medianmembernode_dist)/(membernode_cnt - 1);
		if(median_dist > 0) medMemberDist[medMemberDistSize++] = median_dist;
		temp_medianmembernode_dist = avmembernode_dist;
			
		// hops
		if(max_hopcount < SV.hopcount)	max_hopcount = SV.hopcount;
		sum_hopcount += SV.hopcount;
		medMemberHops[member_count++] = (double) SV.hopcount;

		//cerr << "SV.hc " << SV.hopcount << " SV.worstcase_dist " << SV.worstcase_dist << " SV.worstcase_cost " << SV.worstcase_cost << endl;
	}	

	// ----- Calculate Distance Sets -------	
	// find median distance
	double final_median_dist = 0;
	if(T.Ep.size() == 1) 
		final_median_dist = min_edge_dist; 		// only one edge
	else if(max_outdegree == 2) 			 	// one long line
		final_median_dist = worstcase_dist;
	else
	{
		qsort((void*) medMemberDist, medMemberDistSize, sizeof(double), MyDouble_compare); 
		final_median_dist = findMedian(medMemberDist, medMemberDistSize);
	}
	ASSERTING(final_median_dist > 0);

	// find median pairwise distance
	double final_medianpw_dist = 0;
	if(T.Ep.size() == 1)
		final_medianpw_dist = min_edge_dist; 	// only one edge
	else if(max_outdegree == 2) 				// one long line
		final_medianpw_dist = worstcase_dist;
	else
	{
		double medPWMemberDist[vecPWMemberDist.size()];
		fillArray(vecPWMemberDist, medPWMemberDist);		
		qsort((void*) medPWMemberDist, vecPWMemberDist.size(), sizeof(double), MyDouble_compare); 
		final_medianpw_dist = findMedian(medPWMemberDist, vecPWMemberDist.size());
	}
	ASSERTING(final_medianpw_dist > 0);
	
	// find median edge distance
	qsort((void*) medEdgeDist, T.Ep.size(), sizeof(double), MyDouble_compare);
	double final_median_edgedist = findMedian(medEdgeDist, T.Ep.size());
	ASSERTING(final_median_edgedist > 0);

	// find median hops
	qsort((void*) medMemberHops, membernode_cnt, sizeof(double), MyDouble_compare);
	double final_median_hops = findMedian(medMemberHops, membernode_cnt); 
	ASSERTING(final_median_hops > 0);

	// find median pairwise hop
	double final_medianpw_hops = 0;
	if(vecPWMemberHops.size() == 0)
	{
		ASSERTING(T.Ep.size() == 1 || max_outdegree == 2);
		final_medianpw_hops = final_median_hops;
	}
	else	
	{
		double medPWMemberHops[vecPWMemberHops.size()];
		fillArray(vecPWMemberHops, medPWMemberHops);
		qsort((void*) medPWMemberHops, vecPWMemberHops.size(), sizeof(double), MyDouble_compare);
		final_medianpw_hops = findMedian(medPWMemberHops, vecPWMemberHops.size());
	}
	ASSERTING(final_medianpw_hops > 0);	
	
	double averagePWDistance = 0;
	if(T.Ep.size() == 1)
		averagePWDistance = (double)(min_edge_dist); 
	else if(max_outdegree == 2)
		averagePWDistance = worstcase_dist;
	else
		averagePWDistance = (double)(avmembernode_dist/(medMemberDistSize*(membernode_cnt - 1))); 
	if(averagePWDistance == 0)
		averagePWDistance = sum_edge_dist/T.Ep.size();
	ASSERTING(averagePWDistance > 0);	

	double average_out_degree_wo_leafs = 0;
	double average_out_degree_wo_leafs_sp = 0;
	double average_out_degree_wo_leafs_gm = 0;
	if(T.Ep.size() > 1) 
	{
		average_out_degree_wo_leafs = (double) sum_outdegree_woleafs/(T.V.size() - leafnode_cnt);   
		average_out_degree_wo_leafs_sp = (double) sum_outdegree_woleafs_sp/std::max(1, (int) (T.S.size() - leafnode_cnt_sp)); 
		average_out_degree_wo_leafs_gm = (double) sum_outdegree_woleafs_gm/std::max(1, (int) ((T.V.size() - T.S.size()) - leafnode_cnt_gm));
	}
	
	// ----- Calculate Cost Sets -------		
	// find median cost
	double final_median_cost = 0;
	if(T.Ep.size() == 1) 
		final_median_cost = min_edge_cost; 	// only one edge
	else if(max_outdegree == 2) 			 	// one long line
		final_median_cost = worstcase_cost;
	else
	{
		qsort((void*) medMemberCost, medMemberCostSize, sizeof(double), MyDouble_compare);
		final_median_cost = findMedian(medMemberCost, medMemberCostSize); 
	}
	ASSERTING(final_median_cost > 0);

	// find median pairwise cost
	double final_medianpw_cost = 0;
	if(T.Ep.size() == 1)
		final_medianpw_cost = min_edge_cost; // only one edge
	else if(max_outdegree == 2) 				 // one long line
		final_medianpw_cost = worstcase_cost;
	else
	{
		double medPWMemberCost[vecPWMemberCost.size()];
		fillArray(vecPWMemberCost, medPWMemberCost);
		qsort((void*) medPWMemberCost, vecPWMemberCost.size(), sizeof(double), MyDouble_compare);
		final_medianpw_cost = findMedian(medPWMemberCost, vecPWMemberCost.size()); 
	}
	ASSERTING(final_medianpw_cost > 0);
	
	// find median edge cost
	qsort((void*) medEdgeCost, T.Ep.size(), sizeof(double), MyDouble_compare);
	double final_median_edgecost = findMedian(medEdgeCost, T.Ep.size());
	ASSERTING(final_median_edgecost > 0);

	double averagePWCost = 0;
	if(T.Ep.size() == 1)
		averagePWCost = min_edge_cost; 
	else if(max_outdegree == 2)
		averagePWCost = worstcase_cost;
	else
	   averagePWCost = (avmembernode_cost/(medMemberCostSize* (membernode_cnt - 1)));
	if(averagePWCost == 0)
		averagePWCost = sum_edge_cost/T.Ep.size();
	ASSERTING(averagePWCost > 0);
	
	// Stats
	outfile << " -V "			<< T.V.size();
	outfile << " -E "			<< T.Ep.size();
	outfile << " -Z "			<< T.V.size() - T.S.size();
	outfile << " -S "			<< T.S.size(); 
	outfile << " -L " 			<< leafnode_cnt;
	outfile << " -Cdis " 		<< tree_dist;
	outfile << " -Emaxdis "		<< max_edge_dist;
	outfile << " -Emindis " 	<< min_edge_dist;
	outfile << " -Eavedis " 	<< sum_edge_dist/T.Ep.size();
	outfile << " -Emeddis "		<< final_median_edgedist;
	outfile << " -PWmaxdis "	<< worstcase_dist;
	outfile << " -PWmaxavedis "	<< averagePWDistance;
	outfile << " -PWmaxmeddis " << final_median_dist;
	outfile << " -PWmeddis " 	<< final_medianpw_dist;
	outfile << " -PWmaxhoc " 	<< max_hopcount;
	outfile << " -PWavehoc " 	<< (double)(sum_hopcount/(membernode_cnt));
	outfile << " -PWmaxmedhoc " << final_median_hops;
	outfile << " -PWmedhoc " 	<< final_medianpw_hops;
	outfile << " -ODmax " 		<< max_outdegree;
	outfile << " -ODmaxgm "     << max_outdegree_gm;
	outfile << " -ODavewol "	<< average_out_degree_wo_leafs; 
	outfile << " -ODavewolsp "  << average_out_degree_wo_leafs_sp;
	outfile << " -ODavewolgm "  << average_out_degree_wo_leafs_gm;
	outfile << " -ODave "		<< (double)(sum_outdegree/(double)(T.V.size()));
	outfile << " -Ccos " 		<< tree_cost;
	outfile << " -Emaxcos "		<< max_edge_cost;
	outfile << " -Emincos " 	<< min_edge_cost;
	outfile << " -Eavecos " 	<< sum_edge_cost/T.Ep.size();
	outfile << " -Emedcos "		<< final_median_edgecost;
	outfile << " -PWmaxcos "	<< worstcase_cost;
	outfile << " -PWmaxavecos "	<< averagePWCost;
	outfile << " -PWmaxmedcos " << final_median_cost;
	outfile << " -PWmedcos " 	<< final_medianpw_cost;
	outfile << " -RDPmax " 		<< relative_delay_penalty_max;
	outfile << " -RDPave "		<< temp_ecc_path_dist/temp_shortest_path_dist;
	
	if(T.C.empty()) return true;

	// ---------- add core statistics ------------------
	// ----- Calculate Distance Sets -------	
	// find median distance
	final_median_dist = 0;
	if(T.Ep.size() == 1) 
		final_median_dist = min_edge_dist; 		// only one edge
	else if(max_outdegree == 2) 			 	// one long line
		final_median_dist = core_worstcase_dist;
	else
	{
		qsort((void*) medCoreMemberDist, medCoreMemberDistSize, sizeof(double), MyDouble_compare); 
		final_median_dist = findMedian(medCoreMemberDist, medCoreMemberDistSize);
	}
	//ASSERTING(final_median_dist > 0);

	// find median pairwise distance
	final_medianpw_dist = 0;
	if(T.Ep.size() == 1)
		final_medianpw_dist = min_edge_dist; 	// only one edge
	else if(max_outdegree == 2) 				// one long line
		final_medianpw_dist = core_worstcase_dist;
	else
	{
		double medPWMemberDist[vecCorePWMemberDist.size()];
		fillArray(vecCorePWMemberDist, medPWMemberDist);		
		qsort((void*) medPWMemberDist, vecCorePWMemberDist.size(), sizeof(double), MyDouble_compare); 
		final_medianpw_dist = findMedian(medPWMemberDist, vecCorePWMemberDist.size());
	}
	//ASSERTING(final_medianpw_dist > 0);
	
	// find median hops
	qsort((void*) medCoreMemberHops, corenode_cnt, sizeof(double), MyDouble_compare);
	final_median_hops = findMedian(medCoreMemberHops, corenode_cnt); 
	//ASSERTING(final_median_hops > 0);

	// find median pairwise hop
	final_medianpw_hops = 0;
	if(vecCorePWMemberHops.size() == 0)
	{
		//ASSERTING(T.Ep.size() == 1 || max_outdegree == 2);
		final_medianpw_hops = final_median_hops;
	}
	else	
	{
		double medPWMemberHops[vecCorePWMemberHops.size()];
		fillArray(vecCorePWMemberHops, medPWMemberHops);
		qsort((void*) medPWMemberHops, vecCorePWMemberHops.size(), sizeof(double), MyDouble_compare);
		final_medianpw_hops = findMedian(medPWMemberHops, vecCorePWMemberHops.size());
	}
	//ASSERTING(final_medianpw_hops > 0);	
	
	averagePWDistance = 0;
	if(corenode_cnt == 1)
		averagePWDistance = (double)(min_edge_dist); 
	else if(max_outdegree == 2)
		averagePWDistance = core_worstcase_dist;
	else
		averagePWDistance = (double)(core_avmembernode_dist/(medCoreMemberDistSize*(corenode_cnt - 1))); 
	if(averagePWDistance == 0)
		averagePWDistance = sum_edge_dist/T.Ep.size();
	//ASSERTING(averagePWDistance > 0);	

	// ----- Calculate Cost Sets -------		
	// find median cost
	final_median_cost = 0;
	if(T.Ep.size() == 1) 
		final_median_cost = min_edge_cost; 	// only one edge
	else if(max_outdegree == 2) 			 	// one long line
		final_median_cost = core_worstcase_cost;
	else
	{
		qsort((void*) medCoreMemberCost, medCoreMemberCostSize, sizeof(double), MyDouble_compare);
		final_median_cost = findMedian(medCoreMemberCost, medCoreMemberCostSize); 
	}
	//ASSERTING(final_median_cost > 0);

	// find median pairwise cost
	final_medianpw_cost = 0;
	if(T.Ep.size() == 1)
		final_medianpw_cost = min_edge_cost; // only one edge
	else if(max_outdegree == 2) 				 // one long line
		final_medianpw_cost = core_worstcase_cost;
	else
	{
		double medPWMemberCost[vecCorePWMemberCost.size()];
		fillArray(vecCorePWMemberCost, medPWMemberCost);
		qsort((void*) medPWMemberCost, vecCorePWMemberCost.size(), sizeof(double), MyDouble_compare);
		final_medianpw_cost = findMedian(medPWMemberCost, vecCorePWMemberCost.size()); 
	}
	//ASSERTING(final_medianpw_cost > 0);
	
	averagePWCost = 0;
	if(corenode_cnt == 1)
		averagePWCost = min_edge_cost; 
	else if(max_outdegree == 2)
		averagePWCost = core_worstcase_cost;
	else
	   averagePWCost = (avmembernode_cost/(medCoreMemberCostSize* (corenode_cnt - 1)));
	if(averagePWCost == 0)
		averagePWCost = sum_edge_cost/T.Ep.size();
	//ASSERTING(averagePWCost > 0);
	
	// -- core node stats --
	outfile << " -Core "			<< T.C.size() ;
	outfile << " -CPWmaxdis "		<< core_worstcase_dist;
	outfile << " -CPWmaxavedis "	<< averagePWDistance;
	outfile << " -CPWmaxmeddis " 	<< final_median_dist;
	outfile << " -CPWmeddis " 		<< final_medianpw_dist;
	outfile << " -CPWmaxhoc " 		<< core_max_hopcount;
	outfile << " -CPWavehoc " 		<< (double)(core_sum_hopcount/(corenode_cnt));
	outfile << " -CPWmaxmedhoc " 	<< final_median_hops;
	outfile << " -CPWmedhoc " 		<< final_medianpw_hops;
	outfile << " -CPWmaxcos "		<< core_worstcase_cost;
	outfile << " -CPWmaxavecos "	<< averagePWCost;
	outfile << " -CPWmaxmedcos " 	<< final_median_cost;
	outfile << " -CPWmedcos " 		<< final_medianpw_cost;
	outfile << " -CRDPmax " 		<< core_relative_delay_penalty_max;
	outfile << " -CRDPave "			<< temp_core_ecc_path_dist/temp_core_shortest_path_dist;
	
	coreStatistics(outfile);

	return true;
}

 */

/*
double GraphStatistics::maxDistanceHops(int v, DistanceVector &distance)
{
	ASSERTING(T.V.contains(v));

	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
		distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance

	distance[v] = 0;
	double depth = 1;                       // next distance in BFS 
	int cnt = 0;

	VertexSet neighborSet;
	neighborSet.insert(v);
	
	while(!neighborSet.empty())
	{
		VertexSet nextNeighbors;
		for(VertexSet::iterator vit = neighborSet.begin(), vit_end = neighborSet.end(); vit != vit_end; ++vit)
		{
			int v = *vit;
			out_edge_iteratorN oit, oit_end;
			ASSERTING(T.V.contains(v));

            for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
    	    {
				int u = target(*oit, g);
				if(distance[u] == (std::numeric_limits<double>::max)()) // first time reachable?
				{
					cnt++;
					distance[u] = depth; 
					nextNeighbors.insert(u);
				}
			}
		}
		// next try set of vertices further away
		neighborSet = nextNeighbors;
		depth++;
	}

  	return cnt == T.V.size() ? depth-1 : T.V.size();
}
*/


/*
void GraphStatistics::maxDistance(int src, DistanceVector &distance, DistanceVector &hops, ParentVector &parent)
{
	//const GraphN &g = T.g;
	ASSERTING(T.V.contains(src));

	for(VertexSet::iterator vit = T.V.begin(), vit_end = T.V.end(); vit != vit_end; ++vit)
	{
		distance[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
		hops[*vit] = (std::numeric_limits<double>::max)();   // set to maximum distance
		parent[*vit] = *vit;
	}

	distance[src] = 0;
	hops[src] = 0;
	parent[src] = src;
	double diameter = 0;

	VertexSet neighborSet;
	neighborSet.insert(src);
	
	while(!neighborSet.empty())
	{
		VertexSet nextNeighbors;
		for(VertexSet::iterator vit = neighborSet.begin(), vit_end = neighborSet.end(); vit != vit_end; ++vit)
		{
			int v = *vit;
			//cerr << " v : " << v << endl;

			ASSERTING(T.V.contains(v));
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(v, g); oit != oit_end; ++oit)
    	    {
				int u = target(*oit, g);
				ASSERTING(T.V.contains(u));
			
				if(distance[u] > distance[v] + g[*oit].weight)
				{
					hops[u] = hops[v] + 1;
					distance[u] = distance[v] + g[*oit].weight;
					parent[u] = v;
					nextNeighbors.insert(u);

					//cerr << u << " hops " << hops[u] << " distance " << distance[u] << " parent " << parent[u] << endl;
				}
			}
		}
		// next try set of vertices further away
		neighborSet = nextNeighbors;
	}
}
*/


