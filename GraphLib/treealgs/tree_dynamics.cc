/***************************************************************************
                          tree_dynamics.cc  -  description
                             -------------------
    begin                : Wed Apr 26 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
 
#include "tree_dynamics.h"
#include "prim_mst.h"
#include "../simdefs.h"
#include "../graphalgs/coreh.h"
#include "../graphalgs/complete_graph.h"
#include "../graphalgs/graphalgs.h"
#include "../simtime.h"
#include <cstdlib>
#include <math.h>
#include <cmath>
 
using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
		class TreeDynamics():
			addVertex -> class InsertDynamics
			removeVertex -> class RemoveDynamics  
------------------------------------------------------------------------- */
//void TreeDynamics::insertVertex() { abort(); }
//void TreeDynamics::removeVertex() {	abort(); }

/*-----------------------------------------------------------------------
	class TreeDynamics():
		help functions for class insertDynamics(), and 
		class removeDynamics()
------------------------------------------------------------------------- */
clock_t
TreeDynamics::subTree(TreeStructure &groupT, TreeStructure &subT, VertexSet& steinerSet)
{
	int core = GraphAlgorithms::findBestLocatedMemberNode(subT);
	ASSERTING(core > -1);
	
	TreeStructure new_subT;
	clock_t exec_time = TreeAlgorithms::buildTree(subT, new_subT, GlobalSimArgs::getSimTreeAlgo(), core);

	groupT.mergeTrees(new_subT);		// merge trees
	groupT.removeUnusedVertices();		// remove unused steiner points

	// -- debug --
	//cerr << WRITE_FUNCTION << endl;
	//dumpTreeStructure(new_subT);
	// -- debug end --
	return exec_time;
}
/*-----------------------------------------------------------------------
		connectNeighbors:
------------------------------------------------------------------------- */
void
TreeDynamics::connectMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0, avInc = 2;
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	
	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeWeight = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= (getDegreeConstraint(globalG_, *vit) + degInc)) continue;
			
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);

				edge_descriptorN e = ep.first;
				double tempMinWeight = globalG_[ep.first].weight;
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) - avInc;
				if(minEdgeWeight > tempMinWeight && od > 0)
				{
					minEdgeWeight = tempMinWeight;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}
		
		// -- fail safe --
		if(connV < 0)
		{	// connectdV filled up out-degree (shouldn't happen), available out degree < 0
			if(getAvOutDegree(globalG_, groupT, connectedV) < 0 || avInc < 0) degInc++;
			avInc--;
			if(degInc >= (numeric_limits<double>::max)()) break;

			cerr << "fail safe -> connV < 0, avInc " << avInc << " degInc " << degInc << endl;
			//char c = getchar();
		}
		// -- end fail safe --
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			// calculate available out degree
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	 		// cerr << "Available out degree " << avOutDegConnected << endl;
		}
	}
}

void
TreeDynamics::connectSearchMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0, avInc = 2;
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeWeight = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit) + degInc) continue;

			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);
	
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) - avInc;
				if(od > 0)
				{
					edge_descriptorN e = ep.first;
					double tempMinWeight = globalG_[ep.first].weight;
					findMCEdge(*vit, targ, targ, groupT.g, e, tempMinWeight);
	
					if(minEdgeWeight > tempMinWeight)
					{
						minEdgeWeight = tempMinWeight;
						connE = e;
						connV = targ;
						mitV = mit;
					}		
				}
			}
		}				
		// -- fail safe --
		if(connV < 0)
		{	// connectdV filled up out-degree (shouldn't happen), available out degree < 0
			if(getAvOutDegree(globalG_, groupT, connectedV) < 0 || avInc < 0) degInc++;
			avInc--;
			if(degInc >= (numeric_limits<double>::max)()) break;

			cerr << "fail safe -> connV < 0, avInc " << avInc << " degInc " << degInc << endl;
			//char c = getchar();
		}
		// -- end fail safe --
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			// calculate available out degree
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	 		// cerr << "Available out degree " << avOutDegConnected << endl;
		}
	}
}

// NEW CONNECT:
void TreeDynamics::connectMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0;
	while(adjacentMap.size() > 1)
	{
		int src_conn = -1, targ_conn = -1;
		edge_descriptorN connE;
		double min_current_diameter = (numeric_limits<double>::max)();
		double min_src_eccentricity = (numeric_limits<double>::max)(), min_targ_eccentricity = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mit_src, mit_targ;
		
		for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
		{
			int src = mit->second;
			if(getOutDegree(src, groupT.g) >= (getDegreeConstraint(globalG_, src) + degInc)) continue;
	
			double src_eccentricity = 0;
			if(getOutDegree(src, groupT.g) > 0)
				worstCaseDist(src, src, src, groupT, src_eccentricity);

			for(OutDegreeMap::iterator mit_in = adjacentMap.begin(), mit_in_end = adjacentMap.end(); mit_in != mit_in_end; ++mit_in)
			{
				int targ = mit_in->second;
				if(src == targ) continue;
				if(getOutDegree(targ, groupT.g) >= (getDegreeConstraint(globalG_, targ) + degInc)) continue;

				double target_eccentricity = 0;
				if(getOutDegree(targ, groupT.g) > 0)
					worstCaseDist(targ, targ, targ, groupT, target_eccentricity);

				pair<edge_descriptorN, bool> ep = edge(src, targ, globalG_);
				ASSERTING(ep.second);
				double new_src_eccentricity = max(src_eccentricity, globalG_[ep.first].weight + target_eccentricity);
				double new_target_eccentricity = max(target_eccentricity, globalG_[ep.first].weight + src_eccentricity);
	
				edge_descriptorN e = ep.first;
				//findMDEdge(src, src_eccentricity, targ, targ, groupT.g, e, new_src_eccentricity);

				if(min_current_diameter > new_src_eccentricity)
				{
					min_current_diameter = new_src_eccentricity;
					targ_conn = targ;
					src_conn = src;
					
					mit_targ = mit_in;
					mit_src = mit;
					min_src_eccentricity = new_src_eccentricity;
					min_targ_eccentricity = new_target_eccentricity;

					// -- debug --
					//int targ = target(connE, globalG_), src = source(connE,globalG_);
					//if(targ != src) targ = src;
					//cerr << WRITE_FUNCTION << " found edge : " << connE << " current diameter " << new_src_eccentricity << endl; 	
					//char c = getchar();
					// -- end debug --
				}		
			}
		}

		// -- fail safe --
		if(targ_conn < 0)
		{
			degInc++;
			if(degInc >= (numeric_limits<double>::max)()) break;
			//cerr << "fail safe -> targ_conn < 0, degInc " << degInc << endl;
		}
		// -- end fail safe --
		else
		{
			groupT.insertEdge(src_conn, targ_conn, globalG_);
			adjacentMap.erase(mit_targ);
			//cerr << WRITE_FUNCTION << " CONNECTED: (" << src_conn << "," << targ_conn << ")" << " current diameter " << min_current_diameter << endl; 	
		}
	}
}

// NEW CONNECT:
void TreeDynamics::connectSearchMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0;
	while(adjacentMap.size() > 1)
	{
		int src_conn = -1, targ_conn = -1;
		edge_descriptorN connE;
		double min_current_diameter = (numeric_limits<double>::max)();
		double min_src_eccentricity = (numeric_limits<double>::max)(), min_targ_eccentricity = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mit_src, mit_targ;
		
		for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
		{
			int src = mit->second;
			if(getOutDegree(src, groupT.g) >= (getDegreeConstraint(globalG_, src) + degInc)) continue;
	
			double src_eccentricity = 0;
			if(getOutDegree(src, groupT.g) > 0)
				worstCaseDist(src, src, src, groupT, src_eccentricity);

			for(OutDegreeMap::iterator mit_in = adjacentMap.begin(), mit_in_end = adjacentMap.end(); mit_in != mit_in_end; ++mit_in)
			{
				int targ = mit_in->second;
				if(src == targ) continue;
				if(getOutDegree(targ, groupT.g) >= (getDegreeConstraint(globalG_, targ) + degInc)) continue;

				double target_eccentricity = 0;
				if(getOutDegree(targ, groupT.g) > 0)
					worstCaseDist(targ, targ, targ, groupT, target_eccentricity);

				pair<edge_descriptorN, bool> ep = edge(src, targ, globalG_);
				ASSERTING(ep.second);
				double new_src_eccentricity = max(src_eccentricity, globalG_[ep.first].weight + target_eccentricity);
				double new_target_eccentricity = max(target_eccentricity, globalG_[ep.first].weight + src_eccentricity);
	
				edge_descriptorN e = ep.first;
				findMDEdge(src, src_eccentricity, targ, targ, groupT.g, e, new_src_eccentricity);

				if(min_current_diameter > new_src_eccentricity)
				{
					min_current_diameter = new_src_eccentricity;
					targ_conn = targ;
					src_conn = src;
					
					mit_targ = mit_in;
					mit_src = mit;
					min_src_eccentricity = new_src_eccentricity;
					min_targ_eccentricity = new_target_eccentricity;

					// -- debug --
					//int targ = target(connE, globalG_), src = source(connE,globalG_);
					//if(targ != src) targ = src;
					//cerr << WRITE_FUNCTION << " found edge : " << connE << " current diameter " << new_src_eccentricity << endl; 	
					//char c = getchar();
					// -- end debug --
				}		
			}
		}

		// -- fail safe --
		if(targ_conn < 0)
		{
			degInc++;
			if(degInc >= (numeric_limits<double>::max)()) break;
			//cerr << "fail safe -> targ_conn < 0, degInc " << degInc << endl;
		}
		// -- end fail safe --
		else
		{
			groupT.insertEdge(src_conn, targ_conn, globalG_);
			adjacentMap.erase(mit_targ);
			//cerr << WRITE_FUNCTION << " CONNECTED: (" << src_conn << "," << targ_conn << ")" << " current diameter " << min_current_diameter << endl; 	
		}
	}
}


/*-----------------------------------------------------------------------
			Connect vertices using delay constrained MCEs
------------------------------------------------------------------------- */
void 
TreeDynamics::connectDCMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	
	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();

		OutDegreeMap::iterator mitV;
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= (getDegreeConstraint(globalG_, *vit))) continue;
			
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);

				edge_descriptorN e = ep.first;
				double tempMinWeight = distance[*vit] + globalG_[ep.first].weight;
				double tempMinCost = globalG_[ep.first].cost;
				
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(minEdgeCost > tempMinCost && tempMinWeight <= GlobalSimArgs::getDelayConstraint() && od > 0)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}
		
		if(connV < 0) 
		{
			//cerr << " trying connectMCE " << endl;
			connectMCE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

void
TreeDynamics::connectDCMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);
		
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				double tempMinWeight = distance[*vit] + globalG_[ep.first].weight;
				if(od <= 0 || tempMinWeight > GlobalSimArgs::getDelayConstraint()) continue;
								
				double worstcase_cost = 0;
				if(getOutDegree(targ, groupT.g) > 0)
					worstCaseCost(targ, targ, targ, groupT.g, worstcase_cost);
				
				edge_descriptorN e = ep.first;
				double tempMinCost = globalG_[ep.first].cost + worstcase_cost;
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}				
		if(connV < 0) 
		{
			connectMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

void
TreeDynamics::connectDCSearchMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);

				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(od <= 0) continue; 
	
				edge_descriptorN e;
				double tempMinCost = (numeric_limits<double>::max)();
				double tempMinWeight = distance[*vit] + globalG_[ep.first].weight;
				if(tempMinWeight <= GlobalSimArgs::getDelayConstraint())
				{
					tempMinCost = globalG_[ep.first].cost;
					e = ep.first;
				}
				findMCEdgeC(*vit, targ, targ, groupT.g, e, tempMinCost, distance);
	
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}		
		if(connV < 0) 
		{
			connectSearchMCE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

void
TreeDynamics::connectDCSearchMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	
	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);
	
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(od <= 0) continue; 
		
				double tempMinCost = (numeric_limits<double>::max)();
				edge_descriptorN e = ep.first;
			
				double tempMinWeight = distance[*vit] + globalG_[ep.first].weight;
				if(tempMinWeight <= GlobalSimArgs::getDelayConstraint())
				{
					double worstcase_dist = 0;
					if(getOutDegree(targ, groupT.g) > 0)
						worstCaseCost(targ, targ, targ, groupT.g, worstcase_dist);
					tempMinCost = globalG_[ep.first].cost + worstcase_dist;
					e = ep.first;
				}
				findMDEdgeC(*vit, targ, targ, groupT.g, e, tempMinCost, distance);
	
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}		
		if(connV < 0) 
		{
			connectSearchMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}
/*-----------------------------------------------------------------------
			Connect vertices using diameter bounded MCEs
------------------------------------------------------------------------- */
void TreeDynamics::connectBDMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	
	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();

		OutDegreeMap::iterator mitV;
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= (getDegreeConstraint(globalG_, *vit))) continue;
			
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);

				edge_descriptorN e = ep.first;
				double diameter = distance[*vit] + globalG_[ep.first].weight;
				double tempMinCost = globalG_[ep.first].weight;
				
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(minEdgeCost > tempMinCost && diameter <= diameterBound && od > 0)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;

					// -- debug --
					//int targ = target(connE, globalG_), src = source(connE,globalG_);
					//if(targ != *vit) targ = src;
					//double diameter = distance[targ] + globalG_[e].weight;
					//cerr << WRITE_FUNCTION << " found edge : " << connE << " diameter : " << diameter << endl;
					//char c = getchar();
					//ASSERTING(diameter <= diameterBound);					
					// -- end debug --					
				}		
			}
		}
		
		if(connV < 0) 
		{
			//cerr << " trying connectMCE " << endl;
			connectMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}
void
TreeDynamics::connectBDMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);
		
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				double diameter = distance[*vit] + globalG_[ep.first].weight; // tempMinWeight
				if(od <= 0 || diameter > diameterBound) continue;
								
				double worstcase_cost = 0;
				if(getOutDegree(targ, groupT.g) > 0)
					worstCaseDist(targ, targ, targ, groupT, worstcase_cost);
				
				edge_descriptorN e = ep.first;
				double tempMinCost = globalG_[ep.first].weight + worstcase_cost;
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}				
		if(connV < 0) 
		{
			connectMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

void
TreeDynamics::connectBDSearchMCE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);

				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(od <= 0) continue; 
	
				edge_descriptorN e;
				double tempMinCost = (numeric_limits<double>::max)();
				double diameter = distance[*vit] + globalG_[ep.first].weight;

				if(diameter <= diameterBound)
				{
					tempMinCost = globalG_[ep.first].weight;
					e = ep.first;
				}
				findMCEdgeBD(*vit, targ, targ, groupT.g, e, tempMinCost, distance, diameterBound);
	
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;

					// -- debug --
					//int targ = target(connE, globalG_), src = source(connE,globalG_);
					//if(targ != *vit) targ = src;
					//double diameter = distance[targ] + globalG_[e].weight;
					//cerr << WRITE_FUNCTION << " found edge : " << connE << " diameter : " << diameter << endl;
					//char c = getchar();
					//ASSERTING(diameter <= diameterBound);					
					// -- end debug --					
				}		
			}
		}		
		if(connV < 0) 
		{
			//cerr << " Couldn't connect! Connectingin connectSearchMDE " << endl;
			connectSearchMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

void
TreeDynamics::connectBDSearchMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV, const DistanceVector &distance, const double &diameterBound)
{
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
	
	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double minEdgeCost = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT.g) >= getDegreeConstraint(globalG_, *vit)) continue;
				
			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(*vit, targ, globalG_);
				ASSERTING(ep.second);
	
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) ;
				if(od <= 0) continue; 
		
				double tempMinCost = (numeric_limits<double>::max)();
				edge_descriptorN e = ep.first;
			
				double diameter = distance[*vit] + globalG_[ep.first].weight;
				if(diameter <= diameterBound)
				{
					double worstcase_dist = 0;
					if(getOutDegree(targ, groupT.g) > 0)
						worstCaseDist(targ, targ, targ, groupT, worstcase_dist);
					tempMinCost = globalG_[ep.first].weight + worstcase_dist;
					e = ep.first;
				}
				findMDEdgeBD(*vit, targ, targ, groupT.g, e, tempMinCost, distance, diameterBound);
	
				if(minEdgeCost > tempMinCost)
				{
					minEdgeCost = tempMinCost;
					connE = e;
					connV = targ;
					mitV = mit;
				}		
			}
		}		
		if(connV < 0) 
		{
			connectSearchMDE(groupT, adjacentMap, connectedV);
			break;
		}
		else
		{
			ASSERTING(connV > -1);
		  	groupT.insertEdge(connE, globalG_);
			
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}

/*-----------------------------------------------------------------------
						various functions
------------------------------------------------------------------------- */
void
TreeDynamics::getNeighborsOptimizeSP(const TreeStructure &groupT, vertex_descriptorN src, vertex_descriptorN prev, VertexSet &adjacentSet, VertexSet &adjacentSetSP, EdgeList &remEdges)
{
	//if(adjacentSet.contains(src) || adjacentSetSP.contains(src)) return;

	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(src, groupT.g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT.g);
		if(adjacentSet.contains(targ) || adjacentSetSP.contains(targ)) continue;

		if(targ != prev)
		{
			remEdges.push_back(*oit);
			//cerr << *oit << ", ";
			if(groupT.g[targ].vertexState == GROUP_MEMBER)
				adjacentSet.insert(targ);
			if(groupT.g[targ].vertexState == STEINER_POINT) 
			{
				adjacentSetSP.insert(targ);
				getNeighborsOptimizeSP(groupT, targ, src, adjacentSet, adjacentSetSP, remEdges);
			}
		}
	}
}

void
TreeDynamics::getNeighbors(const TreeStructure &groupT, vertex_descriptorN src, VertexSet &adjacentSet)
{
	//cerr << WRITE_FUNCTION << " " << src << " neighbors " ;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(src, groupT.g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT.g);
		adjacentSet.insert(targ);
		//cerr << targ << " " ;
	}
	//cerr << endl;
}

void
TreeDynamics::getNeighbors(const TreeStructure &groupT, vertex_descriptorN src, VertexSet &adjacentSet, OutDegreeMap &adjacentMap)
{
	//cerr << WRITE_FUNCTION << " " << src << " neighbors " ;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(src, groupT.g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT.g);
		int avOD = getAvOutDegree(globalG_, groupT, targ) + 1;
		adjacentMap.insert(pair<int, vertex_descriptorN>(avOD,targ));
		adjacentSet.insert(targ);
		//cerr << targ << " " ;
	}
	//cerr << endl;
}
/*
void
TreeDynamics::subGraphRingSP(SimVertexMap& subVertices, VertexSet& steinerSet, vertex_descriptorN actVert, vertex_descriptorN src, vertex_descriptorN prev, EdgeList& remEdges, int k)
{
	//cerr << WRITE_FUNCTION << endl;
	out_edge_iteratorN oit, oit_end;
	for(tie(oit, oit_end) = out_edges(src, groupT_.g); oit != oit_end && k > 0; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT_.g);	
		ASSERTING(targ != src );
		//cerr << "ActVert " << actVert << ": " << *oit << " targ " << targ << " == " << prev << " ? " ;
		
		if(targ != actVert && targ != prev)
		{
			remEdges.push_back(*oit);
			
			int k_out = k;
			if(groupT_.g[targ].vertexState == GROUP_MEMBER) 
			{
				subVertices.insertVertex(groupT_.g[targ]);
				k_out--; 
				//cerr << " Inserted Group Member: " << targ << endl;
			}
			else if(groupT_.g[targ].vertexState == STEINER_POINT) 
			{
				steinerSet.insert(targ);
				//cerr << " Inserted SP: " << targ << endl;
			}
			else ASSERTING(0);
		
			subGraphRingSP(subVertices, steinerSet, actVert, targ, src, remEdges, k_out);
		}
	}	
}
*/
void
TreeDynamics::subGraphRingSP(VertexSet& subVertices, VertexSet& steinerSet, vertex_descriptorN actVert, vertex_descriptorN src, vertex_descriptorN prev, EdgeList& remEdges, int k)
{
	//cerr << WRITE_FUNCTION << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(src, groupT_.g); oit != oit_end && k > 0; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT_.g);	
		ASSERTING(targ != src );
		//cerr << "ActVert " << actVert << ": " << *oit << " targ " << targ << " == " << prev << " ? " ;
		
		if(targ != actVert && targ != prev)
		{
			remEdges.push_back(*oit);
			
			int k_out = k;
			if(groupT_.g[targ].vertexState == GROUP_MEMBER) 
			{
				subVertices.insert(targ);
				k_out--; 
				//cerr << " Inserted Group Member: " << targ << endl;
			}
			else if(groupT_.g[targ].vertexState == STEINER_POINT) 
			{
				steinerSet.insert(targ);
				//cerr << " Inserted SP: " << targ << endl;
			}
			else ASSERTING(0);
		
			subGraphRingSP(subVertices, steinerSet, actVert, targ, src, remEdges, k_out);
		}
	}	
}


void
TreeDynamics::findMCEdge(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << endl;
		pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
		ASSERTING(e.second);

		if((globalG_[e.first].weight) < w && getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
		{
			w = globalG_[e.first].weight;
			new_e = e.first;
		}
				
		findMCEdge(src, start_v, targ, tree_g, new_e, w);
	}
}


void
TreeDynamics::findMDEdge(vertex_descriptorN src, const double &src_eccentricity, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &current_src_eccentricity)
{
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		if(targ == prev_v) continue;
		
		if(getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
		{
			double target_eccentricity = 0;
			worstCaseDist(targ, targ, targ, tree_g, target_eccentricity);

			pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
			ASSERTING(e.second);

			double new_src_eccentricity = max(src_eccentricity, globalG_[e.first].weight + target_eccentricity);
			if(new_src_eccentricity < current_src_eccentricity)		
			{
				current_src_eccentricity = new_src_eccentricity;
				new_e = e.first;
			}
		}
		findMDEdge(src, src_eccentricity, start_v, targ, tree_g, new_e, current_src_eccentricity);
	}
}

/*
void
TreeDynamics::findMDEdge(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
	for(tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
			
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << " prev_v " << prev_v << endl;
		if(getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
		{
			double worstcase_dist = 0;
			worstCaseDist(targ, targ, targ, tree_g, worstcase_dist);

			pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
			ASSERTING(e.second);
			if((globalG_[e.first].weight + worstcase_dist) < w)		
			{
				w = globalG_[e.first].weight + worstcase_dist;
				new_e = e.first;
			}
		}
		
		findMDEdge(src, start_v, targ, tree_g, new_e, w);
	}
}
*/

void
TreeDynamics::findMCEdgeC(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << endl;
		pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
		ASSERTING(e.second);
	
		// TODO: check for delay constraints
		double tempMinWeight = distance[targ] + globalG_[e.first].weight;
		if(tempMinWeight <= GlobalSimArgs::getDelayConstraint()) //continue;
		{
			if((globalG_[e.first].cost) < w && getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
			{
				w = globalG_[e.first].cost;
				new_e = e.first;
			}
		}

		findMCEdgeC(src, start_v, targ, tree_g, new_e, w, distance);
	}
}

void
TreeDynamics::findMDEdgeC(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
			
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << " prev_v " << prev_v << endl;
		if(getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
		{
			pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
			ASSERTING(e.second);

			// TODO: check for delay constraints
			double tempMinWeight = distance[targ] + globalG_[e.first].weight;
			if(tempMinWeight <= GlobalSimArgs::getDelayConstraint()) //continue;
			{
				double worstcase_dist = 0;
				worstCaseCost(targ, targ, targ, tree_g, worstcase_dist);
			
				if((globalG_[e.first].cost + worstcase_dist) < w)		
				{
					w = globalG_[e.first].cost+ worstcase_dist;
					new_e = e.first;
				}
			}
		}
		
		findMDEdgeC(src, start_v, targ, tree_g, new_e, w, distance);
	}
}

void
TreeDynamics::findMCEdgeBD(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance, const double &diameterBound)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << endl;
		pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
		ASSERTING(e.second);
	
		// TODO: check for delay constraints
		double diameter = distance[targ] + globalG_[e.first].weight;
		if(diameter <= diameterBound) //continue;
		{
			//cerr << WRITE_FUNCTION << " edge : " << e.first << " diameter " << diameter << " bound " << diameterBound << endl;
			if((globalG_[e.first].weight) < w && getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
			{
				w = globalG_[e.first].weight;
				new_e = e.first;
			}
		}

		findMCEdgeBD(src, start_v, targ, tree_g, new_e, w, distance, diameterBound);
	}
}

void
TreeDynamics::findMDEdgeBD(vertex_descriptorN src, vertex_descriptorN prev_v, vertex_descriptorN start_v, const GraphN &tree_g, edge_descriptorN& new_e, double &w, const DistanceVector &distance, const double &diameterBound)
{
	//cerr << WRITE_FUNCTION << "src " << src << " prev_v " << prev_v << " start_v " << start_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(start_v, tree_g); oit != oit_end; ++oit)
	{
		vertex_descriptorN targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
			
		if(targ == prev_v) continue;
		
		//cerr << *oit << " targ " << targ << " src " << src << " prev_v " << prev_v << endl;
		if(getOutDegree(targ, tree_g) < getDegreeConstraint(globalG_, targ))
		{
			pair<edge_descriptorN, bool> e = edge(src, targ, globalG_);
			ASSERTING(e.second);

			// TODO: check for delay constraints
			double diameter = distance[targ] + globalG_[e.first].weight;
			if(diameter <= diameterBound) //continue;
			{
				double worstcase_dist = 0;
				worstCaseDist(targ, targ, targ, tree_g, worstcase_dist);
			
				if((globalG_[e.first].weight + worstcase_dist) < w)		
				{
					w = globalG_[e.first].weight + worstcase_dist;
					new_e = e.first;
				}
			}
		}
		
		findMDEdgeBD(src, start_v, targ, tree_g, new_e, w, distance, diameterBound);
	}
}

void
TreeDynamics::checkRemove(vertex_descriptorN rem_vert)
{
#ifndef NDEBUG
	if(groupT_.V.contains(rem_vert))
	{
		if(groupT_.g[rem_vert].vertexState != STEINER_POINT)
		{
			dumpTreeStructure(groupT_);
			cerr << rem_vert << " Props: " << groupT_.g[rem_vert] << endl;
			cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
			cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
		}
		ASSERTING(groupT_.g[rem_vert].vertexState == STEINER_POINT);
	}

	if(groupT_.V.size() > 1)
	{
		for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT_.g) <= 0)
			{
				dumpTreeStructure(groupT_);
				//dumpGraph(groupT_.g);
				cerr << " Vertex " << *vit << " out-degree == 0 " << endl;
				cerr << " Props: " << groupT_.g[*vit] << endl;
				cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
				cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
			}
			ASSERTING(getOutDegree(*vit, groupT_.g) > 0);
		}
	
		if(groupT_.E.size() > 1) GraphAlgorithms::checkGraph(groupT_.g);
	}
	
	VertexSet edgeVerts;
	EdgePair::iterator pit = groupT_.Ep.begin(), pit_end = groupT_.Ep.end();
	for( ; pit != pit_end; ++pit)
	{
		if(pit->first == rem_vert || pit->second == rem_vert)
			ASSERTING(groupT_.g[rem_vert].vertexState == STEINER_POINT);

		ASSERTING(getOutDegree(pit->first, groupT_.g) > 0);
		ASSERTING(getOutDegree(pit->second, groupT_.g) > 0);
		vertex_descriptorN src = pit->first;
		vertex_descriptorN targ = pit->second;
		
		ASSERTING(src != targ);
		ASSERTING(groupT_.V.contains(src));
		ASSERTING(groupT_.V.contains(targ));
		
		edgeVerts.insert(src);
		edgeVerts.insert(targ);
	}
	
	VertexSet edgeVerts2;
	edge_iteratorN eit, eit_end;
    for(boost::tuples::tie(eit, eit_end) = edges(groupT_.g); eit != eit_end; ++eit)
	{
		vertex_descriptorN src = source(*eit, groupT_.g), targ = target(*eit, groupT_.g);
		
		ASSERTING(src != targ);
		ASSERTING(groupT_.V.contains(src));
		ASSERTING(groupT_.V.contains(targ));

		if(!edgeVerts.contains(src) || !edgeVerts.contains(targ))
		{
			//dumpTreeStructure(groupT_);
			//dumpGraph(groupT_.g);
	
			cerr << "Missing from T.Ep : (" << src << ", " << targ << ") " << endl;
			cerr << " tree verts " << groupT_.V << endl;
			cerr << " edge verts " << edgeVerts << endl;
			cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
			cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
		}
		ASSERTING(edgeVerts.contains(src));
		ASSERTING(edgeVerts.contains(targ));
		edgeVerts2.insert(src);
		edgeVerts2.insert(targ);
	}
	
	if(GlobalSimArgs::removeMeshAlgo() == NO_DYNAMIC_MESH_ALGO)
	{
		if(groupT_.E.size() != num_edges(groupT_.g) || ((groupT_.V.size() && groupT_.V.size() != num_edges(groupT_.g) + 1))  || groupInfo_.getMembers().size() != (groupT_.V.size() - groupT_.S.size()))
		{
			//dumpTreeStructure(groupT_);
			//dumpGraph(groupT_.g);
	
			cerr << " tree verts " << groupT_.V << endl;
			cerr << " Ep   verts " << edgeVerts << endl;
			cerr << " edge verts " << edgeVerts2 << endl;
			cerr << " remaining  " << edgeVerts - edgeVerts2 << " OR: " << edgeVerts2 - edgeVerts <<  " OR: " << edgeVerts - groupT_.V << " OR: " << groupT_.V - edgeVerts << " OR: " << edgeVerts2 - groupT_.V << " OR: " << groupT_.V - edgeVerts2 << endl;;
			cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
			cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
			cerr << "Num edge verts " << edgeVerts.size() << " " << edgeVerts2.size() << endl;
			cerr << " members : " << groupInfo_.getMembers() << " steiner points : " << groupT_.S << endl;
			cerr << " members - steiner points: " << groupT_.V - groupT_.S << endl;
		}
		if(groupT_.V.size())
		{
			ASSERTING(groupT_.V.size() == num_edges(groupT_.g) + 1);
			ASSERTING(groupT_.V.size() == groupT_.E.size() + 1);
		}
	}
	else
	{
		if(groupT_.E.size() != num_edges(groupT_.g) ||  // || ((groupT_.V.size() && groupT_.V.size() != num_edges(groupT_.g) + 1)))
			groupInfo_.getMembers().size() != (groupT_.V.size() - groupT_.S.size()))
		{
			//dumpTreeStructure(groupT_);
			//dumpGraph(groupT_.g);
	
			cerr << " tree verts " << groupT_.V << endl;
			cerr << " Ep   verts " << edgeVerts << endl;
			cerr << " edge verts " << edgeVerts2 << endl;
			cerr << " remaining  " << edgeVerts - edgeVerts2 << " OR: " << edgeVerts2 - edgeVerts <<  " OR: " << edgeVerts - groupT_.V << " OR: " << groupT_.V - edgeVerts << " OR: " << edgeVerts2 - groupT_.V << " OR: " << groupT_.V - edgeVerts2 << endl;;
			cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
			cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
			cerr << "Num edge verts " << edgeVerts.size() << " " << edgeVerts2.size() << endl;
			cerr << " members : " << groupInfo_.getMembers() << " steiner points : " << groupT_.S << endl;
			cerr << " members - steiner points: " << groupT_.V - groupT_.S << endl;
		}

	}
	
	ASSERTING(groupT_.E.size() == groupT_.Ep.size());
	ASSERTING(groupT_.E.size() == num_edges(groupT_.g));
	ASSERTING(groupInfo_.getMembers().size() == (groupT_.V.size() - groupT_.S.size()));
#endif // NDEBUG
}


void
TreeDynamics::checkInsert(vertex_descriptorN ins_vert)
{
#ifndef NDEBUG
	ASSERTING(groupT_.V.contains(ins_vert));
	
	if(groupT_.V.size() > 1)
	{
		for(VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end(); vit != vit_end; ++vit)
		{
			if(getOutDegree(*vit, groupT_.g) <= 0)
			{
				dumpTreeStructure(groupT_);
				//dumpGraph(groupT_.g);
				cerr << " Vertex " << *vit << " out-degree == 0 " << endl;
				cerr << " Props: " << groupT_.g[*vit] << endl;
				cerr << "Tree Algo " << treeAlgo_ << " action Vert " << actionVert_ << endl;
				cerr << "Num vertices " << groupT_.V.size() << " num edges " << num_edges(groupT_.g) << endl;
			}
			ASSERTING(getOutDegree(*vit, groupT_.g) > 0);
		}
	
		if(groupT_.E.size() > 1) GraphAlgorithms::checkGraph(groupT_.g);
	}

	if(groupT_.E.size() != num_edges(groupT_.g))
	{
		dumpTreeStructure(groupT_);
		dumpGraph(groupT_.g);
	}

	if(num_edges(groupT_.g))
	{
		bool found_ins_vert = false;
		edge_iteratorN eit, eit_end;
        for(boost::tuples::tie(eit, eit_end) = edges(groupT_.g); eit != eit_end; ++eit)
		{
			vertex_descriptorN src = source(*eit, groupT_.g), targ = target(*eit, groupT_.g);
		
			if(ins_vert == src || ins_vert == targ)
				found_ins_vert = true;
		}
		
		if(!found_ins_vert)
		{
			dumpTreeStructure(groupT_);
			dumpGraph(groupT_.g);
		}
		ASSERTING(found_ins_vert);
	}
	
	if(groupT_.V.size())
	{
		ASSERTING(groupT_.V.size() == num_edges(groupT_.g) + 1);
		ASSERTING(groupT_.V.size() == groupT_.E.size() + 1);
	}

	ASSERTING(groupT_.E.size() == groupT_.Ep.size());
	ASSERTING(groupT_.E.size() == num_edges(groupT_.g));
	ASSERTING(groupInfo_.getMembers().size() == (groupT_.V.size() - groupT_.S.size()));

#endif // NDEBUG
}
/* -----------------------------------------------------------------------------
							   unused 
-------------------------------------------------------------------------------*/		
/*-----------------------------------------------------------------------
	connectComponents:	
		compOne and compTwo must be either the same, or disjoint.
------------------------------------------------------------------------- */
void
TreeDynamics::connectComponentsDisjoint(TreeStructure &groupT, const VertexSet &compOne, const VertexSet &compTwo)
{
	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = compOne.begin(), vit_end = compOne.end(); vit != vit_end ; ++vit)
	{
		multimap<double, int> mapRankV;
		for(vit_in = compTwo.begin(), vit_in_end = compTwo.end(); vit_in != vit_in_end; ++vit_in)
		{
			if(*vit_in == *vit) continue;
			
			pair<edge_descriptorN, bool> ep = edge(*vit_in, *vit, groupT.g);
			if((num_vertices(groupT.g) > max(*vit_in, *vit)) && ep.second) continue;
			
			ep = edge(*vit_in, *vit, globalG_);
			ASSERTING(ep.second);
	
			//cerr << "(" << *vit_in << "," << *vit << ") " ;
			mapRankV.insert(pair<double, int>(globalG_[ep.first].weight, *vit_in));
		}
	
		if(mapRankV.size())
		{	
			vertex_descriptorN best_v;
			multimap<double, int>::iterator mmit = mapRankV.begin(), mmit_end = mapRankV.end();
			for( ; mmit != mmit_end; ++mmit)
			{
				if(getDegreeConstraint(globalG_, mmit->second) > (getOutDegree(groupT.g, mmit->second)))
				{
					best_v = mmit->second;
					break;
				}
			}
		
			if(mmit == mmit_end) continue;
			
			groupT.insertEdge(best_v, *vit, globalG_);
		}
	}
}

void
TreeDynamics::connectComponentsEqual(TreeStructure &groupT, const VertexSet &compOne, const VertexSet &compTwo)
{
    std::vector<VertexSet> connectedVerts(num_vertices(groupT.g) + 1);

	VertexSet::const_iterator vit, vit_end, vit_in, vit_in_end;
	for(vit = compOne.begin(), vit_end = compOne.end(); vit != vit_end ; ++vit) 
		connectedVerts[*vit].insert(*vit);

	for(vit = compOne.begin(), vit_end = compOne.end(); vit != vit_end ; ++vit)
	{
		multimap<double, int> mapRankV;
		for(vit_in = compTwo.begin(), vit_in_end = compTwo.end(); vit_in != vit_in_end; ++vit_in)
		{
			if(connectedVerts[*vit].contains(*vit_in)) continue;
			
			pair<edge_descriptorN, bool> ep = edge(*vit_in, *vit, groupT.g);
			if((num_vertices(groupT.g) > max(*vit_in, *vit)) && ep.second) continue;
			
			ep = edge(*vit_in, *vit, globalG_);
			ASSERTING(ep.second);
	
			// cerr << "(" << *vit_in << "," << *vit << ") " ;
			mapRankV.insert(pair<double, int>(globalG_[ep.first].weight, *vit_in));
		}
	
		if(mapRankV.size())
		{	
			int best_v = -1;
			multimap<double, int>::iterator mmit = mapRankV.begin(), mmit_end = mapRankV.end();
			for( ; mmit != mmit_end; ++mmit)
			{
				if(getDegreeConstraint(globalG_, mmit->second) > getOutDegree(groupT.g, mmit->second))
				{
					best_v = mmit->second;
					break;
				}
			}
		
			if(best_v < 0) continue;
		
			groupT.insertEdge(best_v, *vit, globalG_);
			//cerr << "Inserting ( " << best_v << ", " << *vit << ") " << endl; 
		
			connectedVerts[*vit] += connectedVerts[best_v];

			// update for all
			for(VertexSet::iterator cit = connectedVerts[*vit].begin(), cit_end = connectedVerts[*vit].end(); cit != cit_end; ++cit)
				connectedVerts[*cit] = connectedVerts[*vit]; 	
		}
	}
}


void TreeDynamics::calcNeighborHood(vertex_descriptorN actionVert, vertex_descriptorN src, vertex_descriptorN prev, double& w, int& n, int k)
{
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(src, groupT_.g); oit != oit_end && k >= 0; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT_.g);	
		ASSERTING(targ != src );
		
		//cerr << " (targ " << targ << " == " << prev << " prev) ? " << " actionVert "<< actionVert << " src " << src << endl;

		if(targ == prev) continue;

		if(targ != actionVert)
		{
			w = w + groupT_.g[*oit].weight;
			n = n + 1;
			//cerr << "updated w and n " << endl;
			//cerr << *oit << " w: " << w << " n: " << n << endl;
		}
		
		calcNeighborHood(actionVert, targ, src, w, n, (k - 1));	
	}
}

void
TreeDynamics::findBNP(const GraphN &tree_g, vertex_descriptorN actVert, vertex_descriptorN start_v, vertex_descriptorN prev_v, double &bcNeighborHood, int &numNeighbors, edge_descriptorN &new_e)
{
	vertex_descriptorN src, targ;
	vertex_descriptorN curr_v = start_v;
	bool done = false;
	double neighborhoodWeight = 0;
	int neighbors = 0;

	//cerr << WRITE_FUNCTION << " actVert " << actVert << " curr_v " << curr_v << " prev_v " << prev_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(curr_v, tree_g); oit != oit_end; ++oit)
	{
		targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		if(targ == prev_v) continue;
		
		findBNP(tree_g, actVert, targ, curr_v, bcNeighborHood, numNeighbors, new_e);
		if(getOutDegree(targ, tree_g) >= getDegreeConstraint(globalG_, targ)) continue;
						
		calcNeighborHood(targ, targ, targ, neighborhoodWeight, neighbors, 2);
	
		// connecting edge
		pair<edge_descriptorN, bool> e = edge(actVert, targ, globalG_);
		ASSERTING(e.second);
		
		double tempRank = (neighborhoodWeight + globalG_[e.first].weight)/neighbors;
		if((bcNeighborHood > tempRank) || (bcNeighborHood == tempRank && numNeighbors < neighbors))
		{
			bcNeighborHood 	= tempRank;
			numNeighbors 	= neighbors;
			ASSERTING(bcNeighborHood > 0);
			new_e = e.first;
		}
		//cerr << "for(): tempRank " << tempRank << " neighborhoodWeight " << neighborhoodWeight << " neighbors " << neighbors << " edge " << e.first << endl;
		//cerr << "for(): bcNeighborhood " << bcNeighborHood << " numNeighbors " << numNeighbors << " new_e " << new_e << endl;
		neighborhoodWeight = 0; neighbors = 0;
	}
}

void
TreeDynamics::findMDBNP(const GraphN &tree_g, vertex_descriptorN actVert, vertex_descriptorN start_v, vertex_descriptorN prev_v, double &bcNeighborHood, int &numNeighbors, edge_descriptorN &new_e)
{
	vertex_descriptorN src, targ;
	vertex_descriptorN curr_v = start_v;
	bool done = false;
	double neighborhoodWeight = 0;
	int neighbors = 0;

	//cerr << WRITE_FUNCTION << " actVert " << actVert << " curr_v " << curr_v << " prev_v " << prev_v << endl;
	out_edge_iteratorN oit, oit_end;
    for(boost::tuples::tie(oit, oit_end) = out_edges(curr_v, tree_g); oit != oit_end; ++oit)
	{
		targ = target(*oit, tree_g);
		ASSERTING(targ != start_v);
		if(targ == prev_v) continue;
			
		findBNP(tree_g, actVert, targ, curr_v, bcNeighborHood, numNeighbors, new_e);
		if(getOutDegree(targ, tree_g) >= getDegreeConstraint(globalG_, targ)) continue;

		calcNeighborHood(targ, targ, targ, neighborhoodWeight, neighbors, 2);
		double worstcase_dist = 0;
		if(getOutDegree(targ, groupT_.g) > 0)	
		{
			worstCaseDist(targ, targ, targ, tree_g, worstcase_dist);
			//cerr << WRITE_FUNCTION << "worst case distance " << worstcase_dist << endl;
		}
	
		// connecting edge
		pair<edge_descriptorN, bool> e = edge(actVert, targ, globalG_);
		ASSERTING(e.second);
		
		double tempRank = (neighborhoodWeight + globalG_[e.first].weight + worstcase_dist)/neighbors;
		if((bcNeighborHood > tempRank) || (bcNeighborHood == tempRank && numNeighbors < neighbors))
		{
			bcNeighborHood 	= tempRank;
			numNeighbors 	= neighbors;
			ASSERTING(bcNeighborHood > 0);
			new_e = e.first;
		}
		//cerr << "for(): neighborhoodWeight " << neighborhoodWeight + worstcase_dist << " neighbors " << neighbors << " edge " << e.first << endl;
		//cerr << "for(): bcNeighborhood " << bcNeighborHood << " numNeighbors " << numNeighbors << " new_e " << new_e << endl;
		neighborhoodWeight = 0; neighbors = 0;
	}
}


/*
void
TreeDynamics::subGraphRingk(SimVertexMap& subVertices, VertexSet& steinerSet, vertex_descriptorN actVert, vertex_descriptorN src, EdgeList& remEdges, int k)
{
	k--;
	out_edge_iteratorN oit, oit_end;
	for(tie(oit, oit_end) = out_edges(src, groupT_.g); oit != oit_end && k >= 0; ++oit)
	{
		vertex_descriptorN targ = target(*oit, groupT_.g);	
		ASSERTING(targ != src );
		cerr << *oit << endl;
	
		remEdges.push_back(*oit);
		if(targ != actVert)
		{
			// because other vertices are connected to it, it is marked as a group member even though it is steiner point
			subVertices.insertVertex(targ, groupT_.g[targ], GROUP_MEMBER); 
			if(groupT_.g[targ].vertexState == STEINER_POINT)
				steinerSet.insert(targ);
		}
		
		subGraphRingk(subVertices, steinerSet, actVert, targ, remEdges, k);	
	}
}
*/

/*
void
TreeDynamics::connectSearchMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0, avInc = 2;
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double min_current_diameter = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			int src = *vit;
			if(getOutDegree(src, groupT.g) >= getDegreeConstraint(globalG_, src) + degInc) continue;
	
			double src_eccentricity = 0;
			if(getOutDegree(src, groupT.g) > 0)
				worstCaseDist(src, src, src, groupT, src_eccentricity);

			// if source is a leaf node then its eccentricity is alwasy zero -> 

			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(src, targ, globalG_);
				ASSERTING(ep.second);
	
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) - avInc;
				if(od > 0)
				{
					double target_eccentricity = 0;
					if(getOutDegree(targ, groupT.g) > 0)
						worstCaseDist(targ, targ, targ, groupT, target_eccentricity);

					edge_descriptorN e = ep.first;
					//double new_diameter = src_eccentricity + globalG_[ep.first].weight + target_eccentricity;
					double new_src_eccentricity = max(src_eccentricity, globalG_[ep.first].weight + target_eccentricity);
					double new_target_eccentricity = max(target_eccentricity, globalG_[ep.first].weight + src_eccentricity);

					//cerr << src << " src eccentricity " << src_eccentricity << " new src eccentricity " << new_src_eccentricity << " " << targ << " target eccentricity " << target_eccentricity << " new target eccentricity " << new_target_eccentricity << endl;
					//double eccentricity_difference = abs(new_target_eccentricity - target_eccentricity) + abs(new_src_eccentricity - src_eccentricity);
					//cerr << "Eccentricity difference : " << eccentricity_difference << endl;
					//findMDEdge(src, src_eccentricity, targ, targ, groupT.g, e, this_diameter);

					if(min_current_diameter > new_src_eccentricity)
					{
						min_current_diameter = new_src_eccentricity;
						connE = e;
						connV = targ;
						mitV = mit;
	
						// -- debug --
						//int targ = target(connE, globalG_), src = source(connE,globalG_);
						//if(targ != src) targ = src;
						//cerr << WRITE_FUNCTION << " found edge : " << connE << " current diameter " << min_current_diameter << endl; 	
						//char c = getchar();
						// -- end debug --
					}		
				}
			}
		}				
		// -- fail safe --
		if(connV < 0)
		{	// connectdV filled up out-degree (shouldn't happen), available out degree < 0
			if(getAvOutDegree(globalG_, groupT, connectedV) < 0 || avInc < 0) degInc++;
			avInc--;
			if(degInc >= (numeric_limits<double>::max)()) break;

			cerr << "fail safe -> connV < 0, avInc " << avInc << " degInc " << degInc << endl;
			//char c = getchar();
		}
		// -- end fail safe --
		else
		{
			groupT.insertEdge(connE, globalG_);
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			// calculate available out degree
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
			//cerr << WRITE_FUNCTION << " CONNECTED: " << connE << " current diameter " << min_current_diameter << endl; 	
		}
	}
}
*/

/*
void
TreeDynamics::connectMDE(TreeStructure &groupT, OutDegreeMap &adjacentMap, VertexSet &connectedV)
{
	int degInc = 0, avInc = 2;
	int neighborSz = adjacentMap.size() + connectedV.size();
	int avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);

	VertexSet::iterator vit, vit_end;
	while(connectedV.size() < neighborSz)
	{
		int connV = -1;
		edge_descriptorN connE;
		double min_current_eccentricity = (numeric_limits<double>::max)();
		OutDegreeMap::iterator mitV;
		
		for(vit = connectedV.begin(), vit_end = connectedV.end(); vit != vit_end; ++vit)
		{
			int src = *vit;
			if(getOutDegree(src, groupT.g) >= getDegreeConstraint(globalG_, src) + degInc) continue;
	
			double src_eccentricity = 0;
			if(getOutDegree(src, groupT.g) > 0)
				worstCaseDist(src, src, src, groupT, src_eccentricity);

			for(OutDegreeMap::iterator mit = adjacentMap.begin(), mit_end = adjacentMap.end(); mit != mit_end; ++mit)
			{
				vertex_descriptorN targ = mit->second;
				pair<edge_descriptorN, bool> ep = edge(src, targ, globalG_);
				ASSERTING(ep.second);
	
				int od = avOutDegConnected + getAvOutDegree(globalG_, groupT, targ) - avInc;
				if(od > 0)
				{
					double target_eccentricity = 0;
					if(getOutDegree(targ, groupT.g) > 0)
						worstCaseDist(targ, targ, targ, groupT, target_eccentricity);
					
					edge_descriptorN e = ep.first;
					//double this_diameter = src_eccentricity + globalG_[ep.first].weight + target_eccentricity;
					double this_eccentricity = max(src_eccentricity, globalG_[ep.first].weight + target_eccentricity);

					if(min_current_eccentricity > this_eccentricity)
					{
						min_current_eccentricity = this_eccentricity;
						connE = e;
						connV = targ;
						mitV = mit;
	
						// -- debug --
						//int targ = target(connE, globalG_), src = source(connE,globalG_);
						//if(targ != src) targ = src;
						cerr << WRITE_FUNCTION << " found edge : " << connE << " current diameter " << min_current_eccentricity << endl; 	
						//char c = getchar();
						// -- end debug --
					}		
				}
			}
		}				
		// -- fail safe --
		if(connV < 0)
		{	// connectdV filled up out-degree (shouldn't happen), available out degree < 0
			if(getAvOutDegree(globalG_, groupT, connectedV) < 0 || avInc < 0) degInc++;
			avInc--;
			if(degInc >= (numeric_limits<double>::max)()) break;

			cerr << "fail safe -> connV < 0, avInc " << avInc << " degInc " << degInc << endl;
			//char c = getchar();
		}
		// -- end fail safe --
		else
		{
			groupT.insertEdge(connE, globalG_);
			connectedV.insert(connV);
			adjacentMap.erase(mitV);
			// calculate available out degree
			avOutDegConnected = getAvOutDegree(globalG_, groupT, connectedV);
		}
	}
}
*/


