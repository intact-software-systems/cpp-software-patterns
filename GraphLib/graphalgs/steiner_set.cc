/***************************************************************************
                          steiner_set.cc  -  description
                             -------------------
    begin                : Thu Mar 09 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include "steiner_set.h"
#include "coreh.h"
#include "../treealgs/treestructure.h"
#include "../graphalgs/meshalgs.h"
#include "../simdefs.h"
#include <fstream>
#include <cmath>

#ifdef WIN32
#include <process.h>
#include <stdio.h>
#endif


using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
					class SteinerPoints
------------------------------------------------------------------------- */

VertexSet SteinerPoints::steinerSet_ = VertexSet();
SteinerPoints::SteinerQueue SteinerPoints::steinerQueue_ = SteinerPoints::SteinerQueue();
size_t SteinerPoints::size_ = 0;

void SteinerPoints::checkQueue()
{
	ASSERTING(size_ == steinerQueue_.size());
}

/*-----------------------------------------------------------------------
					core selection functions
------------------------------------------------------------------------- */
namespace GraphAlgorithms
{
/*
void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &excludeSet, VertexSet &newWCN, int k, CoreSelectionAlgo algo)
{
	//if((algo == WALK_SELECTION || algo == TAILED_SELECTION) && (k <= 1 || excludeSet.size() <= 5)) algo = AVERAGE_DISTANCE;
	if(k <= 0) return;
	if(k <= 1)
	{
		switch(algo)
		{
			case UNIFORM_SELECTION_LAYOUT :
			case UNICAST_WALK_SELECTION:
			case WALK_SELECTION:
			case UNICAST_WALK_SELECTION_GROUP_TREE:
			case WALK_SELECTION_GROUP_TREE:
			case TAILED_SELECTION:
				algo = AVERAGE_DISTANCE;
				break;
			default:
				break;
		}
	}

	//cerr << WRITE_FUNCTION << " steiner points excluding " << excludeSet << " search set : " << nodeSearchSet << " k: " << k << endl;
	switch(algo)
	{
		case RANDOM_SELECTION:
			findkNodesNotInSet(SteinerPoints::steinerQueue_, excludeSet, newWCN, k);
			cerr << WRITE_PRETTY_FUNCTION << " warning!! Not using input steiner points for algo: " << algo << endl;
			sleep(1);
			break;
		case MEDIAN_DISTANCE:
			findkBestLocatedNodesNotInSet_Median(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case AVERAGE_DISTANCE:
			//findkBestLocatedNodesNotInSet_Average(global_g, nodeSearchSet, excludeSet, newWCN, k);
			kMedianNodes(global_g, nodeSearchSet, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case CENTER_SELECTION:
			kCenterNodes(global_g, nodeSearchSet, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
			findkTailedSpreadNotInSet(global_g, group_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION:
			findkTailedSpreadNotInSet_snake(global_g, group_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case TAILED_SELECTION:
			findkNodesTailedSpreadNotInSet(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case BANDWIDTH_SELECTION:
			findkNodesBandwidthNotInSet(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		default:
			cerr << WRITE_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
}

void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &excludeSet, VertexSet &newWCN, int k)
{
	CoreSelectionAlgo algo = GlobalSimArgs::getSimWCNAlgo();
	//if((algo == WALK_SELECTION || algo == TAILED_SELECTION) && (k <= 1 || excludeSet.size() <= 5)) algo = AVERAGE_DISTANCE;
	if(k <= 0) return;
	if(k <= 1)
	{
		switch(algo)
		{
			case UNIFORM_SELECTION_LAYOUT :
			case UNICAST_WALK_SELECTION:
			case WALK_SELECTION:
			case UNICAST_WALK_SELECTION_GROUP_TREE:
			case WALK_SELECTION_GROUP_TREE:
			case TAILED_SELECTION:
				algo = AVERAGE_DISTANCE;
				break;
			default:
				break;
		}
	}

	switch(algo)
	{
		case RANDOM_SELECTION:
			findkNodesNotInSet(SteinerPoints::steinerQueue_, excludeSet, newWCN, k);
			cerr << WRITE_PRETTY_FUNCTION << " warning!! Not using input steiner points for algo: " << algo << endl;
			sleep(1);
			break;
		case MEDIAN_DISTANCE:
			findkBestLocatedNodesNotInSet_Median(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case AVERAGE_DISTANCE:
			kMedianNodes(global_g, nodeSearchSet, nodeSearchSet, excludeSet, newWCN, k);
			//findkBestLocatedNodesNotInSet_Average(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
			findkTailedSpreadNotInSet(global_g, group_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case CENTER_SELECTION:
			kCenterNodes(global_g, nodeSearchSet, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION:
			findkTailedSpreadNotInSet_snake(global_g, group_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case TAILED_SELECTION:
			findkNodesTailedSpreadNotInSet(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case BANDWIDTH_SELECTION:
			findkNodesBandwidthNotInSet(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		default:
			cerr << WRITE_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
}
*/
void findWCNodes(const GraphN& global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeV, const VertexSet &excludeSet, VertexSet &newWCN, int k, CoreSelectionAlgo algo)
{
	//CoreSelectionAlgo algo = GlobalSimArgs::getSimWCNAlgo();
	//if((algo == WALK_SELECTION || algo == TAILED_SELECTION) && (k <= 1 || excludeSet.size() <= 5)) algo = AVERAGE_DISTANCE;
	if(k <= 0) return;
	if(k <= 1)
	{
		switch(algo)
		{
			case UNIFORM_SELECTION_LAYOUT :
			case UNICAST_WALK_SELECTION:
			case WALK_SELECTION:
			case UNICAST_WALK_SELECTION_GROUP_TREE:
			case WALK_SELECTION_GROUP_TREE:
			case TAILED_SELECTION:
				algo = AVERAGE_DISTANCE;
				break;
			default:
				break;
		}
	}

	switch(algo)
	{
		case AVERAGE_DISTANCE:
			kMedianNodes(global_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		case CENTER_SELECTION:
			kCenterNodes(global_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		case WALK_SELECTION_GROUP_TREE:
		case WALK_SELECTION:
			findkTailedSpreadNotInSets(global_g, group_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		case UNICAST_WALK_SELECTION_GROUP_TREE:
		case UNICAST_WALK_SELECTION:
			findkTailedSpreadNotInSets_snake(global_g, group_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		case TAILED_SELECTION:
			findkNodesTailedSpreadNotInSet(global_g, nodeSearchSet, excludeSet, newWCN, k);
			break;
		case BANDWIDTH_SELECTION:
			findkNodesBandwidthNotInSets(global_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		case RANDOM_SELECTION:
			findkNodesNotInSets(SteinerPoints::steinerQueue_, relativeV, excludeSet, newWCN, k);
			cerr << WRITE_PRETTY_FUNCTION << " warning!! Not using input steiner points for algo: " << algo << endl;
            #ifdef USE_GCC
                sleep(1);
            #endif
			break;
		case MEDIAN_DISTANCE:
			findkBestLocatedNodesNotInSets_Median(global_g, nodeSearchSet, relativeV, excludeSet, newWCN, k);
			break;
		default:
			cerr << WRITE_FUNCTION << "No support for " << algo << endl;
			ASSERTING(0);
			exit(0);
			break;
	}
}

//TODO delay constrained WCNs
void kMedianNodes(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k)
{
	multimap<double, int> vertMMap;
	VertexSet::iterator it = nodeSearchSet.begin(), it_end = nodeSearchSet.end();
	for( ; it != it_end; ++it)
	{
		double sumWeight = 0;
		VertexSet::const_iterator vit = relativeSet.begin(), vit_end = relativeSet.end();
		for( ; vit != vit_end; ++vit)
		{
			if(*it == *vit) continue;
			
			pair<edge_descriptorN, bool> ep = edge(*it, *vit, global_g);
			ASSERTING(ep.second);
			sumWeight = sumWeight + global_g[ep.first].weight;			
		}
		
		vertMMap.insert(pair<double, int>(sumWeight, *it));
	}

	multimap<double, int>::iterator mmit = vertMMap.begin(), mmit_end = vertMMap.end();
	for( ; mmit != mmit_end && k > 0; ++mmit)
	{
		if(excludeV.contains(mmit->second)) continue;

		k--;
		vSet.insert(mmit->second);
	}
}


/*-----------------------------------------------------------------------
		k Center Nodes (lowest eccentricities)
------------------------------------------------------------------------- */
void kCenterNodes(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapEccId;

	ASSERTING(!relativeSet.empty());

	// -> full mesh of shortest paths
	if(num_edges(g) == ( (nodeSearchSet.size() * (nodeSearchSet.size() - 1))/2) )
	{
		for(VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end(); vit != vit_end; ++vit)
		{
			// worst case distance from *vit to any node in the mesh
			double worst_case_distance = 0;
			for(VertexSet::iterator vit_in = relativeSet.begin(), vit_in_end = relativeSet.end(); vit_in != vit_in_end; ++vit_in)
			{
				if(*vit == *vit_in) continue;

				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(wmap[ep.first] > worst_case_distance) 
					worst_case_distance = wmap[ep.first];
			}

			mapEccId.insert(pair<double, int>(worst_case_distance, *vit));
		}

		if(!mapEccId.empty())
		{
			int i = 0;
			for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++it)
			{
				//if(excludeV.contains(it->second)) continue;
				vSet.insert(it->second);
				i++;
			}
		}
		ASSERTING(!mapEccId.empty());

		//cerr << WRITE_FUNCTION << " core Set : " << vSet << endl;
		return;
	}

	DistanceVector 	distance(num_vertices(g));
	DistanceVector 	hops(num_vertices(g));
	ParentVector	parent(num_vertices(g));

	VertexSet::const_iterator vit, vit_end;
	for(vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end(); vit != vit_end; ++vit)
	{
		//if(excludeV.contains(*vit)) continue;
		// worst case distance from *vit to any node in the mesh
		double worstcase_dist = 0;
		if(getOutDegree(g, *vit) > 0)
			worstcase_dist = GraphAlgorithms::eccentricity_distance(g, relativeSet, *vit, distance, hops, parent);
	
		mapEccId.insert(pair<double, int>(worstcase_dist, *vit));
	}

	ASSERTING(!mapEccId.empty());
	if(!mapEccId.empty())
	{
		int i = 0;
		for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++it)
		{
			if(excludeV.contains(it->second)) continue;
			
			vSet.insert(it->second);
			i++;
		}
	}
}

/*-----------------------------------------------------------------------
		k Center Nodes (lowest eccentricities)
------------------------------------------------------------------------- */
/*void kCenterNodes(const GraphN &g, const VertexSet &V, const VertexSet &excludeV, VertexSet &coreSet, int k)
{
 	dEdgeMapConst wmap = get(&EdgeProp::weight, g);
	map<double, int> mapEccId;

	// -> full mesh of shortest paths
	if(num_edges(g) == ( (V.size() * (V.size() - 1))/2) )
	{
		for(VertexSet::iterator vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
		{
			// worst case distance from *vit to any node in the mesh
			double worst_case_distance = 0;
			for(VertexSet::iterator vit_in = vit, vit_in_end = vit_end; vit_in != vit_in_end; ++vit_in)
			{
				if(*vit == *vit_in) continue;

				pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, g);
				if(wmap[ep.first] > worst_case_distance) 
					worst_case_distance = wmap[ep.first];
			}

			mapEccId.insert(pair<double, int>(worst_case_distance, *vit));
		}

		if(!mapEccId.empty())
		{
			int i = 0;
			for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
			{
				int targ = it->second;
				if(excludeV.contains(targ)) continue;

				coreSet.insert(targ);
			}
		}
		//cerr << WRITE_FUNCTION << " core Set : " << coreSet << endl;
		return;
	}

	DistanceVector 	distance(num_vertices(g));
	DistanceVector 	hops(num_vertices(g));
	ParentVector	parent(num_vertices(g));

	//cerr << WRITE_FUNCTION << " vertices : " ;
	VertexSet::const_iterator vit, vit_end;
	for(vit = V.begin(), vit_end = V.end(); vit != vit_end; ++vit)
	{
		//cerr << " " << *vit ;
		// worst case distance from *vit to any node in the mesh
		double worstcase_dist = 0;
		if(getOutDegree(g, *vit) > 0)
			worstcase_dist = GraphAlgorithms::eccentricity_distance(g, V, *vit, distance, hops, parent);
	
		mapEccId.insert(pair<double, int>(worstcase_dist, *vit));
	}

	//cerr << ":" ;
	ASSERTING(!mapEccId.empty());
	if(!mapEccId.empty())
	{
		int i = 0;
		for(map<double, int>::iterator it = mapEccId.begin(), it_end = mapEccId.end(); i < k && it != it_end; ++i, ++it)
		{
			int targ = it->second;
			if(excludeV.contains(targ)) continue;
			coreSet.insert(targ);
		}
	}
	//cerr << ":" ;
}*/

/*-----------------------------------------------------------------------
			Finds k nodes doing a random walk -> spreads
------------------------------------------------------------------------- */
void findkTailedSpreadNotInSet(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, VertexSet &vSet, int k)
{
	// find integer defining a region int reg
	int region = (int) max(3.0, floor((double) relativeSet.size()/k));
	VertexSet traverseV = relativeSet;
	int startv = -1, alternative_startv = -1;

	VertexSet remV;
	// start with arbitrary leaf node
	for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, group_g) <= 0) remV.insert(*vit);
		else if(!isVertex(group_g, *vit)) remV.insert(*vit);
		else if(startv < 0 && getOutDegree(*vit, group_g) == 1) startv = *vit;

		if(alternative_startv < 0 && getOutDegree(*vit, group_g) >= 1) alternative_startv = *vit;
	}

	if(startv < 0) startv = alternative_startv;

	// -- debug --
	ASSERTING(remV.size() <= 1);
	ASSERTING(!traverseV.empty());
	ASSERTING(startv >= 0);
	//cerr << WRITE_FUNCTION << " k: " << k << " V size " << V.size() << " region " << region << endl; //" V " << V << " totalV " << totalV << " remV " << remV << endl;
	//dumpGraph(group_g); cerr << " region " << region << endl;cerr << "start v " << startv << endl;
	// -- debug end --
	
	traverseV = traverseV - remV;

	int numberofedges = num_edges(group_g);
	//while(!traverseV.empty() && numberofedges > 3)
	while(!traverseV.empty() && numberofedges > 3 && (int)vSet.size() < k)
	{
		VertexSet markedV, parseV;
		vector<int> interV;
		vector<int> prevV(num_vertices(group_g));
		int src = startv, targ = startv;

		markedV.insert(startv);
		parseV = markedV;
		interV.push_back(startv);
		prevV[startv] = startv;
					
		// start to traverse group_g
		while((int)markedV.size() < region && (int)(traverseV - markedV).size() > 0)
		{
			//cerr << "Parse V " << parseV << endl;
			VertexSet tempV;
			for(VertexSet::iterator vit = parseV.begin(), vit_end = parseV.end(); vit != vit_end  && ((markedV.size() + tempV.size()) < region); ++vit)
			{
				src = *vit;
				//cerr << "Out-degree(" << src << ") == " << getOutDegree(src, group_g) << " prev " << prevV[src] << endl;
				ASSERTING(getOutDegree(src, group_g) > 0);

				out_edge_iteratorN oit, oit_end;
                for(boost::tuples::tie(oit, oit_end) = out_edges(src, group_g); oit != oit_end && (markedV.size() + tempV.size()) < region; ++oit)
				{
					targ = target(*oit, group_g);	
					ASSERTING(targ != src );
				
					if(prevV[targ] == targ)
					{
						//cerr << targ << " == " << prevV[targ] << " next " << endl;  
						continue;
					}
					//cerr << "Edge " << *oit << " targ = " << targ << endl;
				
					int v = -1;
					// -- is target already traversed then check out-edges for untraversed vertices -- 
					if(!traverseV.contains(targ) && getOutDegree(targ, group_g) > 2)
					{
						out_edge_iteratorN oit, oit_end;
						//cerr << "Target " << targ << " already traversed. Checking out-edges : " << endl;
                        for(boost::tuples::tie(oit, oit_end) = out_edges(targ, group_g); oit != oit_end; ++oit)
						{
							//cerr << *oit  << " " ;
							int u = target(*oit, group_g);
							ASSERTING(targ != u);
					
							if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u) && u != src && u != prevV[u]) 
							{
								v = u;  
								break;
							}
						}
						//cerr << endl;
					}
					// -- end target already traversed --

					// found untraversed vertex?
					if(v >= 0)
					{
						//cerr << "Found untraversed neighbor of " << targ << ": " << v << endl;

						// insert intersection vertex (already traversed)
						tempV.insert(targ);
						interV.push_back(targ);
						prevV[targ] = src;
						
						// insert new v
						tempV.insert(v);
						interV.push_back(v);
						prevV[v] = targ;
						
						//cerr << "inserting " << v << " tempV " << tempV << endl;
					}
					else if(traverseV.contains(targ) && !markedV.contains(targ) && !tempV.contains(targ)) 
					{
						tempV.insert(targ);
						interV.push_back(targ);
						prevV[targ] = src;

						//cerr << "inserting " << targ << " tempV " << tempV << endl;
					}
				}
				// -- end out edges src -- 
			}
			
		//	cerr << "tempV " << tempV << " markedV " << markedV << endl;
			if(tempV.size())
			{
				markedV = markedV + tempV;
				parseV = tempV;
		//		cerr << "Updated markedV " << markedV << endl;
			}
			else
			{
		//		cerr << "Couldn't find any more " << endl;
				break;
			}
			//char s = getchar();
		}

		//cerr << "Marked V " << markedV << endl;
		
		// when traversed region hops -> find WCN and mark traversed vertices
		if((int)markedV.size() >= region) // find one WCN for markedV
		{
			kMedianNodes(global_g, nodeSearchSet, markedV, vSet, vSet, 1);
			//cerr << "Searched for new WCN. vSet : " << vSet << endl;
		}
		
		traverseV = traverseV - markedV;
		//cerr << "remaining traverseV size " << traverseV.size() << endl;
		if((int)traverseV.size() < region) break;
		
		VertexSet remV;
		for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
		{
			startv = *vit; 
			if(getOutDegree(startv, group_g) > 0) break;
			else remV.insert(startv);
		}
		//cerr << " finding startv " << startv << endl;
		ASSERTING(getOutDegree(startv, group_g) > 0);

		traverseV = traverseV - remV;
	}

	if((int)vSet.size() < k)
	{
		//cerr << WRITE_FUNCTION << "Found " << k - vSet.size() << " too few -> searching" << endl;
		kMedianNodes(global_g, nodeSearchSet, relativeSet, vSet, vSet, k - vSet.size());
	}
	//cerr << WRITE_FUNCTION << " found WCNs " << vSet << endl;
	//char e = getchar();
}

void findkTailedSpreadNotInSets(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k)
{
	// find integer defining a region int reg
	int region = (int) max(3.0, floor((double) relativeSet.size()/k));
	VertexSet traverseV = relativeSet, steinerV = excludeV - relativeSet;
	int startv = -1, alternative_startv = -1;

	VertexSet remV;
	// start with arbitrary leaf node
	for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, group_g) <= 0) remV.insert(*vit);
		else if(!isVertex(group_g, *vit)) remV.insert(*vit);
		else if(startv < 0 && getOutDegree(*vit, group_g) == 1) startv = *vit;

		if(alternative_startv < 0 && getOutDegree(*vit, group_g) >= 1) alternative_startv = *vit;
	}

	if(startv < 0) startv = alternative_startv;

	// -- debug --
	ASSERTING(remV.size() <= 1);
	ASSERTING(startv >= 0);
	//cerr << WRITE_FUNCTION << " k: " << k << " relativeV size " << relativeSet.size() << " relativeV " << relativeSet << " excludeV " << excludeV << " steiner V " << steinerV << " remV " << remV << endl;
	//dumpGraph(group_g);
	//cerr << " region " << region << endl;
	//cerr << "start v " << startv << endl;
	// -- debug end --
	
	traverseV = traverseV - remV;
	
	int numberofedges = num_edges(group_g);
	while(!traverseV.empty() && numberofedges > 3 && (int)vSet.size() < k)
	{
		VertexSet markedV, parseV;
		vector<int> interV;
		vector<int> prevV(num_vertices(group_g));
		int src = startv, targ = startv;

		markedV.insert(startv);
		parseV = markedV;
		interV.push_back(startv);
		prevV[startv] = startv;
					
		// start to traverse group_g
		while((int)(markedV - steinerV).size() < region && (int)(traverseV - markedV).size() > 0)
		{
			//cerr << "Parse V " << parseV << endl;
			VertexSet tempV;
			for(VertexSet::iterator vit = parseV.begin(), vit_end = parseV.end(); vit != vit_end  && ((markedV.size() + (tempV - steinerV).size()) < region); ++vit)
			{
				src = *vit;
				//cerr << "Out-degree(" << src << ") == " << getOutDegree(src, group_g) << " prev " << prevV[src] << endl;
				ASSERTING(getOutDegree(src, group_g) > 0);

				out_edge_iteratorN oit, oit_end;
                for(boost::tuples::tie(oit, oit_end) = out_edges(src, group_g); oit != oit_end && (markedV.size() + (tempV - steinerV).size()) < region; ++oit)
				{
					targ = target(*oit, group_g);	
					ASSERTING(targ != src );
				
					if(prevV[targ] == targ)
					{
						//cerr << targ << " == " << prevV[targ] << " next " << endl;  
						continue;
					}
					//cerr << "Edge " << *oit << " targ = " << targ << endl;
				
					int v = -1;
					// -- is target already traversed then check out-edges for untraversed vertices -- 
					if(!traverseV.contains(targ) && getOutDegree(targ, group_g) > 2)
					{
						out_edge_iteratorN oit, oit_end;
						//cerr << "Target " << targ << " already traversed. Checking out-edges : " << endl;
                        for(boost::tuples::tie(oit, oit_end) = out_edges(targ, group_g); oit != oit_end; ++oit)
						{
							//cerr << *oit  << " " ;
							int u = target(*oit, group_g);
							ASSERTING(targ != u);
					
							if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u) && u != src && u != prevV[u]) 
							{
								v = u;  
								break;
							}
						}
						//cerr << endl;
					}
					// -- end target already traversed --

					// found untraversed vertex?
					if(v >= 0)
					{
						//cerr << "Found untraversed neighbor of " << targ << ": " << v << endl;

						// insert intersection vertex (already traversed)
						tempV.insert(targ);
						interV.push_back(targ);
						prevV[targ] = src;
						
						// insert new v
						tempV.insert(v);
						interV.push_back(v);
						prevV[v] = targ;
						
						//cerr << "inserting " << v << " tempV " << tempV << endl;
					}
					else if(traverseV.contains(targ) && !markedV.contains(targ) && !tempV.contains(targ)) 
					{
						tempV.insert(targ);
						interV.push_back(targ);
						prevV[targ] = src;

						//cerr << "inserting " << targ << " tempV " << tempV << endl;
					}
				}
				// -- end out edges src -- 
			}
			
			//cerr << "tempV " << tempV << " markedV " << markedV << endl;
			if(tempV.size())
			{
				markedV = markedV + tempV;
				parseV = tempV;
				//cerr << "Updated markedV " << markedV << endl;
			}
			else
			{
				//cerr << "Couldn't find any more " << endl;
				break;
			}
			//char s = getchar();
		}

		//cerr << "Marked V " << markedV << endl;
		
		// when traversed region hops -> find WCN and mark traversed vertices
		if((int)markedV.size() >= region) // find one WCN for markedV
		{
			kMedianNodes(global_g, nodeSearchSet, markedV, excludeV + vSet, vSet, 1);
			//cerr << "Searched for new WCN. vSet : " << vSet << endl;
		}
		
		traverseV = traverseV - markedV;
		//cerr << "remaining traverseV size " << traverseV.size() << endl;
		if((int)traverseV.size() < region) break;
		
		VertexSet remV;
		for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
		{
			startv = *vit; 
			if(getOutDegree(startv, group_g) > 0) break;
			else remV.insert(startv);
		}
		//cerr << " finding startv " << startv << endl;
		ASSERTING(getOutDegree(startv, group_g) > 0);

		traverseV = traverseV - remV;
	}

	if((int)vSet.size() < k)
	{
		//cerr << "Found " << k - vSet.size() << " too few -> searching" << endl;
		kMedianNodes(global_g, nodeSearchSet, relativeSet, excludeV + vSet, vSet, k - vSet.size());
	}
						
	//cerr << WRITE_FUNCTION << " found WCNs " << vSet << endl;
	//char e = getchar();
}
/*-----------------------------------------------------------------------
				Random walk -> like a snake
------------------------------------------------------------------------- */
void findkTailedSpreadNotInSets_snake(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &excludeV, VertexSet &vSet, int k)
{
	//cerr << WRITE_FUNCTION << " k: " << k << " V size " << relativeSet.size() << " nodeSearchSet size " << nodeSearchSet.size() << endl;
	//dumpGraph(group_g);
	//char c = getchar();

	// find integer defining a region int reg
	int region = (int) max(3.0, floor((double) relativeSet.size()/k));
	VertexSet traverseV = relativeSet, steinerV = excludeV - relativeSet;
	int startv = -1, alternative_startv = -1;

	// start with arbitrary leaf node
	for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
	{
		if(startv < 0 && getOutDegree(*vit, group_g) == 1) startv = *vit;
		if(alternative_startv < 0 && getOutDegree(*vit, group_g) >= 1) alternative_startv = *vit;
	}
	if(startv < 0) startv = alternative_startv;
	ASSERTING(startv >= 0);

	//cerr << " region " << region << endl;
	//cerr << "start v " << startv << endl;
	int numberofedges = num_edges(group_g);
	while(!traverseV.empty() && numberofedges > 3 && (int)vSet.size() < k)
	{
		VertexSet markedV;
		vector<int> interV; // intersection vertices == out-degree > 2
		markedV.insert(startv);
		interV.push_back(startv);
		
		int src = startv, prev = startv, targ = startv;

		// start to traverse group_g
		while((int)(markedV - steinerV).size() < region && (int)(traverseV - markedV).size() > 0)
		{
			//cerr << "Out-degree(" << src << ") == " << getOutDegree(src, group_g) << " prev = " << prev << endl;
			
			VertexSet tempV;
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(src, group_g); oit != oit_end && (markedV.size() + (tempV - steinerV).size()) < region; ++oit)
			{
				targ = target(*oit, group_g);	
				ASSERTING(targ != src );
				if(targ == prev) 
				{
					//cerr << targ << " == " << prev << " next " << endl;
					continue;
				}
				
				//cerr << "Edge " << *oit << " targ = " << targ << endl;
			
				// -- is target already traversed then check out-edges for untraversed vertices -- 
				if((!traverseV.contains(targ) || targ == prev) && getOutDegree(targ, group_g) > 2)
				{
					int v = -1;
					out_edge_iteratorN oit, oit_end;
					//cerr << "Target " << targ << " already traversed. Checking out-edges : " << endl;
                    for(boost::tuples::tie(oit, oit_end) = out_edges(targ, group_g); oit != oit_end; ++oit)
					{
						//cerr << *oit  << " " ;
						int u = target(*oit, group_g);
						ASSERTING(targ != u);
					
						if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u) && u != src && u != prev) 
						{
							v = u;
							break;
						}
					}
					//cerr << endl;
				
					// found untraversed vertex?
					if(v >= 0)
					{
						//cerr << "Found untraversed neighbor of " << targ << ": " << v << endl;
						tempV.insert(targ);
						interV.push_back(targ);
						src = targ;
						targ = v;
					}
					else 
					{
						//cerr << "No untraversed neighbors of " << targ << endl;
						continue; 
					}
				}
				// -- end target already traversed --
				
				if(targ != prev && traverseV.contains(targ) && !markedV.contains(targ) && !tempV.contains(targ)) 
				{
					tempV.insert(targ);
					interV.push_back(targ);
					//cerr << "inserting " << targ << " tempV " << tempV << endl;
					if(getOutDegree(targ, group_g) > 1) break;
				}
			}
			// -- end out edges src -- 
			
			//cerr << "tempV " << tempV << " markedV " << markedV << endl;
			
			// at this point I should have found a new valid target if there is one
			if(!traverseV.contains(targ) || (markedV.contains(targ) && getOutDegree(targ, group_g) <= 1)) 
			{
				//cerr << "Trying backtracking " << endl;
				int new_src = -1, new_targ = -1;
				// check if I can start again from previous traversed vertex and walk in other directions
				for(vector<int>::reverse_iterator it = interV.rbegin(), it_end = interV.rend(); it != it_end && new_src < 0; ++it)
				{
					//cerr << *it << " out degree " << getOutDegree(*it, group_g) << endl;
                    for(boost::tuples::tie(oit, oit_end) = out_edges(*it, group_g); oit != oit_end; ++oit)
					{
						int u = target(*oit, group_g);
						if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u)) // found untraversed targ
						{
							//cerr << "Found untraversed neighbor of " << *it << ": " << u << endl;
							new_targ = u;
							new_src = *it;
							break;
						}
					}
				}
				
				if(new_targ >= 0)
				{
					ASSERTING(!markedV.contains(new_targ));
					ASSERTING(markedV.contains(new_src) || tempV.contains(new_src));
					ASSERTING(!tempV.contains(new_targ));

					markedV.insert(new_targ);
					interV.push_back(new_targ);
					//cerr << "inserting " << new_targ << " markedV " << markedV << endl;

					prev = new_src;
					src = new_targ;
					//cerr << "Backtracked: prev " << prev << " src " << src << endl;
				}
				else
				{
					//cerr << "Failed. Already traversed targ " << targ << " end markedV " << markedV << endl;
					break;
				}
			}
			else
			{
				prev = src;
				src = targ;
				//cerr << "New: prev " << prev << " src " << src << endl;
			}

			if(tempV.size())
			{
				markedV = markedV + tempV;
				//cerr << "Updated markedV " << markedV << endl;
			}

			if(prev == src) break;
			//char s = getchar();
		}

		//cerr << "Marked V " << markedV << endl;
		// when traversed region hops -> find WCN and mark traversed vertices
		if((int)markedV.size() >= region) // find one WCN for markedV
		{
			kMedianNodes(global_g, nodeSearchSet, markedV, excludeV + vSet, vSet, 1);
			//cerr << "Searched for new WCN. vSet : " << vSet << endl;
		}
		
		traverseV = traverseV - markedV;
		//cerr << "remaining traverseV size " << traverseV.size() << endl;

		if(traverseV.size() <= 0) break;
		
		startv = *(traverseV.begin());
		//cerr << "New startv " << startv << endl;
		//char d = getchar();
	}

	if((int)vSet.size() < k)
	{
		//cerr << WRITE_FUNCTION << "Found " << k - vSet.size() << " too few -> searching" << endl;
		kMedianNodes(global_g, nodeSearchSet, relativeSet, excludeV + vSet, vSet, 1);
	}
	//else cerr << WRITE_FUNCTION << "Found enough " << endl;
						
	//cerr << WRITE_FUNCTION << endl;
	//cerr << " found WCNs " << vSet << endl;
	//char e = getchar();
}

/*-----------------------------------------------------------------------
				Random walk -> like a snake
------------------------------------------------------------------------- */
void findkTailedSpreadNotInSet_snake(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, VertexSet &vSet, int k)
{
	//cerr << WRITE_FUNCTION << " k: " << k << " V size " << V.size() << endl;
	//dumpGraph(group_g);
	//char c = getchar();

	// find integer defining a region int reg
	int region = (int) max(3.0, floor((double) relativeSet.size()/k));
	VertexSet traverseV = relativeSet;
	int startv = -1, alternative_startv = -1;

	// start with arbitrary leaf node
	for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
	{
		if(startv < 0 && getOutDegree(*vit, group_g) == 1) startv = *vit;
		if(alternative_startv < 0 && getOutDegree(*vit, group_g) >= 1) alternative_startv = *vit;
	}
	if(startv < 0) startv = alternative_startv;
	ASSERTING(startv >= 0);

	//cerr << "start v " << startv << endl;
	int numberofedges = num_edges(group_g);
	//while(!traverseV.empty() && numberofedges > 3)
	while(!traverseV.empty() && numberofedges > 3 && (int)vSet.size() < k)
	{
		VertexSet markedV;
		vector<int> interV; // intersection vertices == out-degree > 2
		markedV.insert(startv);
		interV.push_back(startv);
		
		int src = startv, prev = startv, targ = startv;

		// start to traverse group_g
		while((int)markedV.size() < region && (int)(traverseV - markedV).size() > 0)
		{
			//cerr << "Out-degree(" << src << ") == " << getOutDegree(src, group_g) << " prev = " << prev << endl;
			
			VertexSet tempV;
			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(src, group_g); oit != oit_end && (markedV.size() + tempV.size()) < region; ++oit)
			{
				targ = target(*oit, group_g);	
				ASSERTING(targ != src );
				if(targ == prev) 
				{
					//cerr << targ << " == " << prev << " next " << endl;
					continue;
				}
				
				//cerr << "Edge " << *oit << " targ = " << targ << endl;
			
				// -- is target already traversed then check out-edges for untraversed vertices -- 
				if((!traverseV.contains(targ) || targ == prev) && getOutDegree(targ, group_g) > 2)
				{
					int v = -1;
					out_edge_iteratorN oit, oit_end;
					//cerr << "Target " << targ << " already traversed. Checking out-edges : " << endl;
                    for(boost::tuples::tie(oit, oit_end) = out_edges(targ, group_g); oit != oit_end; ++oit)
					{
						//cerr << *oit  << " " ;
						int u = target(*oit, group_g);
						ASSERTING(targ != u);
					
						if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u) && u != src && u != prev) 
						{
							v = u;
							break;
						}
					}
					//cerr << endl;
				
					// found untraversed vertex?
					if(v >= 0)
					{
						//cerr << "Found untraversed neighbor of " << targ << ": " << v << endl;
						tempV.insert(targ);
						interV.push_back(targ);
						src = targ;
						targ = v;
					}
					else 
					{
						//cerr << "No untraversed neighbors of " << targ << endl;
						continue; 
					}
				}
				// -- end target already traversed --
				
				if(targ != prev && traverseV.contains(targ) && !markedV.contains(targ) && !tempV.contains(targ)) 
				{
					tempV.insert(targ);
					interV.push_back(targ);
					//cerr << "inserting " << targ << " tempV " << tempV << endl;
					if(getOutDegree(targ, group_g) > 1) break;
				}
			}
			// -- end out edges src -- 
			
			//cerr << "tempV " << tempV << " markedV " << markedV << endl;
			
			// at this point I should have found a new valid target if there is one
			if(!traverseV.contains(targ) || (markedV.contains(targ) && getOutDegree(targ, group_g) <= 1)) 
			{
				//cerr << "Trying backtracking " << endl;
				int new_src = -1, new_targ = -1;
				// check if I can start again from previous traversed vertex and walk in other directions
				for(vector<int>::reverse_iterator it = interV.rbegin(), it_end = interV.rend(); it != it_end && new_src < 0; ++it)
				{
					//cerr << *it << " out degree " << getOutDegree(*it, group_g) << endl;
                    for(boost::tuples::tie(oit, oit_end) = out_edges(*it, group_g); oit != oit_end; ++oit)
					{
						int u = target(*oit, group_g);
						if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u)) // found untraversed targ
						{
							//cerr << "Found untraversed neighbor of " << *it << ": " << u << endl;
							new_targ = u;
							new_src = *it;
							break;
						}
					}
				}
				
				if(new_targ >= 0)
				{
					ASSERTING(!markedV.contains(new_targ));
					ASSERTING(markedV.contains(new_src) || tempV.contains(new_src));
					ASSERTING(!tempV.contains(new_targ));

					markedV.insert(new_targ);
					interV.push_back(new_targ);
					//cerr << "inserting " << new_targ << " markedV " << markedV << endl;

					prev = new_src;
					src = new_targ;
					//cerr << "Backtracked: prev " << prev << " src " << src << endl;
				}
				else
				{
					//cerr << "Failed. Already traversed targ " << targ << " end markedV " << markedV << endl;
					break;
				}
			}
			else
			{
				prev = src;
				src = targ;
				//cerr << "New: prev " << prev << " src " << src << endl;
			}

			if(tempV.size())
			{
				markedV = markedV + tempV;
				//cerr << "Updated markedV " << markedV << endl;
			}
			if(prev == src) break;
			//char s = getchar();
		}

		//cerr << "Marked V " << markedV << endl;
		// when traversed region hops -> find WCN and mark traversed vertices
		if((int)markedV.size() >= region) // find one WCN for markedV
		{
			kMedianNodes(global_g, nodeSearchSet, markedV, vSet, vSet, 1);
			//cerr << "Searched for new WCN. vSet : " << vSet << endl;
		}
		
		traverseV = traverseV - markedV;
		//cerr << "remaining traverseV size " << traverseV.size() << endl;

		if(traverseV.size() <= 0) break;
		
		startv = *(traverseV.begin());
		//cerr << "New startv " << startv << endl;
		//char d = getchar();
	}

	if((int)vSet.size() < k)
	{
		//cerr << WRITE_FUNCTION << "Found " << k - vSet.size() << " too few -> searching" << endl;
		kMedianNodes(global_g, nodeSearchSet, relativeSet, vSet, vSet, 1);
	}
						
	//cerr << WRITE_FUNCTION << endl;
	//cerr << " found WCNs " << vSet << endl;
	//char e = getchar();
}

/*-----------------------------------------------------------------------
		Finds k WCNs tailed selection from the node layout:
			- Divides rectangle to rectangles and choose
			  nodes from the center of the rectangles
------------------------------------------------------------------------- */
void findkNodesTailedSpreadNotInSet(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &V, VertexSet &vertexSet, int k)
{
	//cerr << WRITE_FUNCTION << endl;
	multimap<int, Rectangle> mapRect;
	multimap<double, int> arrCoord;
	VertexSet vSet;

	// first round : arrange all vertices according to xpos
	for(VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end(); vit != vit_end; ++vit)
	{
		vSet.insert(*vit);
		arrCoord.insert(pair<double, int>(g[*vit].xpos, *vit));
	}
	
	Rectangle rect(pair<double, double>(1000,0), pair<double, double>(0, 1000), vSet, arrCoord, true);
	
	// mapRect.size() == 1
	mapRect.insert(pair<int, Rectangle>(vSet.size(), rect));
	
	while((int)vertexSet.size() < k)
	{
		VertexSet split;
		multimap<double, int>::iterator it, it_end;
		int newWCN;
		
		Rectangle max = (--(mapRect.end()))->second;
	   	findClosestVertXY(V + vertexSet, max, split, newWCN);
		vertexSet.insert(newWCN);

		// -- debug --
		/*cerr << "MapRect size " << mapRect.size()  << endl;
		for(multimap<int, Rectangle>::iterator mmit = mapRect.begin(), mmit_end = mapRect.end(); mmit != mmit_end; ++mmit) cerr << " sz " << mmit->first ;
		cerr << endl;
	    cerr << " max Coordinates : " << max.C1 << max.C2 << " arrCoord.size() " << max.arrCoord.size() << " vSet.size() " << max.vSet.size() << endl;
		cerr << "Inserting WCN " << newWCN << endl;
		cerr << "Split.size() " << split.size() << endl;
		cerr << "max.arrX " << max.arrX << endl;*/
		// -- debug end --

		Rectangle newRect1, newRect2;

		bool newArrX;
	    if(max.arrX) newArrX = false;
		else newArrX = true;
		
		ASSERTING(max.arrX != newArrX);

		// Init new rectangle
		newRect1.vSet = split - vertexSet;
		newRect1.arrX = newArrX;
		if(max.arrX) 
		{
			newRect1.C1 = pair<double, double>(g[newWCN].xpos, max.C1.second);
			newRect1.C2 = max.C2;
		}
		else // !max.arrX
		{
			newRect1.C2 = pair<double, double>(max.C2.first, g[newWCN].ypos);
			newRect1.C1 = max.C1;
		}
		makeSortedCoord(g, newRect1); 
		// Init end

		// -- debug --
		//VertexSet debugSet;
		//for( it = newRect1.arrCoord.begin(), it_end = newRect1.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//cerr << "Diff " << debugSet - newRect1.vSet << newRect1.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect1.vSet);
		// -- debug end --
		
		// Init new rectangle
		newRect2.vSet = max.vSet - split;
		newRect2.arrX = newArrX;
		if(max.arrX) 
		{
			newRect2.C2 = pair<double, double>(g[newWCN].xpos, max.C2.second);
			newRect2.C1 = max.C1;
		}
		else // !max.arrX
		{
			newRect2.C1 = pair<double, double>(max.C1.first, g[newWCN].ypos);
			newRect2.C2 = max.C2;
		}
		makeSortedCoord(g, newRect2); 
		// Init end

		// -- debug --
		//debugSet.clear();
		//for( it = newRect2.arrCoord.begin(), it_end = newRect2.arrCoord.end(); it != it_end; ++it)debugSet.insert(it->second);
		//cerr << "Diff " << debugSet - newRect2.vSet << newRect2.vSet - debugSet << endl;
		//ASSERTING(debugSet == newRect2.vSet);
		/*cerr << "New Rect1: " << newRect1.C1 << newRect1.C2 << endl;
		cerr << "New Rect2: " << newRect2.C1 << newRect2.C2 << endl;
		cerr << "Rect1 arrCoord.size() " << newRect1.arrCoord.size() << " vSet.size() " << newRect1.vSet.size() << endl;
		cerr << "Rect2 arrCoord.size() " << newRect2.arrCoord.size() << " vSet.size() " << newRect2.vSet.size() << endl;
		cerr << "Current steiner set size() " << vertexSet.size() << endl;
		char d = getchar();*/
		// -- debug end --

		removeRectangle(mapRect, max);

		mapRect.insert(pair<int, Rectangle>(newRect1.vSet.size(), newRect1));
		mapRect.insert(pair<int, Rectangle>(newRect2.vSet.size(), newRect2));
	}

	//cerr << WRITE_FUNCTION << " steiner set " << vertexSet << endl;
	//char c = getchar();
}

void findkTailedSpreadNotInSets(const GraphN &global_g, const GraphN &group_g, const VertexSet &nodeSearchSet, const VertexSet &relativeSet, const VertexSet &V, VertexSet &vSet, int k, CoreSelectionAlgo algo)
{
	// find integer defining a region int reg
	int region = (int) max(3.0, floor((double) V.size()/k));
	VertexSet traverseV = V, steinerV = relativeSet - V;
	int startv = -1, alternative_startv = -1;

	VertexSet remV;
	// start with arbitrary leaf node
	for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
	{
		if(getOutDegree(*vit, group_g) <= 0) remV.insert(*vit);
		else if(!isVertex(group_g, *vit)) remV.insert(*vit);
		else if(startv < 0 && getOutDegree(*vit, group_g) == 1) startv = *vit;

		if(alternative_startv < 0 && getOutDegree(*vit, group_g) >= 1) alternative_startv = *vit;
	}

	if(startv < 0) startv = alternative_startv;

	// -- debug --
	ASSERTING(remV.size() <= 1);
	ASSERTING(startv >= 0);
	//cerr << WRITE_FUNCTION << " k: " << k << " V size " << V.size() << " V " << V << " relativeSet " << relativeSet << " steiner V " << steinerV << " remV " << remV << endl;
	//dumpGraph(group_g);
	//cerr << " region " << region << endl;
	//cerr << "start v " << startv << endl;
	// -- debug end --
	
	traverseV = traverseV - remV;

	while(!traverseV.empty())
	{
		VertexSet markedV;
		markedV.insert(startv);
		
		switch(algo)
		{
		//	case RANDOM_BROADCAST_WALK_SELECTION:
		//		random_broadcast_walk(startv, group_g, steinerV, markedV);
		//		break;
			default:
				random_broadcast_walk(group_g, traverseV, startv, region, steinerV, markedV);
				break;
		}

		//cerr << "Marked V " << markedV << endl;
		
		// when traversed region hops -> find WCN and mark traversed vertices
		if((int)markedV.size() >= region) // find one WCN for markedV
		{
			kMedianNodes(global_g, nodeSearchSet, markedV, relativeSet + vSet, vSet, 1);
			//cerr << "Searched for new WCN. vSet : " << vSet << endl;
		}
		
		traverseV = traverseV - markedV;
		//cerr << "remaining traverseV size " << traverseV.size() << endl;
		if((int)traverseV.size() < region) break;
		
		VertexSet remV;
		for(VertexSet::iterator vit = traverseV.begin(), vit_end = traverseV.end(); vit != vit_end; ++vit)
		{
			startv = *vit; 
			if(getOutDegree(startv, group_g) > 0) break;
			else remV.insert(startv);
		}
		//cerr << " finding startv " << startv << endl;
		ASSERTING(getOutDegree(startv, group_g) > 0);

		traverseV = traverseV - remV;
	}

	if((int)vSet.size() < k)
	{
		//cerr << "Found " << k - vSet.size() << " too few -> searching" << endl;
		kMedianNodes(global_g, nodeSearchSet, relativeSet, relativeSet + vSet, vSet, k - vSet.size());
	}
						
	//cerr << WRITE_FUNCTION << " found WCNs " << vSet << endl;
	//char e = getchar();
}

void random_broadcast_walk(const GraphN &group_g, const VertexSet &traverseV, const int &startv, const int &region, const VertexSet &steinerV, VertexSet &markedV)
{
	VertexSet parseV;
	vector<int> interV;
	vector<int> prevV(num_vertices(group_g));
	int src = -1, targ = -1;

	parseV = markedV;
	interV.push_back(startv);
	prevV[startv] = startv;

	// start to traverse group_g
	while((int)(markedV - steinerV).size() < region && (int)(traverseV - markedV).size() > 0)
	{
		//cerr << "Parse V " << parseV << endl;
		VertexSet tempV;
		for(VertexSet::iterator vit = parseV.begin(), vit_end = parseV.end(); vit != vit_end  && ((markedV.size() + (tempV - steinerV).size()) < region); ++vit)
		{
			src = *vit;
			//cerr << "Out-degree(" << src << ") == " << getOutDegree(src, group_g) << " prev " << prevV[src] << endl;
			ASSERTING(getOutDegree(src, group_g) > 0);

			out_edge_iteratorN oit, oit_end;
            for(boost::tuples::tie(oit, oit_end) = out_edges(src, group_g); oit != oit_end && (markedV.size() + (tempV - steinerV).size()) < region; ++oit)
			{
				targ = target(*oit, group_g);	
				ASSERTING(targ != src );
			
				if(prevV[targ] == targ)
				{
					//cerr << targ << " == " << prevV[targ] << " next " << endl;  
					continue;
				}
				//cerr << "Edge " << *oit << " targ = " << targ << endl;
			
				int v = -1;
				// -- is target already traversed then check out-edges for untraversed vertices -- 
				if(!traverseV.contains(targ) && getOutDegree(targ, group_g) > 2)
				{
					out_edge_iteratorN oit, oit_end;
					//cerr << "Target " << targ << " already traversed. Checking out-edges : " << endl;
                    for(boost::tuples::tie(oit, oit_end) = out_edges(targ, group_g); oit != oit_end; ++oit)
					{
						//cerr << *oit  << " " ;
						int u = target(*oit, group_g);
						ASSERTING(targ != u);
				
						if(traverseV.contains(u) && !markedV.contains(u) && !tempV.contains(u) && u != src && u != prevV[u]) 
						{
							v = u;  
							break;
						}
					}
					//cerr << endl;
				}
				// -- end target already traversed --

				// found untraversed vertex?
				if(v >= 0)
				{
					//cerr << "Found untraversed neighbor of " << targ << ": " << v << endl;

					// insert intersection vertex (already traversed)
					tempV.insert(targ);
					interV.push_back(targ);
					prevV[targ] = src;
					
					// insert new v
					tempV.insert(v);
					interV.push_back(v);
					prevV[v] = targ;
					
					//cerr << "inserting " << v << " tempV " << tempV << endl;
				}
				else if(traverseV.contains(targ) && !markedV.contains(targ) && !tempV.contains(targ)) 
				{
					tempV.insert(targ);
					interV.push_back(targ);
					prevV[targ] = src;

					//cerr << "inserting " << targ << " tempV " << tempV << endl;
				}
			}
			// -- end out edges src -- 
		}
		
		//cerr << "tempV " << tempV << " markedV " << markedV << endl;
		if(tempV.size())
		{
			markedV = markedV + tempV;
			parseV = tempV;
			//cerr << "Updated markedV " << markedV << endl;
		}
		else
		{
			//cerr << "Couldn't find any more " << endl;
			break;
		}
		//char s = getchar();
	}
}
/*-----------------------------------------------------------------------
					Random WCN Selection
------------------------------------------------------------------------- */
void findkNodesNotInSet(SteinerPoints::SteinerQueue &steinerQueue, const VertexSet &vertexSet, VertexSet &vSet, int k) 
{
	vector<SteinerPoints::SteinerQueue::iterator> iteratorSet;
	
	SteinerPoints::SteinerQueue::iterator it = steinerQueue.begin(), it_end = steinerQueue.end();
	for( int i = 0; it != it_end && i < k; ++it)
	{
		if(!(vertexSet.contains(*it)))
		{
			int w = *it;
			vSet.insert(w);
			iteratorSet.push_back(it);
			i++;
		}
	}
	
	vector<SteinerPoints::SteinerQueue::iterator>::iterator sit, sit_end;
	for(sit = iteratorSet.begin(), sit_end = iteratorSet.end(); sit != sit_end; ++sit)
	{
		steinerQueue.push_back(*(*sit));
		steinerQueue.erase(*sit);
	}
}

void findkNodesNotInSets(SteinerPoints::SteinerQueue &steinerQueue, const VertexSet &totalV, const VertexSet &vertexSet, VertexSet &vSet, int k) 
{
	vector<SteinerPoints::SteinerQueue::iterator> iteratorSet;
	
	SteinerPoints::SteinerQueue::iterator it = steinerQueue.begin(), it_end = steinerQueue.end();
	for( int i = 0; it != it_end && i < k; ++it)
	{
		if(!vertexSet.contains(*it) && !totalV.contains(*it))
		{
			int w = *it;
			vSet.insert(w);
			iteratorSet.push_back(it);
			i++;
		}
	}
	
	vector<SteinerPoints::SteinerQueue::iterator>::iterator sit, sit_end;
	for(sit = iteratorSet.begin(), sit_end = iteratorSet.end(); sit != sit_end; ++sit)
	{
		steinerQueue.push_back(*(*sit));
		steinerQueue.erase(*sit);
	}
}

/*-----------------------------------------------------------------------
		Best k WCN according to average distance from members
------------------------------------------------------------------------- */
void findkBestLocatedNodesNotInSet_Average(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &excludeV, VertexSet &vSet, int k)
{
	multimap<double, int> vertMMap;
	VertexSet::iterator it = nodeSearchSet.begin(), it_end = nodeSearchSet.end();
	for( ; it != it_end; ++it)
	{
		if(excludeV.contains(*it)) continue;
		
		double sumWeight = 0;
		VertexSet::const_iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end();
		for( ; vit != vit_end; ++vit)
		{
			if(*it == *vit) continue;
			
			pair<edge_descriptorN, bool> ep = edge(*it, *vit, global_g);
			ASSERTING(ep.second);
			sumWeight = sumWeight + global_g[ep.first].weight;			
		}
		
		vertMMap.insert(pair<double, int>(sumWeight, *it));
	}

	multimap<double, int>::iterator mmit = vertMMap.begin(), mmit_end = vertMMap.end();
	for( ; mmit != mmit_end && k > 0; ++mmit, k--)
	{
		vSet.insert(mmit->second);
	}
}

void findkBestLocatedNodesNotInSets_Average(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet &excludeV, VertexSet &vSet, int k)
{
	multimap<double, int> vertMMap;
	VertexSet::iterator it = nodeSearchSet.begin(), it_end = nodeSearchSet.end();
	for( ; it != it_end; ++it)
	{
		if(totalV.contains(*it) || excludeV.contains(*it)) continue;
		
		double sumWeight = 0;
		VertexSet::const_iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end();
		for( ; vit != vit_end; ++vit)
		{
			if(*it == *vit) continue;
			
			pair<edge_descriptorN, bool> ep = edge(*it, *vit, global_g);
			ASSERTING(ep.second);
			sumWeight = sumWeight + global_g[ep.first].weight;			
		}
		
		vertMMap.insert(pair<double, int>(sumWeight, *it));
	}

	multimap<double, int>::iterator mmit = vertMMap.begin(), mmit_end = vertMMap.end();
	for( ; mmit != mmit_end && k > 0; ++mmit, k--)
	{
		vSet.insert(mmit->second);
	}
}
											// steiner set					// member-node-set			// excluded nodes		  // found nodes
/*-----------------------------------------------------------------------
		Best k WCN according to median distance from members
------------------------------------------------------------------------- */
void findkBestLocatedNodesNotInSet_Median(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &excludeV, VertexSet& vSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, global_g);
	
	VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end();
	for( ; vit != vit_end; ++vit)
	{
		if(excludeV.contains(*vit)) continue;
		multiset<double> weightSet;

		for(VertexSet::iterator vit_in = nodeSearchSet.begin(), vit_in_end = nodeSearchSet.end(); vit_in != vit_in_end; ++vit_in)
		{
			if(*vit_in == *vit) continue;

			pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, global_g);
			ASSERTING(ep.second);
			
			weightSet.insert(wmap[ep.first]);
		}
		
		int median = weightSet.size()/2;
		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if(i >= median)
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}		
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end && k > 0; ++mit, k--)
	{
		vSet.insert(mit->second);	
	}
}

void findkBestLocatedNodesNotInSets_Median(const GraphN &global_g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet &excludeV, VertexSet& vSet, int k)
{
	multimap<double, int> rankMap;
	dEdgeMapConst wmap = get(&EdgeProp::weight, global_g);
	
	VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end();
	for( ; vit != vit_end; ++vit)
	{
		if(excludeV.contains(*vit) || totalV.contains(*vit)) continue;
		multiset<double> weightSet;

		for(VertexSet::iterator vit_in = nodeSearchSet.begin(), vit_in_end = nodeSearchSet.end(); vit_in != vit_in_end; ++vit_in)
		{
			if(*vit == *vit_in) continue;

			pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, global_g);
			ASSERTING(ep.second);

			weightSet.insert(wmap[ep.first]);
		}
		
		int median = weightSet.size()/2;

		multiset<double>::iterator sit = weightSet.begin(), sit_end = weightSet.end();
		for( int i = 0; sit != sit_end; ++sit, i++)
		{
			if(i >= median)
			{
				rankMap.insert(pair<double, int>(*sit, *vit));
				break;
			}			
		}
	}
	
	multimap<double, int>::iterator mit = rankMap.begin(), mit_end = rankMap.end();
	for( ; mit != mit_end && k > 0; ++mit, k--)
	{
		vSet.insert(mit->second);	
	}
}

/*-----------------------------------------------------------------------
		Finds k WCNs with best sum of outgoing bandwidth
------------------------------------------------------------------------- */
void findkNodesBandwidthNotInSet(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet& V, VertexSet &vertexSet, int k)
{
	multimap<double, int> bwMap;
	for(VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end(); vit != vit_end; ++vit)
	{
		if(V.contains(*vit)) continue;

		double sumBW = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
			sumBW = sumBW + g[*oit].bw;
			
		bwMap.insert(pair<double, int>(sumBW, *vit));
	}
	
	multimap<double, int>::iterator mit = bwMap.begin(), mit_end = bwMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
}

void findkNodesBandwidthNotInSets(const GraphN &g, const VertexSet &nodeSearchSet, const VertexSet &totalV, const VertexSet& V, VertexSet &vertexSet, int k)
{
	multimap<double, int> bwMap;
	for(VertexSet::iterator vit = nodeSearchSet.begin(), vit_end = nodeSearchSet.end(); vit != vit_end; ++vit)
	{
		if(totalV.contains(*vit) || V.contains(*vit)) continue;

		double sumBW = 0;
		out_edge_iteratorN oit, oit_end;
        for(boost::tuples::tie(oit, oit_end) = out_edges(*vit, g); oit != oit_end; ++oit)
			sumBW = sumBW + g[*oit].bw;
			
		bwMap.insert(pair<double, int>(sumBW, *vit));
	}
	
	multimap<double, int>::iterator mit = bwMap.begin(), mit_end = bwMap.end();
	for(int i = 0; i < k && mit != mit_end; i++, ++mit)
	{
		vertexSet.insert(mit->second);	
	}
}

};

