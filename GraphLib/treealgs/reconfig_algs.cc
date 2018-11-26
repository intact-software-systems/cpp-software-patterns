/***************************************************************************
                          reconfig_algs.cc  -  description
                             -------------------
    begin                : Tue Sep 5 2006
    copyright            : (C) 2005 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include <fstream>
 
#include "reconfig_algs.h"
#include "../simdefs.h"
#include "../boostprop.h"
#include "../graphalgs/graphalgs.h"

using namespace std;
using namespace boost;
using namespace TreeAlgorithms;
using namespace GraphAlgorithms;

/*-----------------------------------------------------------------------
						class ReconfigAlgs
		TODO: steiner points and group members have different degree 
		constraints. Should I reconfigure, adjust for that?
------------------------------------------------------------------------- */
void ReconfigAlgs::reconfigTree()
{
	//cerr << WRITE_FUNCTION << endl;
	
	switch(GlobalSimArgs::getSimReconfigAlgo())
	{
		default:
			reconfigMSP();
			break;
	}
}

/*-----------------------------------------------------------------------
		insert vertex:
			minimum steiner path, try to insert a steiner point
	connecting new vertex, and neighbors of group member.
	maximum 1 steiner point on path, check the steiner points in 
	SteinerPoint, a steiner point is added iff two or more tree 
	members are involved in the link/weight/edge calculation,
	TODO: find best located steiner point(s) for this tree,
	reconfiguration of tree involves at least two member nodes
   	-> idea to connect everyone to the steiner point, it becomes 
	a local star

	NB! TODO: fill up out-degree constraint of WCN!!!

	------------------------------------------------------------------------- */
void 
ReconfigAlgs::reconfigMSP()
{
	//cerr << WRITE_FUNCTION << endl;
	dumpTreeStructure(groupT_);
	dumpGraph(groupT_.g);
	
	int k = generateSizeOfSteinerSet(groupInfo_.getMembers().size(), groupInfo_.getSteinerPoints().size());
	if(k <= 0) return;
	
	VertexSet steinerSet;
	findkBestLocatedNodesNotInSet_Average(globalG_, fifoSteiner_.steinerSet_, groupT_.V, steinerSet, k);
			
	//cerr << "Steiner set " << steinerSet << endl;
	ASSERTING(steinerSet.size() <= k);

	VertexSet::iterator sit = steinerSet.begin(), sit_end = steinerSet.end();
	for( ; sit != sit_end; ++sit)
	{
		double bestImprovement = (numeric_limits<double>::max)();
		vertex_descriptorN bestVert;
		EdgeList bestRemEdges;
		VertexSet bestAddVerts;
		
		VertexSet::iterator vit = groupT_.V.begin(), vit_end = groupT_.V.end();
		for( ; vit != vit_end; ++vit)
		{
			if(groupT_.g[*vit].vertexState == STEINER_POINT) continue;
			
			VertexSet addVerts;
			addVerts.insert(*vit);
			EdgeList remEdges;
			bool foundSP = false;
			pair<edge_descriptorN, bool> ep;
			double sumEdgeSP = 0;
			double sumEdges = 0;
			out_edge_iteratorN eit, eit_end;
            for(boost::tuples::tie(eit, eit_end) = out_edges(*vit, groupT_.g); eit != eit_end; ++eit)
			{
				sumEdges = sumEdges + groupT_.g[*eit].weight;
				
				vertex_descriptorN targ = target(*eit, groupT_.g);
				ASSERTING(targ != *vit);
				ep = edge(*sit, targ, globalG_);
				ASSERTING(ep.second);
				sumEdgeSP = sumEdgeSP + globalG_[ep.first].weight;
				remEdges.push_back(*eit);
				addVerts.insert(targ);
				if(groupT_.g[targ].vertexState == STEINER_POINT)
				{
					foundSP = true;
					break;
				}
			}
			
			// do not add two neighboring steiner points
			if(foundSP) continue;
			
			// pluss edge weight from SP -> *vit
			ep = edge(*sit, *vit, globalG_);
			ASSERTING(ep.second);
			sumEdgeSP = sumEdgeSP + globalG_[ep.first].weight;

			double improvement = sumEdgeSP/sumEdges;
			if(bestImprovement > improvement) 
			{
				bestImprovement = improvement;
				bestRemEdges = remEdges;
				bestAddVerts = addVerts;
				bestVert = *vit;
			}			
		}
			
		//cerr << "Steiner Point " << *sit << " group member " << bestVert << " bestImprovement: " << bestImprovement << endl;
			
		if(bestImprovement < 1) 
		{
			// add steiner vertex
			groupT_.insertVertex(*sit, STEINER_POINT, CLIENT);				
				
			// add edges
			VertexSet::iterator vit_in = bestAddVerts.begin(), vit_in_end = bestAddVerts.end();
			for( ; vit_in != vit_in_end; ++vit_in)
			{
				groupT_.insertEdge(*sit, *vit_in, globalG_);
			}			
				
			// remove edges
			EdgeList::iterator eit = bestRemEdges.begin(), eit_end = bestRemEdges.end();
			for( ; eit != eit_end; ++eit)
			{
				groupT_.removeEdge(*eit);					
			}
		}			
	}
	
	cerr << WRITE_FUNCTION << endl;
	dumpTreeStructure(groupT_);
	dumpGraph(groupT_.g);
	char c = getchar();
}

/*-----------------------------------------------------------------------
 						Prune functions
------------------------------------------------------------------------- */
void 
ReconfigAlgs::pruneRTR(TreeStructure &T)
{
	VertexSet reconfigSet = T.S;

	while(reconfigSet.size())
	{
		//cerr << WRITE_FUNCTION << " steiner points " << reconfigSet << endl;

		VertexSet removeSP;
		// identify holes of Steiner points - apply RTR-like algorithm
		for(VertexSet::iterator vit = reconfigSet.begin(), vit_end = reconfigSet.end(); vit != vit_end; ++vit)
		{
			ASSERTING(get(&VertexProp::vertexState, T.g)[*vit] == STEINER_POINT);
			vertex_descriptorN actionVert_ = *vit;

			// variables 
			VertexSet 	adjacentSet;	 // only group members
			VertexSet 	adjacentSetSP;	 // only steiner points
			VertexSet	newSPSet;
			EdgeList	remEdges;
		
			adjacentSetSP.insert(actionVert_);
			getNeighborsOptimizeSP(T, actionVert_, actionVert_, adjacentSet, adjacentSetSP, remEdges);
			removeSP = removeSP + adjacentSetSP;
		
			//cerr << actionVert_ << " neighbor SP " << adjacentSetSP << " neighbor GM " << adjacentSet << endl;
			// feasibility test
			int ODAvNeighbors = getAvOutDegree(globalG_, T.g, adjacentSetSP + adjacentSet) + adjacentSetSP.size(); 
			int MinODNeeded = getMinODNeedConnectV(adjacentSet.size());
			
			//cerr << "OdAvNeighbors " << ODAvNeighbors << " MinODNeeded " << MinODNeeded << endl;

			if(adjacentSetSP.size() <= 1 || ODAvNeighbors < MinODNeeded) continue;
			//if(adjacentSetSP.empty() || ODAvNeighbors < MinODNeeded) continue;
			//cerr << "Optimizing in progress " << endl;			

			for(EdgeList::iterator eit = remEdges.begin(), eit_end = remEdges.end(); eit != eit_end; ++eit)
			{
				//cerr << *eit << " ";
				T.removeEdge(*eit);
			}
			//cerr << endl;

			VertexSet testVertices = adjacentSetSP;
			map<int, double> mapSumEdges;
            {
                VertexSet::iterator vit = adjacentSet.begin(), vit_end = adjacentSet.end(), vit_in, vit_in_end;
                for( ; vit != vit_end; ++vit)
                {
                    for(vit_in = testVertices.begin(), vit_in_end = testVertices.end(); vit_in != vit_in_end; ++vit_in)
                    {
                        ASSERTING(T.g[*vit_in].vertexState == STEINER_POINT);

                        if(*vit == *vit_in) continue;

                        pair<edge_descriptorN, bool> ep = edge(*vit, *vit_in, globalG_);
                        ASSERTING(ep.second);
                        mapSumEdges[*vit_in] = mapSumEdges[*vit_in] + globalG_[ep.first].weight;
                    }
                }
            }

			// next: identify the intersection vertices - rank nodes
			multimap<double, int> mapConnectV; 
			for(vit = testVertices.begin(), vit_end = testVertices.end(); vit != vit_end; ++vit)
				mapConnectV.insert(pair<double, int>(mapSumEdges[*vit], *vit));
			
			int sumODCapacity = 0;
			VertexSet connectV;
			for(multimap<double, int>::iterator mmit = mapConnectV.begin(), mmit_end = mapConnectV.end(); mmit != mmit_end; ++mmit)
			{
				int ODCapacity = getAvOutDegree(globalG_, T.g, mmit->second); //(getDegreeConstraint(T.g, mmit->second) - getOutDegree(T.g, mmit->second));
				//ASSERTING(ODCapacity == (getDegreeConstraint(T.g, mmit->second) - getOutDegree(T.g, mmit->second)));
				if(ODCapacity > 1)
				{
					connectV.insert(mmit->second);
					sumODCapacity += ODCapacity;
				}
	
				int connectInterconnectOD = ((connectV.size() - 2) * 2) + 2; 	
				if(sumODCapacity >= (adjacentSet.size() + connectInterconnectOD) ) 
					break; 
			}
			ASSERTING(connectV.size() > 0);
		
			//cerr << " connect vertices " << connectV << endl;

			// now: connectV is the absolute minimum and best set of vertices that we want to connect to adjacentSet
			// remove unused SPs 
			//cerr << " remove unused SPs " ;
			VertexSet remV = adjacentSetSP - connectV;
			for(vit = remV.begin(), vit_end = remV.end(); vit != vit_end; ++vit)
			{
				removeSP.insert(*vit);
				//cerr << *vit << "," ;
				ASSERTING(T.g[*vit].vertexState == STEINER_POINT);
				T.removeVertex(*vit);
			}
	
			VertexSet totConnect = connectV + adjacentSet;
			//cerr << " total to connect " << totConnect << endl;

			OutDegreeMap adjacentMap;
			for(VertexSet::iterator vit = totConnect.begin(), vit_end = totConnect.end(); vit != vit_end; ++vit)
				adjacentMap.insert(pair<int, vertex_descriptorN>(getAvOutDegree(globalG_, T.g, *vit), *vit));

			//cerr << "AdjacentMap: " << endl;
			//for(OutDegreeMap::iterator it = adjacentMap.begin(), it_end = adjacentMap.end(); it != it_end; ++it)
				//cerr << "V: " << it->second << " OD: " << it->first << endl;
			
			// connect again
			VertexSet connectedV;
			OutDegreeMap::iterator big = adjacentMap.begin();
			connectedV.insert(big->second); 
			adjacentMap.erase(big);
						
			connectMCE(T, adjacentMap, connectedV);
			pruneNTLeaves(T, connectV);
			break;
		}	
		reconfigSet = reconfigSet - removeSP;
	}
}

void 
ReconfigAlgs::pruneNTLeaves(TreeStructure &T, VertexSet &S)
{
	bool foundLeaf = true;
	while(foundLeaf)
	{
		foundLeaf = false;
		VertexSet pruneSet;
		for(VertexSet::const_iterator vit = S.begin(), vit_end = S.end(); vit != vit_end; ++vit)
		{
			ASSERTING(get(&VertexProp::vertexState, T.g)[*vit] == STEINER_POINT);
			if(getOutDegree(*vit, T.g) <= 1 && T.g[*vit].vertexState != GROUP_MEMBER) 
			{
				pruneSet.insert(*vit);
				foundLeaf = true;
			}
		}

		for(VertexSet::iterator vit = pruneSet.begin(), vit_end = pruneSet.end(); vit != vit_end; ++vit)
			T.removeVertex(*vit);

		S = S - pruneSet;
	}
}
/*
void
ReconfigAlgs::changeNTerminals(TreeStructure &T)
{
	// idea: for T.S until end ->
	// 		if out degree of t is high then search for replacement steiner points that reduces that total cost
	// 				
}
*/

